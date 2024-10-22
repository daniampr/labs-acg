#include "arealightsource.h"
#include "../core/utils.h"
#include <iostream>
#include <random>



AreaLightSource::AreaLightSource(Square* areaLightsource_) :
    myAreaLightsource(areaLightsource_)
{ }



Vector3D AreaLightSource::getIntensity() const
{
    return myAreaLightsource->getMaterial().getEmissiveRadiance();
}


Vector3D AreaLightSource::sampleLightPosition()   const
{
    // Generate two random numbers between 0 and 1 for interpolation
    double u = ((double)rand()) / RAND_MAX;  // Random factor for the first vector
    double v = ((double)rand()) / RAND_MAX;  // Random factor for the second vector

    // Calculate the coordinates of the random point within the square
    double x = myAreaLightsource->corner.x + u * myAreaLightsource->v1.x + v * myAreaLightsource->v2.x;
    double y = myAreaLightsource->corner.y + u * myAreaLightsource->v1.y + v * myAreaLightsource->v2.y;
    double z = myAreaLightsource->corner.z + u * myAreaLightsource->v1.z + v * myAreaLightsource->v2.z;

    // Create and return the Vector3D with the computed coordinates
    return Vector3D(x, y, z);
}

