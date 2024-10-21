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
                    }
                }
            }
            Vector3D diffuse_coeffitient = its.shape->getMaterial().getDiffuseReflectance(); // rho_d
            Vector3D ambient_constant = Vector3D(0.2); // Define the ambient constant value by eye
            Vector3D pixelColor = ambient_constant * diffuse_coeffitient + (Lo_dir / (N));
            return pixelColor;
        }
        else if (its.shape->getMaterial().hasTransmission()) {
            //First we need to know if the ray is changing the medium from eta1 to eta2
            //To this end we have to check if we are inside or outside the sphere. 
            //We know it for the dot product. 
            Vector3D wo = -r.d;
            double eta_mat = its.shape->getMaterial().getIndexOfRefraction();
            Vector3D normal = its.normal;
            //Check is we are inside
            if (dot(r.d, normal) > 0.0) {
                eta_mat = 1.0 / eta_mat; //If we are inside we compute the inverse of the density fraction
                normal = -normal;
            }
            double radicant = 1.0 - eta_mat * eta_mat * (1 - std::pow(dot(wo, normal), 2));
            if (radicant >= 0) {
                double parentesis = eta_mat * dot(normal, wo) - sqrt(radicant);
                Vector3D T = -wo * eta_mat + normal * parentesis;
                Ray ray_refraction;
                ray_refraction.o = its.itsPoint;
                ray_refraction.d = T.normalized();
                ray_refraction.depth = r.depth + 1;

                return computeColor(ray_refraction, objList, lsList);
            }
            else {//Total internal reflection, the readicant does not exist
                Ray ray_reflection;
                ray_reflection.o = its.itsPoint;
                Vector3D wr = normal * 2 * (dot(normal, wo)) - wo;
                ray_reflection.d = wr.normalized();
                ray_reflection.depth = r.depth + 1;
                return computeColor(ray_reflection, objList, lsList);
            }
        }
    }
    return bgColor;
}



Vector3D HDIShader::computeColor_v2(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
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
            int max_depth = 4;
            int N = 256; // Number of samples to iterate
            for (LightSource* light : lsList) {
				AreaLightSource* area_light = dynamic_cast<AreaLightSource*>(light);
                Vector3D random_point = area_light->generateRandomPoint();  // y_i
                double area = light->getArea();
				double probability = 1 / area;
				Vector3D wi = random_point - its.itsPoint; //Incident light direction
				Vector3D wi_normalized = wi.normalized();
				double distance = wi.lengthSq();
                double geometric_term = ((dot(wi_normalized, n) * dot(-wi_normalized, n))) / distance;

                for (int i = 1; i < N; i++) {
                    wj = hemisphericalSampler.getSample(n); // Get a random direction, normalized by default
                    wj_normalized = wj.normalized();
                    Vector3D reflectance = its.shape->getMaterial().getReflectance(n, wj_normalized, wo.normalized()); //Using the formula from the Phong model

                    //Create a ray from the hit point to the random direction
                    Ray random_ray = Ray(its.itsPoint, wj, 0, Epsilon);
                    Intersection random_its; //Initialize intersection to get the hit point
                    bool random_intersected = Utils::getClosestIntersection(random_ray, objList, random_its);
                    if (random_intersected) {
                        if (random_its.shape->getMaterial().isEmissive()) {
                            //Check if the point is visible
                            Ray shadowRay = Ray(its.itsPoint, wi_normalized, 0, Epsilon, wi.length());
                            bool not_visible = Utils::hasIntersection(shadowRay, objList);
                            if (not_visible) Li = 0; // Set to 0 when the shadow ray intersects, meaning the light is blocked
                            // When the point is visible, calculate the direct ilumination with the given formula
                            else Li = random_its.shape->getMaterial().getEmissiveRadiance() * reflectance * dot(wj_normalized, n) * geometric_term / probability;
                            Lo_dir = Lo_dir + Li;
                        }
                    }
                }
            }
            Vector3D pixelColor = (Lo_dir / (N));
            return pixelColor;
        }
        else if (its.shape->getMaterial().hasTransmission()) {
            //First we need to know if the ray is changing the medium from eta1 to eta2
            //To this end we have to check if we are inside or outside the sphere. 
            //We know it for the dot product. 
            Vector3D wo = -r.d;
            double eta_mat = its.shape->getMaterial().getIndexOfRefraction();
            Vector3D normal = its.normal;
            //Check is we are inside
            if (dot(r.d, normal) > 0.0) {
                eta_mat = 1.0 / eta_mat; //If we are inside we compute the inverse of the density fraction
                normal = -normal;
            }
            double radicant = 1.0 - eta_mat * eta_mat * (1 - std::pow(dot(wo, normal), 2));
            if (radicant >= 0) {
                double parentesis = eta_mat * dot(normal, wo) - sqrt(radicant);
                Vector3D T = -wo * eta_mat + normal * parentesis;
                Ray ray_refraction;
                ray_refraction.o = its.itsPoint;
                ray_refraction.d = T.normalized();
                ray_refraction.depth = r.depth + 1;

                return computeColor(ray_refraction, objList, lsList);
            }
            else {//Total internal reflection, the readicant does not exist
                Ray ray_reflection;
                ray_reflection.o = its.itsPoint;
                Vector3D wr = normal * 2 * (dot(normal, wo)) - wo;
                ray_reflection.d = wr.normalized();
                ray_reflection.depth = r.depth + 1;
                return computeColor(ray_reflection, objList, lsList);
            }
        }
    }
    return bgColor;
}
