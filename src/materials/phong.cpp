#include "phong.h"

#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


Phong::Phong()
{ }

Phong::Phong(Vector3D Kd_, Vector3D Ks_, float alpha_):
rho_d(Kd_), Ks(Ks_), alpha(alpha_){}


Vector3D Phong::getReflectance(const Vector3D& n, const Vector3D& wo,
    const Vector3D& wi) const {
    
    // TASK 5.1
    // First we compute the ideal reflection direction with the formula
    Vector3D wr = n * 2 * dot(n, wi) - wi; // we do this order to avoid c++ bugs

	Vector3D diffuse = (rho_d / M_PI); // Diffuse color, substituted rho_d for Kd in LAB2
	double new_param_lab2 = 2 * M_PI / (alpha + 1); // New parameter for the Phong model
    Vector3D specular =  Ks * pow(dot(wo, wr), alpha) * new_param_lab2;
    Vector3D reflectance = diffuse + specular;

    return reflectance;
};

double Phong::getIndexOfRefraction() const
{
    std::cout << "Warning! Calling \"Material::getIndexOfRefraction()\" for a non-transmissive material"
              << std::endl;

    return -1;
}


Vector3D Phong::getEmissiveRadiance() const
{
    return Vector3D(0.0);
}


Vector3D Phong::getDiffuseReflectance() const
{
    return rho_d;
}

