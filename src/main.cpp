#include <iostream>
#include <algorithm>
#include <conio.h>

#include "core/film.h"
#include "core/matrix4x4.h"
#include "core/ray.h"
#include "core/utils.h"
#include "core/scene.h"


#include "shapes/sphere.h"
#include "shapes/infiniteplan.h"

#include "cameras/ortographic.h"
#include "cameras/perspective.h"

#include "shaders/intersectionshader.h"
#include "shaders/depthshader.h"
#include "shaders/normalshader.h"
#include "shaders/whittedintegrator.h"
#include "shaders/hdishader.h"
#include "shaders/adishader.h"
#include "shaders/pathtracingshader.h"



#include "materials/phong.h"
#include "materials/emissive.h"
#include "materials/mirror.h"
#include "materials/material.h"
#include "materials/transmissive.h" // added for lab1 final part


#include <chrono>

using namespace std::chrono;

typedef std::chrono::duration<double, std::milli> durationMs;


void buildSceneCornellBox(Camera*& cam, Film*& film,
    Scene myScene)
{
    /* **************************** */
/* Declare and place the camera */
/* **************************** */
    Matrix4x4 cameraToWorld = Matrix4x4::translate(Vector3D(0, 0, -3));
    double fovDegrees = 60;
    double fovRadians = Utils::degreesToRadians(fovDegrees);
    cam = new PerspectiveCamera(cameraToWorld, fovRadians, *film);

    /* ********* */
    /* Materials */
    /* ********* */
    Material* redDiffuse = new Phong(Vector3D(0.7, 0.2, 0.3), Vector3D(0, 0, 0), 100);
    Material* greenDiffuse = new Phong(Vector3D(0.2, 0.7, 0.3), Vector3D(0, 0, 0), 100);
    Material* greyDiffuse = new Phong(Vector3D(0.8, 0.8, 0.8), Vector3D(0, 0, 0), 100);      
    Material* blueGlossy_20 = new Phong(Vector3D(0.2, 0.3, 0.8), Vector3D(0.8, 0.8, 0.8), 20);
    Material* blueGlossy_80 = new Phong(Vector3D(0.2, 0.3, 0.8), Vector3D(0.8, 0.8, 0.8), 80);
    Material* cyandiffuse = new Phong(Vector3D(0.2, 0.8, 0.8), Vector3D(0, 0, 0), 100);

    //Task 5.3
    Material* mirror = new Mirror();
    //Task 5.4
    
    Material* transmissive = new Transmissive(0.7);


    /* ******* */
    /* Objects */
    /* ******* */
    double offset = 3.0;
    Matrix4x4 idTransform;
    // Construct the Cornell Box
    Shape* leftPlan = new InfinitePlan(Vector3D(-offset - 1, 0, 0), Vector3D(1, 0, 0), redDiffuse);
    Shape* rightPlan = new InfinitePlan(Vector3D(offset + 1, 0, 0), Vector3D(-1, 0, 0), greenDiffuse);
    Shape* topPlan = new InfinitePlan(Vector3D(0, offset, 0), Vector3D(0, -1, 0), greyDiffuse);
    Shape* bottomPlan = new InfinitePlan(Vector3D(0, -offset, 0), Vector3D(0, 1, 0), greyDiffuse);
    Shape* backPlan = new InfinitePlan(Vector3D(0, 0, 3 * offset), Vector3D(0, 0, -1), greyDiffuse);

    myScene.AddObject(leftPlan);
    myScene.AddObject(rightPlan);
    myScene.AddObject(topPlan);
    myScene.AddObject(bottomPlan);
    myScene.AddObject(backPlan);


    // Place the Spheres and square inside the Cornell Box
    double radius = 1;         
    Matrix4x4 sphereTransform1;
    sphereTransform1 = Matrix4x4::translate(Vector3D(1.5, -offset + radius, 6));
    Shape* s1 = new Sphere(radius, sphereTransform1, blueGlossy_20); 

    Matrix4x4 sphereTransform2;
    sphereTransform2 = Matrix4x4::translate(Vector3D(-1.5, -offset + 3*radius, 4));
    Shape* s2 = new Sphere(radius, sphereTransform2, blueGlossy_80);

    Shape* square = new Square(Vector3D(offset + 0.999, -offset - 0.2, 3.0), Vector3D(0.0, 4.0, 0.0), Vector3D(0.0, 0.0, 2.0), Vector3D(-1.0, 0.0, 0.0), mirror);

    myScene.AddObject(s1);
    myScene.AddObject(s2);
    myScene.AddObject(square);

    PointLightSource* myPointLight = new PointLightSource(Vector3D(0, 2.5, 3.0), Vector3D(2.0));
    myScene.AddPointLight(myPointLight);

}


void buildSceneCornellBox_lab2(Camera*& cam, Film*& film,
    Scene myScene)
{
    /* **************************** */
/* Declare and place the camera */
/* **************************** */
    Matrix4x4 cameraToWorld = Matrix4x4::translate(Vector3D(0, 0, -3));
    double fovDegrees = 60;
    double fovRadians = Utils::degreesToRadians(fovDegrees);
    cam = new PerspectiveCamera(cameraToWorld, fovRadians, *film);

    /* ********* */
    /* Materials */
    /* ********* */
    Material* redDiffuse = new Phong(Vector3D(0.7, 0.2, 0.3), Vector3D(0, 0, 0), 100);
    Material* greenDiffuse = new Phong(Vector3D(0.2, 0.7, 0.3), Vector3D(0, 0, 0), 100);
    Material* greyDiffuse = new Phong(Vector3D(0.8, 0.8, 0.8), Vector3D(0, 0, 0), 100);
    Material* blueGlossy_20 = new Phong(Vector3D(0.2, 0.3, 0.8), Vector3D(0.2, 0.2, 0.2), 20);
    Material* blueGlossy_80 = new Phong(Vector3D(0.2, 0.3, 0.8), Vector3D(0.2, 0.2, 0.2), 80);
    Material* cyandiffuse = new Phong(Vector3D(0.2, 0.8, 0.8), Vector3D(0, 0, 0), 100);
    Material* emissive = new Emissive(Vector3D(25, 25, 25), Vector3D(0.5));

    Material* mirror = new Mirror();
    Material* transmissive = new Transmissive(0.7);

    /* ******* */
    /* Objects */
    /* ******* */
    double offset = 3.0;
    Matrix4x4 idTransform;
    // Construct the Cornell Box
    Shape* leftPlan = new InfinitePlan(Vector3D(-offset - 1, 0, 0), Vector3D(1, 0, 0), redDiffuse);
    Shape* rightPlan = new InfinitePlan(Vector3D(offset + 1, 0, 0), Vector3D(-1, 0, 0), greenDiffuse);
    Shape* topPlan = new InfinitePlan(Vector3D(0, offset, 0), Vector3D(0, -1, 0), greyDiffuse);
    Shape* bottomPlan = new InfinitePlan(Vector3D(0, -offset, 0), Vector3D(0, 1, 0), greyDiffuse);
    Shape* backPlan = new InfinitePlan(Vector3D(0, 0, 3 * offset), Vector3D(0, 0, -1), greyDiffuse);
    Shape* square_emissive = new Square(Vector3D(-1.0, 3.0, 3.0), Vector3D(2.0, 0.0, 0.0), Vector3D(0.0, 0.0, 2.0), Vector3D(0.0, -1.0, 0.0), emissive);


    myScene.AddObject(leftPlan);
    myScene.AddObject(rightPlan);
    myScene.AddObject(topPlan);
    myScene.AddObject(bottomPlan);
    myScene.AddObject(backPlan);
    myScene.AddObject(square_emissive);


    // Place the Spheres inside the Cornell Box
    double radius = 1;
    Matrix4x4 sphereTransform1;
    sphereTransform1 = Matrix4x4::translate(Vector3D(1.5, -offset + radius, 6));
    Shape* s1 = new Sphere(radius, sphereTransform1, blueGlossy_20);

    Matrix4x4 sphereTransform2;
    sphereTransform2 = Matrix4x4::translate(Vector3D(-1.5, -offset + 3 * radius, 4));
    Shape* s2 = new Sphere(radius, sphereTransform2, transmissive); // transmissi

    Shape* square = new Square(Vector3D(offset + 0.999, -offset - 0.2, 3.0), Vector3D(0.0, 4.0, 0.0), Vector3D(0.0, 0.0, 2.0), Vector3D(-1.0, 0.0, 0.0), mirror);

    myScene.AddObject(s1);
    myScene.AddObject(s2);
    myScene.AddObject(square);
}


void buildSceneSphere(Camera*& cam, Film*& film,
    Scene myScene)
{
    /* **************************** */
      /* Declare and place the camera */
      /* **************************** */
      // By default, this gives an ID transform
      //  which means that the camera is located at (0, 0, 0)
      //  and looking at the "+z" direction
    Matrix4x4 cameraToWorld;
    double fovDegrees = 60;
    double fovRadians = Utils::degreesToRadians(fovDegrees);
    cam = new PerspectiveCamera(cameraToWorld, fovRadians, *film);



    /* ************************** */
    /* DEFINE YOUR MATERIALS HERE */
    /* ************************** */

    Material* green_100 = new Phong(Vector3D(0.2, 0.7, 0.3), Vector3D(0.2, 0.6, 0.2), 50);

    // Define and place a sphere
    Matrix4x4 sphereTransform1;
    sphereTransform1 = sphereTransform1.translate(Vector3D(-1.25, 0.5, 4.0));
    Shape* s1 = new Sphere(1.0, sphereTransform1, green_100);

    // Define and place a sphere
    Matrix4x4 sphereTransform2;
    sphereTransform2 = sphereTransform2.translate(Vector3D(1.25, 0.0, 6));
    Shape* s2 = new Sphere(1.25, sphereTransform2, green_100);

    // Define and place a sphere
    Matrix4x4 sphereTransform3;
    sphereTransform3 = sphereTransform3.translate(Vector3D(1.0, -0.75, 3.5));
    Shape* s3 = new Sphere(0.25, sphereTransform3, green_100);

    // Store the objects in the object list
    myScene.AddObject(s1);
    myScene.AddObject(s2);
    myScene.AddObject(s3);
   
}

void raytrace(Camera* &cam, Shader* &shader, Film* &film,
              std::vector<Shape*>* &objectsList, std::vector<LightSource*>* &lightSourceList)
{
    
    double my_PI = 0.0;
    double n_estimations = 0.0;
    unsigned int sizeBar = 40;

    size_t resX = film->getWidth();
    size_t resY = film->getHeight();

    // Main raytracing loop
    // Out-most loop invariant: we have rendered lin lines
    for(size_t lin=0; lin<resY; lin++)
    {
        // Show progression 
        double progress = (double)lin / double(resY);
        Utils::printProgress(progress);

        // Inner loop invariant: we have rendered col columns
        for(size_t col=0; col<resX; col++)
        {
            // Compute the pixel position in NDC
            double x = (double)(col + 0.5) / resX;
            double y = (double)(lin + 0.5) / resY;
            // Generate the camera ray
            Ray cameraRay = cam->generateRay(x, y);
            Vector3D pixelColor = Vector3D(0.0);
			pixelColor += shader->computeColor(cameraRay, *objectsList, *lightSourceList);

            // Store the pixel color
            film->setPixelValue(col, lin, pixelColor);
        }
    }


}



//------------TASK 1---------------------//
void PaintImage(Film* film)
{
    unsigned int sizeBar = 40;

    size_t resX = film->getWidth();
    size_t resY = film->getHeight();

    // Main Image Loop
    for (size_t lin = 0; lin < resY; lin++)
    {
        // Show progression
        if (lin % (resY / sizeBar) == 0)
            std::cout << ".";

        for (size_t col = 0; col < resX; col++)
        { 
			//CHANGED with the formula given in the task
			// FORMULA: color = RGBColor(x/width, y/height, 0.0)
			Vector3D random_color = Vector3D(1.0 * col / (double)resX, 1.0 * lin / (double) resY, 0.0);
			film->setPixelValue(col, lin, random_color);
        }
    }
}


void DisplayMenu() {
    std::cout << "------------------ Menu ------------------\n";
    std::cout << "Choose a task to execute:\n";
	std::cout << " ------  LAB 1 TASKS -------------------\n";
    std::cout << "1. Task 1: Paint Image\n";
    std::cout << "2. Task 2: Intersection Integrator\n";
    std::cout << "3. Task 3: Depth Integrator\n";
    std::cout << "4. Task 4: Normal Integrator\n";
    std::cout << "5. Task 5: Whitted Integrator - Direct Ilumination & Mirror Material\n";
	std::cout << " ------  LAB 2 TASKS -------------------\n";
	std::cout << "6. Task 1: Hemispherical Direct Illumination Shader\n";
	std::cout << "7. Task 2: Area Direct Illumination Shader\n";
    std::cout << "8. Task 3: Pure Path Tracing\n";
	std::cout << "-------------------------------------------\n";
	std::cout << "ESC. Exit\n";
    std::cout << "-------------------------------------------\n";
    std::cout << "Enter your choice (1-8): ";
}


int main()
{
    while (true) {

        std::string separator     = "\n----------------------------------------------\n";
        std::string separatorStar = "\n**********************************************\n";
        std::cout << separator << "RT-ACG - Ray Tracer for \"Advanced Computer Graphics\"" << separator << std::endl;

        // Create an empty film
        Film *film;
        film = new Film(720, 512);

        // Declare the shader params
        Vector3D bgColor(0.0, 0.0, 0.0); // Background color (for rays which do not intersect anything)
        Vector3D intersectionColor(1.0, 0.0, 0.0);

        // Build the scene---------------------------------------------------------
        // 
        // Declare pointers to all the variables which describe the scene
        Camera* cam;
        Scene myScene;
        Shader* shader = new IntersectionShader(intersectionColor, bgColor);

        int choice; // What the user selects
        
        // Main menu
        DisplayMenu();

        // Get user input
        std::cin >> choice;;  // Use _getch() to capture special keys like 'Esc'

        switch (choice) {
        case 1:
            buildSceneSphere(cam, film, myScene); //Task 2,3,4;
            PaintImage(film);  // Task 1
            choice = 1;
            break;
        case 2:
            buildSceneSphere(cam, film, myScene); //Task 2,3,4
            choice = 2;
            break;
        case 3:
            intersectionColor = Vector3D(0.0, 1.0, 0.0);
            shader = new DepthShader(intersectionColor, 7.5f, bgColor);
            buildSceneSphere(cam, film, myScene); //Task 2,3,4;
            choice = 3;
            break;

        case 4:
            shader = new NormalShader(intersectionColor, bgColor);
            buildSceneSphere(cam, film, myScene); //Task 2,3,4;
            choice = 4;
            break;

        case 5:
            shader = new WhittedIntegrator(bgColor);
            buildSceneCornellBox(cam, film, myScene); //Task 5
			choice = 5;
            break;

        case 6:
            shader = new HDIShader();
            buildSceneCornellBox_lab2(cam, film, myScene); //Task 2 lab2
			choice = 6;
            break;

        case 7:
            shader = new ADIShader();
            buildSceneCornellBox_lab2(cam, film, myScene); //Task 2 lab2
            choice = 7;
            break;

        case 8:
            shader = new PathTracingShader();
            buildSceneCornellBox_lab2(cam, film, myScene); //task 3 lab2
            dynamic_cast<PathTracingShader*>(shader)->raytrace(cam, shader, film, myScene.objectsList, myScene.LightSourceList);
            choice = 8;
            break;

        case 27:  // ASCII value of 'Esc' is 27
            std::cout << "Exiting the program...\n";
            return 0;  // Exit the loop and program
        default:
            std::cout << "Invalid choice. Please enter a number between 1 and 8.\n";
        }


        // Launch some rays! TASK 2,3,...   
        auto start = high_resolution_clock::now();
        
        // Print the number of light sources in our list
        std::cout << "Number of light sources: " << myScene.LightSourceList->size() << std::endl;


        if (choice!=1 && choice!= 8) raytrace(cam, shader, film, myScene.objectsList, myScene.LightSourceList);
        auto stop = high_resolution_clock::now();


        // Save the final result to file
        std::cout << "\n\nSaving the result to file output.bmp\n" << std::endl;
        film->save();
        film->saveEXR();

        float durationS = (durationMs(stop - start) / 1000.0).count();
        std::cout << "FINAL_TIME(s): " << durationS << std::endl;


        std::cout << "\n\n" << std::endl;
    }
}
