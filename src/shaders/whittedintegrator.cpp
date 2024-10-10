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
