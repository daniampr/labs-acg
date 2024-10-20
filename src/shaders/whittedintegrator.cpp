#include "whittedintegrator.h"
#include "../core/utils.h"
#include "../materials/material.h"
#include "../materials/mirror.h"

// Constructor with default background color
WhittedIntegrator::WhittedIntegrator() : Shader(Vector3D(0.0)) {}

// Constructor with custom background color
WhittedIntegrator::WhittedIntegrator(Vector3D bgColor_) : Shader(bgColor_) {}

Vector3D WhittedIntegrator::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    
    Intersection its; //Initialize intersection to get the hit point
    bool intersects = Utils::getClosestIntersection(r, objList, its);
    Vector3D n = its.normal;
    Vector3D Lo = Vector3D(0.0); // Initialize the function
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

		// When the material is not specular, check if it has a diffuse or glossy component
		// Compute the direct ilumination using the Phong model -- 5.2
        else if (material->hasDiffuseOrGlossy()) {
            for (LightSource* light : lsList) {
                Vector3D incident_light = light->getIntensity();
                Vector3D wi = light->sampleLightPosition() - its.itsPoint; //Incident light direction
                Vector3D wi_normalized = wi.normalized();
                Vector3D reflectance = its.shape->getMaterial().getReflectance(n, wo, wi_normalized); //Using the formula from the Phong model

                //Check if the point is visible
                Ray shadowRay = Ray(its.itsPoint, wi_normalized, 0, Epsilon, wi.length());
                bool not_visible = Utils::hasIntersection(shadowRay, objList);
                if (not_visible) direct_ilumination = 0; // Set to 0 when the shadow ray intersects, meaning the light is blocked

                // When the point is visible, calculate the direct ilumination with the given formula
                else direct_ilumination = incident_light * reflectance * dot(wi_normalized, n);
                Lo = Lo.operator+(direct_ilumination);
            }
            // Add the constants to increase or reduce the ambient light
            Vector3D diffuse_coeffitient = its.shape->getMaterial().getDiffuseReflectance(); // rho_d
            Vector3D ambient_constant = Vector3D(0.2); // Define the ambient constant value by eye
            Vector3D pixelColor = diffuse_coeffitient * ambient_constant + Lo;

            return pixelColor;
        }
    }
    return bgColor;  
}
