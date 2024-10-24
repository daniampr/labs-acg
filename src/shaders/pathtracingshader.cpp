#include "pathtracingshader.h"
#include "../core/utils.h"
#include "../materials/material.h"
#include "../materials/mirror.h"


// Constructor with default background color
PathTracingShader::PathTracingShader() : Shader(Vector3D(0.0)) {}

// Constructor with custom background color
PathTracingShader::PathTracingShader(Vector3D bgColor_) : Shader(bgColor_) {}


Vector3D PathTracingShader::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    Intersection its; // Initialize the intersection object
    bool intersects = Utils::getClosestIntersection(r, objList, its); // Check if there's an intersection
    if (!intersects) return bgColor; // If no intersection, return background color

    // Check if there's an intersection
    const Material* material = &its.shape->getMaterial(); // Get the material of the intersected object
    // Only proceed with shading if there is a valid intersection
    LightSource* light = lsList[0]; // Assuming only one light source in the scene

    // Obtain values from the intersection
    Vector3D x = its.itsPoint;      // Intersection point
    Vector3D n = its.normal;        // Surface normal at the intersection
    Vector3D Le = its.shape->getMaterial().getEmissiveRadiance(); // Emissive radiance from the material
    Vector3D wi = hemisphericalSampler.getSample(n); // Sample a direction based on the normal
    Vector3D Lo = Vector3D(0.0); // Initialize the function
    Ray newR = Ray(x, wi, r.depth + 1); // Create a new ray with increased depth
    // Recursively trace rays for indirect illumination (reflection/refraction)
    if (r.depth < maxDepth) {
        if (material->isEmissive()) {
            Lo += Le;
            pixelColor =  Lo; // Return the computed radiance
        }
        else if (material->hasSpecular()) {
            Vector3D wo = -r.d; // Outgoing light direction
            Vector3D wr = material->getReflectance(n, wo, wo); //Calculate the reflection direction
            Ray reflectedRay = Ray(its.itsPoint, wr, r.depth + 1);
            Vector3D reflectedColor = computeColor(reflectedRay, objList, lsList); //Recursive call to get the reflected color
                
            pixelColor = reflectedColor;
        }
        else if (material->hasTransmission()) {
            //First we need to know if the ray is changing the medium from eta1 to eta2
            //To this end we have to check if we are inside or outside the sphere. 
            //We know it for the dot product. 
            Vector3D wo = -r.d;
            double eta_mat = its.shape->getMaterial().getIndexOfRefraction();
            Vector3D normal = its.normal;
            //Check is we are inside
            if (dot(r.d, normal) > 0.0) {
                eta_mat = 1.0 / eta_mat; //If we are inside we compute the inverse of the density fraction
                normal = -normal;
            }
            double radicant = 1.0 - eta_mat * eta_mat * (1 - std::pow(dot(wo, normal), 2));
            if (radicant >= 0) {
                double parentesis = eta_mat * dot(normal, wo) - sqrt(radicant);
                Vector3D T = -wo * eta_mat + normal * parentesis;
                Ray ray_refraction;
                ray_refraction.o = its.itsPoint;
                ray_refraction.d = T.normalized();
                ray_refraction.depth = r.depth + 1;
                return computeColor(ray_refraction, objList, lsList);
            }
            else {//Total internal reflection, the readicant does not exist
                Ray ray_reflection;
                ray_reflection.o = its.itsPoint;
                Vector3D wr = normal * 2 * (dot(normal, wo)) - wo;
                ray_reflection.d = wr.normalized();
                ray_reflection.depth = r.depth + 1;
                return computeColor(ray_reflection, objList, lsList);
            }
        }
        else if (material->hasDiffuseOrGlossy()) {
            Vector3D Lo_dir = Vector3D(0.0); // Initialize
            Vector3D Li = Vector3D(0.0); // Initialize
            Vector3D wj = Vector3D(0.0); // Initialize
            Vector3D wj_normalized = Vector3D(0.0); // Initialize
            Vector3D Le = Vector3D(0.0); // Initialize
            Vector3D wo = -r.d; // Outgoing light direction
            AreaLightSource* area_light = dynamic_cast<AreaLightSource*>(light);
            double area = light->getArea();
            double probability = 1 / area;
            Le = light->getIntensity(); // L_e(y_i)
            Vector3D n_y = light->getNormal();
            Vector3D random_point = light->sampleLightPosition();  // y_i: Random point in the light source
            wj = random_point - its.itsPoint; //Incident light direction
            wj_normalized = wj.normalized();
            double distance = wj.lengthSq();
            double geometric_term = ((dot(wj_normalized, n) * dot(-wj_normalized, n_y))) / distance;
            Vector3D reflectance = its.shape->getMaterial().getReflectance(n, wj_normalized, wo.normalized()); //Using the formula from the Phong model
            //Check if the point is visible
            Ray shadowRay = Ray(its.itsPoint, wj_normalized, 0, Epsilon, wj.length());
            bool not_visible = Utils::hasIntersection(shadowRay, objList);
            if (not_visible) Li = 0;
            else Li = Le * reflectance * dot(wj_normalized, n) * geometric_term / probability;
            Lo_dir = Lo_dir + Li;
            Vector3D diffuse_coeffitient = its.shape->getMaterial().getDiffuseReflectance(); // rho_d
            Vector3D ambient_constant = Vector3D(0.2); // Define the ambient constant value by eye
            pixelColor = ambient_constant * diffuse_coeffitient + (Lo_dir / (N));
        }
        pixelColor += computeColor(newR, objList, lsList) * 
            its.shape->getMaterial().getReflectance(n, wi, -r.d) *
            dot(wi, n) / pdf; // Reflectance and cosine-weighted contribution;
    }
    return pixelColor;


    // If no intersection is found, return background color (black in this case)
    return Vector3D(0.0, 0.0, 0.0);
}




void PathTracingShader::raytrace(Camera*& cam, Shader*& shader, Film*& film, std::vector<Shape*>*& objectsList, std::vector<LightSource*>*& lightSourceList) const {
    size_t resX = film->getWidth();
    size_t resY = film->getHeight();
    int ssp = N;  // Samples per pixel

    // Main raytracing loop
    for (size_t lin = 0; lin < resY; lin++) {
        for (size_t col = 0; col < resX; col++) {
            Vector3D pixelColor = Vector3D(0.0);  // Initialize pixel color outside the sampling loop
            double progress = (double)lin / double(resY);
            Utils::printProgress(progress);
            // Accumulate color over multiple samples per pixel
            for (int i = 0; i < ssp; i++) {
                // Compute the pixel position in NDC with jittering for anti-aliasing
                double x = (double)(col + 0.5) / resX;
                double y = (double)(lin + 0.5) / resY;

                // Generate the camera ray
                Ray cameraRay = cam->generateRay(x, y);

                // Accumulate color from the current sample
                pixelColor += shader->computeColor(cameraRay, *objectsList, *lightSourceList);
            }

            // Average the color over the number of samples
            pixelColor /= (double)ssp;

            // Store the averaged pixel color in the film
            film->setPixelValue(col, lin, pixelColor);
        }
    }
}
