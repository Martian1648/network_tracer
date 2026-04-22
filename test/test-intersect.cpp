#include "ray.h"
#include <iostream>
#include <optional>
#include <chrono>
#include "constants.h"
#include "sphere.h"
#include "pixels.h"
int main() {
    //Testing to make it not hit, hit once, and hit twice, by adjusting by epsilon
    Sphere sphere{{0,3,0}, 2};
    Ray ray_no{{0,0,2+Constants::epsilon}, {0,1,0}};
    Ray ray_one{{0,0,2}, {0,1,0}};
    Ray ray_two{{0,0,2-Constants::epsilon}, {0,1,0}};
    auto result_alg = sphere.intersect_alg(ray_no);
    if (result_alg.has_value()) {
        std::cout << result_alg.value() << '\n';
    }
    else {
        std::cout << "No intersection found!\n";
    }
    result_alg = sphere.intersect_alg(ray_one);
    if (result_alg.has_value()) {
        std::cout << result_alg.value() << '\n';
    }
    else {
        std::cout << "No intersection found!\n";
    }
    result_alg = sphere.intersect_alg(ray_two);
    if (result_alg.has_value()) {
        std::cout << result_alg.value() << '\n';
    }
    else {
        std::cout << "No intersection found!\n";
    }
    auto result_geo = sphere.intersect_geo(ray_no);
    if (result_geo.has_value()) {
        std::cout << result_geo.value() << '\n';
    }
    else {
        std::cout << "No intersection found!\n";
    }

    result_geo = sphere.intersect_geo(ray_one);
    if (result_geo.has_value()) {
        std::cout << result_geo.value() << '\n';
    }
    else {
        std::cout << "No intersection found!\n";
    }

    result_geo = sphere.intersect_geo(ray_two);
    if (result_geo.has_value()) {
        std::cout << result_geo.value() << '\n';
    }
    else {
        std::cout << "No intersection found!\n";
    }
    //time trials
    auto start = std::chrono::high_resolution_clock::now();
    result_alg = sphere.intersect_alg(ray_no);
    auto end = std::chrono::high_resolution_clock::now();
    if (result_alg.has_value()) {
        std::cout << end - start << "\n";
    } else {
        std::cout << "No intersection found!\n";
    }

    start = std::chrono::high_resolution_clock::now();
    result_alg = sphere.intersect_alg(ray_one);
    end = std::chrono::high_resolution_clock::now();
    if (result_alg.has_value()) {
        std::cout << end - start << "\n";
    } else {
        std::cout << "No intersection found!\n";
    }

    start = std::chrono::high_resolution_clock::now();
    result_alg = sphere.intersect_alg(ray_two);
    end = std::chrono::high_resolution_clock::now();
    if (result_alg.has_value()) {
        std::cout << end - start << "\n";
    } else {
        std::cout << "No intersection found!\n";
    }

    start = std::chrono::high_resolution_clock::now();
    result_geo = sphere.intersect_geo(ray_no);
    end = std::chrono::high_resolution_clock::now();
    if (result_geo.has_value()) {
        std::cout << end - start << "\n";
    } else {
        std::cout << "No intersection found!\n";
    }

    start = std::chrono::high_resolution_clock::now();
    result_geo = sphere.intersect_geo(ray_one);
    end = std::chrono::high_resolution_clock::now();
    if (result_geo.has_value()) {
        std::cout << end - start << "\n";
    } else {
        std::cout << "No intersection found!\n";
    }

    start = std::chrono::high_resolution_clock::now();
    result_geo = sphere.intersect_geo(ray_two);
    end = std::chrono::high_resolution_clock::now();
    if (result_geo.has_value()) {
        std::cout << end - start << "\n";
    } else {
        std::cout << "No intersection found!\n";
    }

    //make picture
    const unsigned columns = 1280;
    const unsigned rows = 720;
    Pixels pixels{columns, rows};
    Sphere circle{{columns/2,251,rows/2}, 250};
    Ray ray{{0,0,0},
                {0,1,0}};
    for (int row = 0; row < pixels.rows; ++row) {
        for (int col = 0; col < pixels.columns; ++col) {
            ray = {{static_cast<double>(col),0,static_cast<double>(row)},
                {0,1,0}};
            auto check = circle.intersect_geo(ray);
            if (check.has_value()) {
                Hit o = circle.construct_hit(ray ,check.value());
                Color surface_norm{
                    .75*(o.normal.y+1),
                    .75*(o.normal.z+1),
                    .75*(o.normal.x+1)
                };
                pixels(row, col) = surface_norm;
            }
        }
    }
    pixels.save_png("output.png");
    std::cout<<"Saved";

}