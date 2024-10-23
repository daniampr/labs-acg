#include "pathtracinghader.h"
#include "../core/utils.h"
#include "../materials/material.h"
#include "../materials/mirror.h"

// Constructor with default background color
PathTracingShader::PathTracingShader() : Shader(Vector3D(0.0)) {}

// Constructor with custom background color
PathTracingShader::PathTracingShader(Vector3D bgColor_) : Shader(bgColor_) {}


Vector3D PathTracingShader::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{

	Intersection its; //Initialize intersection to get the hit point
	Utils::getClosestIntersection(r, objList, its); // Update the intersection object with the closest one

	LightSource* light = lsList[0]; // In the current framework we only have one light!!
	
	// Obtain values from the intersection
	Vector3D x = its.itsPoint;
	Vector3D n = its.normal;
	Vector3D Lo = its.shape->getMaterial().getEmissiveRadiance();
	Vector3D wi = hemisphericalSampler.getSample(n);

	if (r.depth > maxDepth) {
		Ray newR = Ray(x, wi, r.depth + 1);
		Lo += computeColor(newR, objList, lsList) * its.shape->getMaterial().getReflectance(n, wi, -r.d) * dot(wi, n) / pdf;
	}
	return Lo;
}

void PathTracingShader::raytrace(Camera*& cam, Shader*& shader, Film*& film, std::vector<Shape*>*& objectsList, std::vector<LightSource*>*& lightSourceList) const
{
    size_t resX = film->getWidth();
    size_t resY = film->getHeight();

    // Main raytracing loop
    // Out-most loop invariant: we have rendered lin lines
    for (size_t lin = 0; lin < resY; lin++)
    {
        // Inner loop invariant: we have rendered col columns
        for (size_t col = 0; col < resX; col++)
        {
            // Compute the pixel position in NDC
            double x = (double)(col + 0.5) / resX;
            double y = (double)(lin + 0.5) / resY;
            // Generate the camera ray
            Ray cameraRay = cam->generateRay(x, y);
            Vector3D pixelColor = Vector3D(0.0);
            pixelColor += shader->computeColor(cameraRay, *objectsList, *lightSourceList);

            // Store the pixel color
            film->setPixelValue(col, lin, pixelColor);
        }
    }
}