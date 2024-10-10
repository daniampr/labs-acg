#include "depthshader.h"
#include "../core/utils.h"
#include <iostream>

DepthShader::DepthShader() :
    color(Vector3D(1, 0, 0))
{ }

DepthShader::DepthShader(Vector3D hitColor_, double maxDist_, Vector3D bgColor_) :
    Shader(bgColor_), maxDist(maxDist_), color(hitColor_)
{ }

Vector3D DepthShader::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    // Initialize the color
    Vector3D pixelColor;

	Intersection its; //Initialize intersection to get the hit point

    // Compute ray color according to the used shader
    bool intersects = Utils::getClosestIntersection(r, objList, its); //Task 3

    if (intersects) {
		double hitDistance = its.itsPoint.length();
        pixelColor = std::max(1 - hitDistance / maxDist, 0.0); //using the formula shown
		return color * pixelColor; // multiply to keep the pixel value between 0 and 1
    }
    
    return bgColor;
}