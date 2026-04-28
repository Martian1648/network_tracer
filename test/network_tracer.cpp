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
#include "jsonparser.h"

void print_progress(long long ray_num, long long total_rays);
Color trace(const World& world, const Ray& ray);
Color trace_path(const BVH& bvh, const Ray& ray, int depth);
void render(const BVH& bvh, const Camera& camera, int depth, int samples, Pixels& pixels,
            std::atomic<long long>& rays_done, long long total_rays, bool progress=false);





void send_msg(int& c, const int& size);
void send_msg(int& c, const std::string msg);
void send_mesg(int&c, const nlohmann::json& obj);

std::pair<ssize_t, nlohmann::json> receive(int& c) {
    char buffer[4096] = {};
    auto received = recv(c, &buffer, sizeof(buffer), 0);
    std::string msg(buffer,static_cast<std::size_t>(received));
    return {received, nlohmann::json::parse(msg)};
}

int main() {

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
    send_msg(c, "Connection made");


    JsonParser parser;
    while (true) {
        try {
            auto [received, data] = receive(c);
            if (received <= 0) {
                continue;
            }
            if (data.contains("action") && get_json<std::string>(data, "action") == "QUIT") {
                break;
            }
            auto output = parser.handle_command(data);
            if (data.contains("action") && get_json<std::string>(data, "action") == "GET") {
                send_mesg(c, output);
                continue;
            }
            parser.build();
            World world = parser.get_world();
            BVH bvh{world.objects};

            Pixels pixels = parser.get_pixels();
            Camera camera = parser.get_camera();

            int depth = parser.ray_depth;
            int samples = parser.ray_samples;
            int num_threads = parser.num_threads;
            int thread_count = std::max(1, num_threads);
            std::atomic<long long> rays_done = 0;
            const long long total_rays = static_cast<long long>(pixels.rows) * pixels.columns * samples;
            std::cout<<parser.filename;
            // Create n-1 images for the additional threads we will launch
            std::vector<Pixels> images;
            for (int i = 0; i < thread_count-1; ++i) {
                images.push_back(pixels);
            }

            // launch additional threads
            std::vector<std::thread> threads;
            int base = samples / thread_count;
            int extra = samples % thread_count;
            for (int i = 0; i < thread_count-1; ++i) {
                int thread_samples = base + (i < extra ? 1 : 0);
                std::thread t{render, std::ref(bvh), camera, depth, thread_samples,
                              std::ref(images.at(i)), std::ref(rays_done), total_rays, false};
                threads.push_back(std::move(t));
            }

            // render on main thread
            int main_samples = base + (thread_count - 1 < extra ? 1 : 0);
            render(bvh, camera, depth, main_samples, pixels, rays_done, total_rays, false);


            // wait for other threads to finish
            for (std::thread& t : threads) {
                t.join();
            }

            // collect all image data
            for (const Pixels& p : images) {
                for (std::size_t i = 0; i < p.values.size(); ++i) {
                    pixels.values.at(i) += p.values.at(i);
                }
            }

            // normalize color values by number of threads
            for (Color& c : pixels.values) {
                c /= samples;
            }

            //pixels.save_png(parser.filename);
            auto z = pixels.get_bytes();
            send_msg(c, z.size());
            auto [_, a] = receive(c);
            const char* bytes = reinterpret_cast<const char*>(z.data());
            auto remaining = z.size();
            while (remaining > 0) {
                auto track = send(c, bytes, remaining, 0);
                if (track <=0) {
                    throw std::runtime_error("failure to send img");
                }
                bytes += track;
                remaining -= track;
            }

        }
        catch (std::exception& err) {
            std::cout << err.what() << '\n';
            close(c);
            close(s);

            break;
        }
    }
    close(c);
    close(s);

}



void send_msg(int& c, const int& size) {
    nlohmann::json message = nlohmann::json::object();
    message["type"]="WARNING";
    message["contents"] = size;
    auto package = message.dump();
    send(c, package.c_str(), package.size(), 0);
}
void send_msg(int& c, const std::string msg) {
    nlohmann::json message = nlohmann::json::object();
    message["type"]="MESSAGE";
    message["contents"] = msg;
    auto package = message.dump();
    send(c, package.c_str(), package.size(), 0);
}
void send_mesg(int&c, const nlohmann::json& obj) {
    nlohmann::json message = nlohmann::json::object();
    message["type"] = "ANSWER";
    message["contents"] = obj;
    std::cout<<message<<'\n';
    auto package = message.dump();
    send(c, package.c_str(), package.size(), 0 );
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
