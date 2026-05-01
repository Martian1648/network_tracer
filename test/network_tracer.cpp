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

// Pre-declare functions
void print_progress(long long ray_num, long long total_rays);
Color trace(const World& world, const Ray& ray);
Color trace_path(const BVH& bvh, const Ray& ray, int depth);
void render(const BVH& bvh, const Camera& camera, int depth, int samples, Pixels& pixels,
            std::atomic<long long>& rays_done, long long total_rays, bool progress=false);
void send_msg(int& c, const int& size);
void send_msg(int& c, const std::string msg);
void send_mesg(int&c, const nlohmann::json& obj);
std::pair<ssize_t, nlohmann::json> receive(int& c);

// Runs from the command line, takes in address & port number
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " {ipaddr} {port}\n";
        return 0;
    }
    // create socket
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        throw std::runtime_error("Cannot create socket");
    }
    int port = std::stoi(argv[2]);
    //set it to reuse the address
    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    socklen_t len = sizeof(server);
    if (bind(s, reinterpret_cast<struct sockaddr *>(&server), len) < 0) {
        throw std::runtime_error("Cannot bind");
    }

    //listens for only one client
    if (listen(s, 1) < 0) {
        throw std::runtime_error("Deaf");
    }

    // accept connection
    int c = accept(s, reinterpret_cast< struct sockaddr *>(&server), &len);
    if (c < 0) {
        throw std::runtime_error("No acceptance");
    }

    // send confirmation message to client
    send_msg(c, "Connection made");

    // create parser
    JsonParser parser;

    // start the primary run loop
    while (true) {
        try {
            // get JSON structured input from client and unpacks
            auto [received, data] = receive(c);
            if (received <= 0) {
                continue;
            }

            // handle quits
            if (data.contains("action") && get_json<std::string>(data, "action") == "QUIT") {
                break;
            }

            //apply commands and stores any output, which would occur if it was a GET
            auto output = parser.handle_command(data);

            // send requested data if GET and skip generation
            if (data.contains("action") && get_json<std::string>(data, "action") == "GET") {
                send_mesg(c, output);
                continue;
            }

            // build the "world" from the state
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

            //get the pixels before encoding as a PNG
            auto z = pixels.get_bytes();

            // send a warning to client of how large the image is
            send_msg(c, z.size());

            // get acknowledgement
            auto [_, a] = receive(c);

            // change to raw (ish) bytes
            const char* bytes = reinterpret_cast<const char*>(z.data());

            // send in chunks
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
            // close connections
            std::cout << err.what() << '\n';
            send_msg(c, "Something went wrong");
            close(c);
            close(s);

            break;
        }
    }
    // close connections again, just in case.
    close(c);
    close(s);

}

// receive and unpack message from client.
std::pair<ssize_t, nlohmann::json> receive(int& c) {
    char buffer[4096] = {};
    auto received = recv(c, &buffer, sizeof(buffer), 0);
    std::string msg(buffer,static_cast<std::size_t>(received));
    return {received, nlohmann::json::parse(msg)};
}

// send a WARNING message
void send_msg(int& c, const int& size) {
    nlohmann::json message = nlohmann::json::object();
    message["type"]="WARNING";
    message["contents"] = size;
    auto package = message.dump();
    send(c, package.c_str(), package.size(), 0);
}

// Send a basic message
void send_msg(int& c, const std::string msg) {
    nlohmann::json message = nlohmann::json::object();
    message["type"]="MESSAGE";
    message["contents"] = msg;
    auto package = message.dump();
    send(c, package.c_str(), package.size(), 0);
}

// Send an ANSWER to a GET
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
