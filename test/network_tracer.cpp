#include "camera.h"
#include "sphere.h"
#include "color.h"
#include "pixels.h"
#include "ray.h"
#include "hit.h"
#include "material.h"
#include "texture.h"
#include "world.h"
#include "random.h"
#include "parser.h"
#include "bvh.h"
#include <atomic>
#include <iomanip>
#include <cmath>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void print_progress(long long ray_num, long long total_rays);
Color trace(const World& world, const Ray& ray);
Color trace_path(const BVH& bvh, const Ray& ray, int depth);
void render(const BVH& bvh, const Camera& camera, int depth, int samples, Pixels& pixels,
            std::atomic<long long>& rays_done, long long total_rays, bool progress=false);

int main(int argc, char* argv[]) {
        const auto start = std::chrono::high_resolution_clock::now();
        if (argc < 2) {
            std::cout << "Usage: " << argv[0] << " {filename}\n";
            return 0;
        }
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        throw std::runtime_error("Cannot create socket");
    }

    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(5005);
    socklen_t len = sizeof(server);

    if (bind(s, reinterpret_cast<struct sockaddr *>(&server), len) < 0) {
        throw std::runtime_error("Cannot bind");
    }

    if (listen(s, 3) < 0) {
        throw std::runtime_error("Deaf");
    }


    int c = accept(s, reinterpret_cast< struct sockaddr *>(&server), &len);
    if (c < 0) {
        throw std::runtime_error("No acceptance");
    }


    char buffer[4096];
    while (true) {
        try {
            auto data = recv(c, &buffer, 4096, 0);
            std::cout<<buffer<<"\n";

        }
        catch (std::exception& err) {
            std::cout << err.what() << '\n';
            close(s);
            close(c);
            break;
        }
    }

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

Color trace_path(const BVH& bvh, const Ray& ray, int depth) {
    if (depth == 0) {
        return Black;
    }

    std::optional<Hit> hit = bvh.find_nearest(ray);
    if (!hit) {
        return Black;
    }

    const Object* object = hit->object;
    auto [u, v] = object->get_uv(*hit);
    const Material* material = object->material;
    Color color = material->texture->value(u, v);
    if (material->emitting) {
        return color;
    }

    // more bounces!
    Ray scattered = material->scatter(ray, hit.value());
    return color * trace_path(bvh, scattered, depth-1);
}


void print_progress(long long ray_num, long long total_rays) {
    auto width = std::to_string(total_rays).length() + 4;
    int percentage = std::round(static_cast<double>(ray_num) / total_rays * 100);
    std::cout << "\rProgram: " << std::setw(3) << percentage << "% ";
    std::cout << std::setw(width) << ray_num << "/" << total_rays << " rays";
    std::cout << std::flush;
}

void render(const BVH& bvh, const Camera& camera, int depth, int samples, Pixels& pixels,
            std::atomic<long long>& rays_done, long long total_rays, bool progress) {

    if (progress) {
        print_progress(0, total_rays);
    }

    const long long step = std::max(1LL, total_rays / 100);
    for (auto row = 0; row < pixels.rows; ++row) {
	for (auto col = 0; col < pixels.columns; ++col) {
            Color color{0, 0, 0};
            for (int N = 0; N < samples; ++N) {
                double y = (row + random_double()) / (pixels.rows - 1);
                double x = (col + random_double()) / (pixels.columns - 1);
                // cast samples number of rays
                Ray ray = camera.compute_ray(x, y);
                color += trace_path(bvh, ray, depth);

                long long completed = rays_done.fetch_add(1) + 1;
               if (progress && (completed % step == 0 || completed == total_rays)) {
                    print_progress(completed, total_rays);
                }
            }
            pixels(row, col) = color;
	}
    }
}
