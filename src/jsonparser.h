#pragma once
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "../json.hpp"
#include "world.h"

class Camera;
class Pixels;
class Material;
class Texture;

using Materials  = std::map<std::string, std::unique_ptr<Material>>;
using Textures   = std::map<std::string, std::unique_ptr<Texture>>;
using DetailsMap = std::map<std::string, nlohmann::json>;

inline void from_json(const nlohmann::json& j, Vector3D& v) {
    v = Vector3D(j.at(0).get<double>(),
                 j.at(1).get<double>(),
                 j.at(2).get<double>());
}

template<typename T>
T get_json(const nlohmann::json& j, const std::string& key) {
    return j.at(key).get<T>();
}




class JsonParser {
public:
    JsonParser();
    nlohmann::json handle_command(const nlohmann::json& cmd );
    void build();
    Camera get_camera();
    Pixels get_pixels();
    World  get_world();
    std::string filename;
    int ray_depth, ray_samples;
    int num_threads = 1;

    DetailsMap objects_details, materials_details, textures_details;
    nlohmann::json misc_details = nlohmann::json::object();

private:
    void do_add(const nlohmann::json& cmd);
    void do_delete(const nlohmann::json& cmd);
    void do_set(const nlohmann::json& cmd);
    nlohmann::json do_get(const nlohmann::json& cmd) const;

    void build_textures();
    void build_materials();
    void build_objects();
    void build_misc();
    void verify();
    std::unique_ptr<Texture>  make_texture(const nlohmann::json& j) const;
    std::unique_ptr<Material> make_material(const nlohmann::json& j) const;
    World world;

    Materials materials;
    Textures textures;
    Point3D camera_position, camera_target;
    Vector3D camera_up;
    double camera_fov = 90.0;

    int rows = 720;
    int columns = 1280;
    double aspect = static_cast<double>(columns) / rows;

    bool found_camera = false;
    bool found_pixels = false;
    bool found_rays = false;


};