#include "normalshader.h"
#include "../core/utils.h"

NormalShader::NormalShader() :
    normalColor(Vector3D(1, 0, 0))
{ }

NormalShader::NormalShader(Vector3D normalColor_, Vector3D bgColor_) :
    Shader(bgColor_), normalColor(normalColor_)
{ }

Vector3D NormalShader::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    // Initialize the color
    Vector3D pixelColor;

    Intersection its; //Initialize intersection to get the hit point
    
    // Compute ray color according to the used shader
    bool intersects = Utils::getClosestIntersection(r, objList, its);

    if (intersects) {
        
        pixelColor = (its.normal.operator+(Vector3D(1.0, 1.0, 1.0))) / 2; // Using formula provided
        return pixelColor;
        
    }

    return bgColor;
}
