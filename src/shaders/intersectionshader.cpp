#include "intersectionshader.h"
#include "../core/utils.h"

IntersectionShader::IntersectionShader() :
    hitColor(Vector3D(1, 0, 0))
{ }

IntersectionShader::IntersectionShader(Vector3D hitColor_, Vector3D bgColor_) :
    Shader(bgColor_), hitColor(hitColor_)
{ }

Vector3D IntersectionShader::computeColor(const Ray &r, const std::vector<Shape*> &objList, const std::vector<LightSource*> &lsList) const
{
	// Initialize the color
	Vector3D pixelColor;

    // Compute ray color according to the used shader
	bool intersects = Utils::hasIntersection(r, objList); //Task 2

    if (intersects) {   
        pixelColor = Vector3D(1.0, 0.0, 0.0); //red pixel
		return pixelColor;
    }
    
    return bgColor;
}
