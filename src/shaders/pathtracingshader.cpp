#include "pathtracingshader.h"
#include "../core/utils.h"
#include "../materials/material.h"
#include "../materials/mirror.h"

// Constructor with default background color
PathTracingShader::PathTracingShader() : Shader(Vector3D(0.0)) {}

// Constructor with custom background color
PathTracingShader::PathTracingShader(Vector3D bgColor_) : Shader(bgColor_) {}

Vector3D PathTracingShader::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    Intersection its; // Initialize the intersection object
    bool intersects = Utils::getClosestIntersection(r, objList, its); // Check if there's an intersection
    if (!intersects) return bgColor; // If no intersection, return background color

    const Material* material = &its.shape->getMaterial(); // Get the material of the intersected object
    Vector3D wo = -r.d; // Outgoing light direction
    Vector3D Le = material->getEmissiveRadiance(); // Emissive radiance from the material

    // If the material is emissive, return its radiance directly
    if (material->isEmissive()) return Le;

    // Recursively trace rays for indirect illumination (reflection/refraction)
    if (r.depth < maxDepth) {
        // For reflective surfaces
        if (material->hasSpecular()) {
            Vector3D wr = material->getReflectance(its.normal, wo, wo); // Calculate reflection direction
            Ray reflectedRay = Ray(its.itsPoint, wr, r.depth + 1);
            return computeColor(reflectedRay, objList, lsList); // Recursive call for reflection
        }
        // For transmissive surfaces
        else if (material->hasTransmission()) {
            // Refraction/reflection logic for transmissive materials
            Vector3D normal = its.normal;
            double eta_mat = material->getIndexOfRefraction();

            // Check if the ray is inside or outside
            if (dot(r.d, normal) > 0.0) {
                eta_mat = 1.0 / eta_mat; // Invert eta if inside
                normal = -normal;
            }

            // Calculate the refraction vector
            Vector3D wo = -r.d;
            double radicant = 1.0 - eta_mat * eta_mat * (1 - std::pow(dot(wo, normal), 2));

            if (radicant >= 0) { // Refraction case
                double refractionFactor = eta_mat * dot(normal, wo) - sqrt(radicant);
                Vector3D T = -wo * eta_mat + normal * refractionFactor;
                Ray refractedRay = Ray(its.itsPoint, T.normalized(), r.depth + 1);
                return computeColor(refractedRay, objList, lsList);
            }
            else { // Total internal reflection case
                Vector3D wr = normal * 2 * dot(normal, wo) - wo;
                Ray reflectedRay = Ray(its.itsPoint, wr.normalized(), r.depth + 1);
                return computeColor(reflectedRay, objList, lsList);
            }
        }
        // For diffuse/glossy surfaces
        else if (material->hasDiffuseOrGlossy()) {
            Vector3D Lo = Vector3D(0.0); // Initialize the accumulated color
            Vector3D wi = hemisphericalSampler.getSample(its.normal); // Sample a direction in the hemisphere
            Ray newRay = Ray(its.itsPoint, wi, r.depth + 1); // Create the new ray for this direction
            Vector3D indirectIllumination = computeColor(newRay, objList, lsList); // Recursively trace

            // Compute reflectance and accumulate light
            Vector3D reflectance = material->getReflectance(its.normal, wi, wo);
            Lo += indirectIllumination * reflectance * dot(wi, its.normal) / pdf; // Monte Carlo integration

            return Lo; // Return accumulated color
        }
    }

    // If no intersection or recursion limit reached, return background color
    return Vector3D(0.0, 0.0, 0.0);
}

void PathTracingShader::raytrace(Camera*& cam, Shader*& shader, Film*& film, std::vector<Shape*>*& objectsList, std::vector<LightSource*>*& lightSourceList) const {
    size_t resX = film->getWidth();
    size_t resY = film->getHeight();
    int ssp = N;  // Samples per pixel

    // Main raytracing loop
    for (size_t lin = 0; lin < resY; lin++) {
        for (size_t col = 0; col < resX; col++) {
            Vector3D pixelColor = Vector3D(0.0);  // Initialize pixel color outside the sampling loop
            double progress = (double)lin / double(resY);
            Utils::printProgress(progress);

            // Accumulate color over multiple samples per pixel
            for (int i = 0; i < ssp; i++) {
                // Compute the pixel position in NDC with jittering for anti-aliasing
                double x = (double)(col + 0.5) / resX;
                double y = (double)(lin + 0.5) / resY;

                // Generate the camera ray
                Ray cameraRay = cam->generateRay(x, y);

                // Accumulate color from the current sample
                pixelColor += shader->computeColor(cameraRay, *objectsList, *lightSourceList);
            }

            // Average the color over the number of samples
            pixelColor /= (double)ssp;

            // Store the averaged pixel color in the film
            film->setPixelValue(col, lin, pixelColor);
        }
    }
}
