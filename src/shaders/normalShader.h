#ifndef NORMALSHADER_H
#define NORMALSHADER_H

#include "shader.h"
// Content based on the IntersectionShader with according changes
class NormalShader : public Shader
{
public:
    NormalShader();
    NormalShader(Vector3D hitColor, Vector3D bgColor_);

    virtual Vector3D computeColor(const Ray& r,
        const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const;

    Vector3D normalColor;  // Change done comparing with IntersectionShader
};

#endif // NormalSHADER_H
