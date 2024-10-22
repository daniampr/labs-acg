#ifndef HDISHADER_H
#define HDISHADER_H

#include <vector>
#include "shader.h"
#include "../core/ray.h"
#include "../core/hemisphericalsampler.h"

class HDIShader : public Shader
{
public:
	HDIShader();
	HDIShader(Vector3D bgColor_);
	virtual Vector3D computeColor(const Ray& r,
		const std::vector<Shape*>& objList,
		const std::vector<LightSource*>& lsList) const;

	HemisphericalSampler hemisphericalSampler = HemisphericalSampler();
};

#endif