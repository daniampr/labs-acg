#include "hdishader.h"
#include "../core/utils.h"
#include "../materials/material.h"
#include "../materials/mirror.h"
#include "../core/hemisphericalsampler.h"
#define M_PI 3.14159265358979323846


// Constructor with default background color
HDIShader::HDIShader() : Shader(Vector3D(0.0)) {}

// Constructor with custom background color
HDIShader::HDIShader(Vector3D bgColor_) : Shader(bgColor_) {}

Vector3D HDIShader::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    Intersection its; //Initialize intersection to get the hit point
    bool intersects = Utils::getClosestIntersection(r, objList, its);
    Vector3D n = its.normal;
    Vector3D Lo_dir = Vector3D(0.0); // Initialize the function
    Vector3D direct_ilumination; // Aux variable
    Vector3D wo = -r.d; //Outgoing light direction

    if (intersects) {
        const Material* material = &its.shape->getMaterial();

        // Check if the material has specular reflection meaning it is a mirror
        if (material->hasSpecular()) {
            Vector3D wr = material->getReflectance(n, wo, wo); //Calculate the reflection direction
            Ray reflectedRay = Ray(its.itsPoint, wr, r.depth + 1);
            Vector3D reflectedColor = computeColor(reflectedRay, objList, lsList); //Recursive call to get the reflected color
            return reflectedColor;
        }
        else if (material->isEmissive()) {
            return material->getEmissiveRadiance();
        }

        // When the material is not specular, check if it has a diffuse or glossy component
        // Compute the direct ilumination using the Phong model -- 5.2
        else if (material->hasDiffuseOrGlossy()) {
            Vector3D Lo_dir = Vector3D(0.0); // Initialize
            Vector3D Li = Vector3D(0.0); // Initialize
			Vector3D wj = Vector3D(0.0); // Initialize
			Vector3D wj_normalized = Vector3D(0.0); // Initialize
            int N = 256; // Number of samples to iterate
            double probability = 1 / (2 * M_PI); // rho_d
            for (LightSource* light : lsList) {
                for (int i = 1; i < N; i++) {
					wj = hemisphericalSampler.getSample(n); // Get a random direction, normalized by default
					wj_normalized = wj.normalized();
                    Vector3D reflectance = its.shape->getMaterial().getReflectance(n, wj_normalized, wo.normalized()); //Using the formula from the Phong model

                    //Create a ray from the hit point to the random direction
					Ray random_ray = Ray(its.itsPoint, wj, 0, Epsilon); 
					Intersection random_its; //Initialize intersection to get the hit point
                    bool random_intersected = Utils::getClosestIntersection(random_ray, objList, random_its);
                    if (random_intersected){
                        if (random_its.shape->getMaterial().isEmissive()) {
                            Li = random_its.shape->getMaterial().getEmissiveRadiance() * reflectance * dot(wj_normalized, n) / probability;
                            Lo_dir = Lo_dir + Li;
                        }
                        /*
                        else{
                            Li = (random_its.shape->getMaterial().getEmissiveRadiance() * reflectance * dot(wj_normalized, n)) / probability;
                            Lo_dir = Lo_dir.operator+(Li);
                        }
                        */
                    }
                }
            }           
            double ambient_constant = 0.2;
            Vector3D pixelColor =  (Lo_dir / (N));
            return pixelColor;
        }
    }
    return bgColor;
}
