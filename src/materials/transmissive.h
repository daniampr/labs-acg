#ifndef TRANSMISSIVEMATERIAL
#define TRANSMISSIVEMATERIAL

#include "material.h"

class Transmissive : public Material
{
public:
    Transmissive() ;
    Transmissive(double eta_);

    Vector3D getReflectance(const Vector3D& n, const Vector3D& wo,
        const Vector3D& wi) const ;
    bool hasSpecular() const { return false; }
    bool hasTransmission() const { return true; }
    bool hasDiffuseOrGlossy() const { return false; }
    bool isEmissive() const { return false; }
    double getIndexOfRefraction() const;
    Vector3D getEmissiveRadiance() const;
private:
    double eta;//Fraction of eta1/eta2 

};


#endif // MATERIAL