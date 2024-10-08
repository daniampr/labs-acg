#include "whittedintegrator.h"
#include "../core/utils.h"

// Constructor with default background color
WhittedIntegrator::WhittedIntegrator() : Shader(Vector3D(0.0)) {}

// Constructor with custom background color
WhittedIntegrator::WhittedIntegrator(Vector3D bgColor_) : Shader(bgColor_) {}

Vector3D WhittedIntegrator::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    
    Intersection its; //Initialize intersection to get the hit point
    bool intersects = Utils::getClosestIntersection(r, objList, its);
    Vector3D n = its.normal;
    Vector3D Lo = Vector3D(0.0);
    Vector3D direct_ilumination;

    if (intersects) {
        
        for (LightSource* light : lsList) {
            PointLightSource* Point_light = (PointLightSource*)light;
            Vector3D incident_light = Point_light->getIntensity();
            Vector3D wi = Point_light->sampleLightPosition() - its.itsPoint;
            Vector3D wi_normalized = wi.normalized();
            Vector3D wo = -r.d;
            Vector3D reflectance = its.shape->getMaterial().getReflectance(n, wo, wi_normalized);
            Ray shadowRay = Ray(its.itsPoint, wi_normalized, 0, Epsilon, wi.length());
            bool visible = Utils::hasIntersection(shadowRay, objList);
            if (visible) direct_ilumination = 0;
            else direct_ilumination = incident_light * reflectance * dot(wi_normalized, n);
            Lo = Lo.operator+(direct_ilumination);
        }
        Vector3D diffuse_coeffitient = its.shape->getMaterial().getDiffuseReflectance();
        Vector3D ambient_constant = Vector3D(0.2);
        Vector3D pixelColor = diffuse_coeffitient * ambient_constant + Lo;
        return pixelColor;
    }
    return bgColor;
    
    

    
}
