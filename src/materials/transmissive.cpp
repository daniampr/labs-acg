#include "transmissive.h"

#include <iostream>




Transmissive::Transmissive() { eta = 1.1; };

Transmissive::Transmissive(double eta_) : eta(eta_) { };

Vector3D  Transmissive::getReflectance(const Vector3D& n, const Vector3D& wo,
    const Vector3D& wi) const {
    return Vector3D(0.0);
};

double Transmissive::getIndexOfRefraction() const
{
    return eta;
}

Vector3D Transmissive::getEmissiveRadiance() const
{
    return Vector3D(0.0);
}

