#ifndef PATHTRACINGSHADER_H
#define PATHTRACINGSHADER_H

#include "../core/ray.h"
#include "../core/film.h"
#include "../materials/material.h"
#include "../core/intersection.h"
#include "../core/hemisphericalsampler.h"
#include "../shaders/shader.h"
#include "../core/scene.h"
#include "../cameras/camera.h"

#define M_PI 3.14159265358979323846


class PathTracingShader : public Shader {
public:
    PathTracingShader();
    PathTracingShader(Vector3D bgColor_);


    virtual Vector3D computeColor(const Ray& r,
        const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const;

    // Main function to trace rays and accumulate color for each pixel
    virtual void raytrace(Camera*& cam, Shader*& shader, Film*& film,
        std::vector<Shape*>*& objectsList, std::vector<LightSource*>*& lightSourceList) const;
    int maxDepth = 4;
    int N = 256; // Samples per pixel
	double pdf = 1 / (2 * M_PI);
    HemisphericalSampler hemisphericalSampler = HemisphericalSampler();

};

#endif // PATHTRACINGSHADER_H