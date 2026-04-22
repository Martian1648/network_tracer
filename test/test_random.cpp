/*
ὄνομα: test_random
αἰών: 2/17/2025
βούλημα: raytracer
*/

#include <iostream>

#include "camera.h"
#include "pixels.h"
#include "random.h"
#include "world.h"
#include "material.h"
#include <memory>
#include <cmath>
#include <iomanip>
#include "color.h"
#include "diffuse.h"

Color trace (const World& world, const Ray& ray);
void print_progress(long long ray_num, long long total_rays);
Color trace_path(const World& world, const Ray& ray, int depth);
int main() {
    Diffuse red{Red, false};
    Diffuse light{White, true};
    Diffuse ground{Gray, false};
    World world;
    world.add({0, 0, 1001}, 1000, &light);
    world.add({0, 0, -1011}, 1000, &ground);
    world.add({0, 0, 0}, .3, &red);
    world.add({1, 0, 0}, .3, &red);

    Pixels pixels{1280, 720};

    Vector3D position{10, -2, 3}, up{0, 0, 1};
    Vector3D target{0, 0, 0};
    double fov = 20;
    double aspect = static_cast<double>(pixels.columns)/pixels.rows;
    Camera camera{position, target, up, fov, aspect};

    constexpr int samples = 200;
    int depth = 5;
    //track progress
    const long long rays_total = pixels.rows * pixels.columns * static_cast<long long>(samples);
    long long ray_num = 0;
    for (auto row = 0; row < pixels.rows; ++row) {
        for (auto col = 0; col < pixels.columns; ++col) {
            for (int N = 0; N < samples; ++N) {
                double y = (row + random_double())/(pixels.rows-1);
                double x = (col + random_double())/(pixels.columns-1);
                Ray ray = camera.compute_ray(x, y);
                //pixels(row, col) += trace(world, ray);
                pixels(row, col) += trace_path(world, ray, depth);
                ++ray_num;
                if (ray_num % (rays_total / 100) == 0) {
                    print_progress(ray_num, rays_total);
                }
            }
            pixels(row, col) /= samples;
        }
    }
    pixels.save_png("world.png");
}

Color trace_path (const World& world, const Ray& ray, int depth) {
    if (depth == 0) {
        return Black;
    }
    std::optional<Hit> hit = world.find_nearest(ray);
    if (!hit.has_value()) {
        return Black; // or a background color
    }
    const Material* material = hit->material;
    if (material->emitting) {
        return material->color;
    }
    Ray scattered = material->scatter(ray, hit.value());
    return trace_path(world, scattered, depth-1) * material->color;
}

Color trace(const World& world, const Ray& ray) {
    std::optional<Hit> hit = world.find_nearest(ray);
    if (hit.has_value()) {
        return hit->normal;
    }
    else {
        return Black;
    }
}

void print_progress(long long ray_num, long long total_rays) {
    auto width = std::to_string(total_rays).length() + 4;
    int percentage = std::round(static_cast<double>(ray_num)/total_rays * 100);
    std::cout<<"\rProgram: " << std::setw(3)<< percentage << "%";
    std::cout<<std::setw(width)<< ray_num << "/" << total_rays << " rays";
    std::cout<<std::flush;
}


