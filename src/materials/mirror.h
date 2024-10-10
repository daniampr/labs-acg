#ifndef MIRROR_H
#define MIRROR_H

#include "../core/vector3d.h"
#include "material.h"

class Mirror : public Material
{
public:
    Mirror();

    virtual Vector3D getReflectance(const Vector3D& n, const Vector3D& wo,
        const Vector3D& wi) const override;

    virtual double getIndexOfRefraction() const override;
    virtual Vector3D getEmissiveRadiance() const override;
    virtual Vector3D getDiffuseReflectance() const override;

    
    virtual bool hasSpecular() const override;
    virtual bool hasTransmission() const override;
    virtual bool hasDiffuseOrGlossy() const override;
    virtual bool isEmissive() const override;
};

#endif // MIRROR_H
