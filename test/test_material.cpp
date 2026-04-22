/*
ὄνομα: test_material
αἰών: 2/28/2025
βούλημα: raytracer
*/

#include <iostream>
#include <chrono>
#include "camera.h"
#include "pixels.h"
#include "world.h"
//#include "material.h"
#include <memory>
#include <cmath>
#include <iomanip>
/**/
#include "parser.h"
#include <thread>
//

Color trace (const World& world, const Ray& ray);
void print_progress(long long ray_num, long long total_rays);
Color trace_path(const World& world, const Ray& ray, int depth);
void render(const World& world, const Camera& camera, int depth, int samples
    , int num_threads, Pixels& pixels, bool progress = false);
int main(int argc, char* argv[]) {
        //const auto start = std::chrono::high_resolution_clock::now();
        if (argc < 2) {
            std::cout <<"Usage: " << argv[0] << " {filename}\n";
            return 0;
        }
        try {
            Parser parser{argv[1]};
            World world = parser.get_world();
            Pixels pixels = parser.get_pixels();
            Camera camera = parser.get_camera();
            int depth = parser.ray_depth;
            int samples = parser.ray_samples;
            int num_threads = parser.num_threads; // TODO: Add to parser

            //Create n-1 images for additional threads
            std::vector<Pixels> images;
            for (int i = 0; i < num_threads-1; ++i) {
                images.push_back(pixels);
            }

            //launch additional threads
            std::vector<std::thread> threads;
            int base = samples/num_threads;
            int extra = samples % num_threads;
            for (int i = 0; i < num_threads - 1; ++i) {
                int thread_samples = base + (1<extra?1:0);
                std::thread t{render, std::ref(world), camera, depth, thread_samples,
                num_threads, std::ref(images[i]), false};
                threads.push_back(std::move(t));
            }

            //render a main thread
            render(world, camera, depth, base, num_threads,
                pixels, true);

            //wait for other threads to finish
            for (std::thread& t : threads) {
                t.join();
            }

            //collect all image data
            for (const Pixels& p : images) {
                for (std::size_t i = 0; i < p.values.size(); ++i) {
                    pixels.values.at(i) += p.values.at(i);
                }
            }

            //normalize
            for (Color& c : pixels.values) {
                c /= num_threads;
            }

            //track progress

            pixels.save_png(parser.filename);
            //const auto end = std::chrono::high_resolution_clock::now();
            //const std::chrono::duration<double> time = end - start;
            //double display_time = time.count();
            std::cout << std::fixed << std::setprecision(6);
            std::cout << '\n' << "Wrote: " << parser.filename << "\n";
            //std::cout << "Time: " << display_time << "\tRays:" << "rays_total" <<"\n";
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }

}

Color trace_path (const World& world, const Ray& ray, int depth) {
    if (depth == 0) {
        return Black;
    }
    std::optional<Hit> hit = world.find_nearest(ray);
    if (!hit.has_value()) {
        return Black; // or a background color
    }
    const Object* object = hit->object;
    const Material* material = object->material;
    std::pair<double, double> uv = object->get_uv(hit.value());
    Color color = material->texture->value(uv.first, uv.second);
    if (material->emitting) {
        return color;
    }
    Ray scattered = material->scatter(ray, hit.value());
    return trace_path(world, scattered, depth-1) * color;
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
    int percentage = std::round(static_cast<double>(ray_num) / total_rays * 100);
    std::cout << "\rProgram: " << std::setw(3) << percentage << "% ";
    std::cout << std::setw(width) << ray_num << "/" << total_rays << " rays";
    std::cout << std::flush;
}

void render(const World& world, const Camera& camera, int depth, int samples
    , int num_threads, Pixels& pixels, bool progress) {
    const long long rays_total = pixels.rows * pixels.columns * static_cast<long long>(samples);
    long long ray_num = 0;

    if (progress) {
        print_progress(ray_num*num_threads, rays_total*num_threads);
    }

    for (auto row = 0; row < pixels.rows; ++row) {
        for (auto col = 0; col < pixels.columns; ++col) {
            Color color{0,0,0};
            for (int N = 0; N < samples; ++N) {
                double y = (row + random_double())/(pixels.rows-1);
                double x = (col + random_double())/(pixels.columns-1);
                Ray ray = camera.compute_ray(x, y);
                //pixels(row, col) += trace(world, ray);
                pixels(row, col) += trace_path(world, ray, depth);
                //color += trace_path(world, ray, depth);

                ++ray_num;
                auto step = std::max(1LL, rays_total / 100);
                if (ray_num % step == 0 && progress) {
                    print_progress(ray_num*num_threads, rays_total*num_threads);
                }
            }
            pixels(row, col) /= samples;
            //pixels(row, col) = color / samples;
        }
    }
}
