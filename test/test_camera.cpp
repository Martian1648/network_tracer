/*
ὄνομα: test_camera
αἰών: 2/10/2025
βούλημα: raytracer
*/
#include <iostream>
#include "camera.h"
#include "pixels.h"
#include "sphere.h"

void test_camera(const Camera& cam, Pixels pixels, const Sphere& sphere,
    const Sphere& ground, std::string filename);

int main() {
    const unsigned columns = 1280;
    const unsigned rows = 720;
    Pixels pixels{columns, rows};
    Sphere sphere{{10, 10, 10}, 5};
    Sphere ground{{0,0,-1001}, 1000};
    //view from equator of sphere? Is that what its called?
    Camera cam_equator{
    {50,50,10}, sphere.center, {0,0,1}, 90,
        16./9.0};
    test_camera(cam_equator, pixels, sphere, ground, "equator_view.png");

    //view from the top of the sphere
    Camera cam_top{
        {10,10,30}, sphere.center, {0,0,1}, 90,
            16./9.0};
    test_camera(cam_top, pixels, sphere, ground, "top_view.png");

    //view from absurdly far away
    Camera cam_far{
        {1000,30,10}, sphere.center, {0,0,1}, 90,
                16./9.0};
    test_camera(cam_far, pixels, sphere, ground, "far_view.png");

    //view with 1/2 fov
    Camera cam_halved{
            {0,0,10}, sphere.center, {0,0,1}, 45,
            16./9.0};
    test_camera(cam_halved, pixels, sphere, ground, "halved_view.png");

    //view with double fov
    Camera cam_double{
                {0,0,10}, sphere.center, {0,0,1}, 180,
                16./9.0};
    test_camera(cam_double, pixels, sphere, ground, "doubled_view.png");
}

void test_camera(const Camera& cam, Pixels pixels, const Sphere& sphere,
    const Sphere& ground, std::string filename) {
    for (int row = 0; row < pixels.rows; ++row) {
        double u = static_cast<double>(row) / (pixels.rows - 1);
        for (int col = 0; col < pixels.columns; ++col) {
            double v = static_cast<double>(col) / (pixels.columns - 1);
            Ray ray = cam.compute_ray(v, u);
            auto check_ground = ground.intersect_geo(ray);
            if (check_ground.has_value()) {
                Hit hit = ground.construct_hit(ray, check_ground.value());
                pixels(row, col) = White * (1-hit.time/50.0);
            }
            std::optional<double> check;
            check = sphere.intersect_geo(ray);
            if (check.has_value()) {
                Hit hit = sphere.construct_hit(ray, check.value());
                pixels(row, col) = hit.normal;
            }
        }
    }
    pixels.save_png(filename);
}