#include "sphere.h"
#include "constantmedium.h"
#include "isotropic.h"
#include "solid.h"
#include "ray.h"


int main() {
    Solid texture{{1, 1, 1}};
    Isotropic isotropic{&texture, false};
    Point3D center{0, 10, 0}; // test outside hit
    //Point3D center{0, 0, 0}; // test inside hit
    Sphere* sphere = new Sphere{center, 1};

    double density = 1;
    Constant_Medium fog{sphere, density, &isotropic};

    Ray ray{{0, 0, 0}, {0, 1, 0}};
    //auto time = sphere->intersect(ray);
    auto time = fog.intersect(ray);
    if (time) {
        std::cout << *time << '\n';
    }
}

