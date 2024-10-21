#ifndef ADISHADER_H
#define ADISHADER_H

#include <vector>
#include "shader.h"
#include "../core/ray.h"
#include "../core/hemisphericalsampler.h"

class ADIShader : public Shader
{
public:
	ADIShader();
	ADIShader(Vector3D bgColor_);
	virtual Vector3D computeColor(const Ray& r,
		const std::vector<Shape*>& objList,
		const std::vector<LightSource*>& lsList) const;
};
#endif