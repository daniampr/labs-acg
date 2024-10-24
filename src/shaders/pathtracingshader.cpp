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

    // Check if there's an intersection
    if (Utils::getClosestIntersection(r, objList, its)) {
        // Only proceed with shading if there is a valid intersection

        LightSource* light = lsList[0]; // Assuming only one light source in the scene

        // Obtain values from the intersection
        Vector3D x = its.itsPoint;      // Intersection point
        Vector3D n = its.normal;        // Surface normal at the intersection
        Vector3D Lo = its.shape->getMaterial().getEmissiveRadiance(); // Emissive radiance from the material
        Vector3D wi = hemisphericalSampler.getSample(n); // Sample a direction based on the normal

        // Recursively trace rays for indirect illumination (reflection/refraction)
        if (r.depth < maxDepth) {
            Ray newR = Ray(x, wi, r.depth + 1); // Create a new ray with increased depth
            Lo += computeColor(newR, objList, lsList) *
                its.shape->getMaterial().getReflectance(n, wi, -r.d) *
                dot(wi, n) / pdf; // Reflectance and cosine-weighted contribution
        }

        return Lo; // Return the computed radiance
    }

    // If no intersection is found, return background color (black in this case)
    return Vector3D(0, 0, 0);
}



//
//void PathTracingShader::raytrace(Camera*& cam, Shader*& shader, Film*& film, std::vector<Shape*>*& objectsList, std::vector<LightSource*>*& lightSourceList) const {
//    size_t resX = film->getWidth();
//    size_t resY = film->getHeight();
//    int ssp = N;  // Samples per pixel
//
//    // Main raytracing loop
//    for (size_t lin = 0; lin < resY; lin++) {
//        for (size_t col = 0; col < resX; col++) {
//            Vector3D pixelColor = Vector3D(0.0);  // Initialize pixel color outside the sampling loop
//
//            // Accumulate color over multiple samples per pixel
//            for (int i = 0; i < ssp; i++) {
//                // Compute the pixel position in NDC with jittering for anti-aliasing
//                double x = (double)(col + 0.5) / resX;
//                double y = (double)(lin + 0.5) / resY;
//
//                // Generate the camera ray
//                Ray cameraRay = cam->generateRay(x, y);
//
//                // Accumulate color from the current sample
//                pixelColor += shader->computeColor(cameraRay, *objectsList, *lightSourceList);
//            }
//
//            // Average the color over the number of samples
//            pixelColor /= (double)ssp;
//
//            // Store the averaged pixel color in the film
//            film->setPixelValue(col, lin, pixelColor);
//        }
//    }
//}
