#include "mirror.h"

Mirror::Mirror() {}

bool Mirror::hasSpecular() const
{
    return true;
}

bool Mirror::hasTransmission() const
{
    return false;
}

bool Mirror::hasDiffuseOrGlossy() const
{
    return false;
}

bool Mirror::isEmissive() const
{
    return false; 
}

Vector3D Mirror::getReflectance(const Vector3D& n, const Vector3D& wo, const Vector3D& wi) const
{
    return n * 2 * (dot(n, wi)) - wi;
}

double Mirror::getIndexOfRefraction() const {
    return -1;
}

Vector3D Mirror::getEmissiveRadiance() const {
    return Vector3D(0);
}

Vector3D Mirror::getDiffuseReflectance() const {
    return Vector3D(0);
}

