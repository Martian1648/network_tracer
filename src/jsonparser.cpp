#include "jsonparser.h"
#include "color.h"
#include "diffuse.h"
#include "camera.h"
#include "pixels.h"
#include "sphere.h"
#include "glass.h"
#include "metal.h"
#include "lambertian.h"
#include "specular.h"
#include <algorithm>
#include <array>
#include <complex>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "checkerboard.h"
#include "constantmedium.h"
#include "constants.h"
#include "gradient.h"
#include "image.h"
#include "isotropic.h"
#include "marble.h"
#include "normal.h"
#include "rectangle.h"
#include "solid.h"
#include "swirl.h"
#include "../json.hpp"
#include <thread>

JsonParser::JsonParser(){
    // set defaults
    num_threads = std::max(1u, std::thread::hardware_concurrency());
    textures_details["white"] = {
        {"texture_type", "solid"},
        {"color", {1.0, 1.0, 1.0}},
    };
    textures_details["light"] = {
        {"texture_type", "solid"},
        {"color", {4.0, 4.0, 4.0}},
    };
    textures_details["green"] = {
        {"texture_type", "solid"},
        {"color", {0.0, 1.0, 0.0}},
    };
    textures_details["red"] = {
        {"texture_type", "solid"},
        {"color", {1.0, 0.0, 0.0}},
    };
    materials_details["light"] = {
        {"material_type", "lambertian"},
        {"texture_name",  "light"},
        {"emitting",      true},
    };
    materials_details["white"] = {
        {"material_type", "lambertian"},
        {"texture_name",  "white"},
        {"emitting",      false},
    };
    materials_details["green"] = {
        {"material_type", "lambertian"},
        {"texture_name",  "green"},
        {"emitting",      false},
    };
    materials_details["red"] = {
        {"material_type", "lambertian"},
        {"texture_name",  "red"},
        {"emitting",      false},
    };
    objects_details["default_light"] = {
        {"center",        {100.0, 100.0, 1100.0}},
        {"radius",        1000.0},
        {"material_name", "light"},
        {"angle",         {0.0, 0.0, 0.0}},
    };
    misc_details["rays"] = {
        {"ray_depth",   10},
        {"ray_samples", 20},
    };

    misc_details["camera"] = {
        {"position",     {0.0, 14.0, 6.0}},
        {"target_point", {0.0, 0.0, 4.0}},
        {"up",           {0.0, 0.0, 1.0}},
        {"fov",          90.0},
    };

    misc_details["output"] = {
        {"name", "default.png"},
    };
    found_pixels = true;
}

// removes parents so children can be inspected directly
nlohmann::json strip_routing(const nlohmann::json& cmd,
                             std::initializer_list<const char*> keys) {
    nlohmann::json payload = cmd;
    for (const char* k : keys) payload.erase(k);
    return payload;
}

// take the input and perform requested action
nlohmann::json JsonParser::handle_command(const nlohmann::json& cmd) {
    std::string action = get_json<std::string>(cmd, "action");
    if (action == "ADD") {
        do_add(cmd);
    }
    else if (action == "EDIT") {
        do_add(cmd);
    }
    else if (action == "DELETE") {
        do_delete(cmd);
    }
    else if (action == "SET") {
        do_set(cmd);
    }
    else if (action == "GET") {
        return do_get(cmd);
    }
    else if (action == "QUIT") {
        return cmd;
    }
    else{
        throw std::runtime_error("Unknown action");
    }
    return {};
}

void JsonParser::do_add(const nlohmann::json &cmd) {
    // get the first layer of information, then get rid of them
    std::string type = get_json<std::string>(cmd, "type");
    std::string name = get_json<std::string>(cmd, "name");
    auto details = strip_routing(cmd, {"action", "type", "name"});

    // add to state
    if (type == "texture"){
        textures_details[name] = details;
    }
    else if (type == "material") {
        materials_details[name] = details;
    }
    else if (type == "object") {
        objects_details[name] = details;
    }
    else {
        throw std::runtime_error("Unknown type");
    }
}

void JsonParser::do_delete(const nlohmann::json &cmd) {
    std::string type = get_json<std::string>(cmd, "type");
    std::string name = get_json<std::string>(cmd, "name");
    // kinda unnecessary call of strip routing
    auto details = strip_routing(cmd, {"action", "type", "name"});

    if (type == "texture"){
        textures_details.erase(name);
    }
    else if (type == "material") {
        materials_details.erase(name);
    }
    else if (type == "object") {
        objects_details.erase(name);
    }
    else {
        throw std::runtime_error("Unknown type");
    }
}

void JsonParser::do_set(const nlohmann::json &cmd) {
    // get the desired section to SET, and replace the entry with parameters given by client
    std::string target = get_json<std::string>(cmd, "target");
    misc_details[target] = strip_routing(cmd, {"action", "target"});
}


nlohmann::json JsonParser::do_get(const nlohmann::json& cmd) const {
    std::string target = get_json<std::string>(cmd, "target");
    // basic retrieval
    if (misc_details.contains(target)) {
        return misc_details.at(target);
    }

    // get one object, texture, or material
    if (target == "object") {
        std::string name = get_json<std::string>(cmd, "name");
        return objects_details.at(name);
    }
    if (target == "material") {
        std::string name = get_json<std::string>(cmd, "name");
        return materials_details.at(name);
    }
    if (target == "texture") {
        std::string name = get_json<std::string>(cmd, "name");
        return textures_details.at(name);
    }

    // get all objects, materials, or textures
    if (target == "objects") {
        nlohmann::json package = nlohmann::json::array();
        int i = 0;
        for (auto [key, value] : objects_details) {
            value["name"] = key;
            package.push_back(value);
        }
        return package;
    }
    if (target == "materials") {
        nlohmann::json package = nlohmann::json::array();
        int i = 0;
        for (auto [key, value] : materials_details) {
            value["name"] = key;
            package.push_back(value);
        }
        return package;
    }
    if (target == "textures") {
        nlohmann::json package = nlohmann::json::array();
        int i = 0;
        for (auto [key, value] : textures_details) {
            value["name"] = key;
            package.push_back(value);
        }
        return package;
    }
    return {};
}

void JsonParser::build() {
    // actually create and fill the world
    // this is essentially identical to the original parser, except it uses nlohmann and JSON instead of stringstream and raw text
    textures.clear();
    materials.clear();
    world = World{};
    build_misc();
    build_textures();
    build_materials();
    build_objects();
    verify();
}

// seperate build and make functions, since working with unique pointers
void JsonParser::build_textures() {
    for (const auto& [name, j] : textures_details) {
        textures[name] = make_texture(j);
    }
}

std::unique_ptr<Texture> JsonParser::make_texture(const nlohmann::json &j) const {
    std::string type = get_json<std::string>(j, "texture_type");

    if (type == "solid") {
        return std::make_unique<Solid>(
            get_json<Vector3D>(j, "color"));
    }
    if (type == "gradient") {
        return std::make_unique<Gradient>(
            get_json<Vector3D>(j, "color1"),
            get_json<Vector3D>(j, "color2"),
            get_json<bool>    (j, "vertical"));
    }
    if (type == "normal") {
        return std::make_unique<Normal>();
    }
    if (type == "checkerboard") {
        return std::make_unique<Checkerboard>(
            get_json<Vector3D>(j, "color1"),
            get_json<Vector3D>(j, "color2"));
    }
    if (type == "swirl") {
        return std::make_unique<Swirl>(
            get_json<Vector3D>(j, "color1"),
            get_json<Vector3D>(j, "color2"),
            get_json<Vector3D>(j, "color3"),
            get_json<Vector3D>(j, "color4"));
    }
    if (type == "marble") {
        return std::make_unique<Marble>();
    }
    throw std::runtime_error("Unknown texture");

}

void JsonParser::build_materials() {
    for (const auto& [name, j] : materials_details) {
        materials[name] = make_material(j);
    }
}

std::unique_ptr<Material> JsonParser::make_material(const nlohmann::json& j) const {
    std::string type= get_json<std::string>(j, "material_type");
    std::string tex_name= get_json<std::string>(j, "texture_name");
    bool emitting= get_json<bool>(j, "emitting");

    auto it = textures.find(tex_name);
    if (it == textures.end()) {
        throw std::runtime_error("Unknown texture");
    }
    Texture* tex = it->second.get();

    if (type == "metallic") {
        const auto fuzz = get_json<double>(j, "fuzz");
        return std::make_unique<Metal>(tex, emitting, fuzz);
    }
    if (type == "lambertian") {
        return std::make_unique<Lambertian>(tex, emitting);
    }

    throw std::runtime_error("Unknown material");
}
void JsonParser::build_misc() {
    if (misc_details.contains("camera")) {
        const auto& c = misc_details.at("camera");
        camera_position = get_json<Point3D>(c, "position");
        camera_target = get_json<Point3D>(c, "target_point");
        camera_up = get_json<Vector3D>(c, "up");
        camera_fov = get_json<double>(c, "fov");
        found_camera = true;
    }
    if (misc_details.contains("rays")) {
        const auto& r = misc_details.at("rays");
        ray_depth   = get_json<int>(r, "ray_depth");
        ray_samples = get_json<int>(r, "ray_samples");
        found_rays = true;
    }
    if (misc_details.contains("output")) {
        filename = get_json<std::string>(misc_details.at("output"), "name");
    }
}

void JsonParser::build_objects() {
    for (const auto& [name, j] : objects_details) {
        const auto center= get_json<Point3D>(j, "center");
        const auto radius= get_json<double>(j, "radius");
        const auto mat_name= get_json<std::string>(j, "material_name");
        const auto angle = get_json<Vector3D>(j, "angle");

        auto it = materials.find(mat_name);
        if (it == materials.end()) {
            throw std::runtime_error("Unknown object");
        }
        Material* mat = it->second.get();
        world.add(std::make_unique<Sphere>(center, radius, mat, angle));
    }
}

void JsonParser::verify() {
    std::string msg{"Missing from file: "};
    if (!found_camera) {
        throw std::runtime_error(msg + "camera");
    }
    if (!found_pixels) {
        throw std::runtime_error(msg + "pixels");
    }
    if (world.objects.empty()) {
        throw std::runtime_error(msg + "sphere");
    }
    if (filename.empty()) {
        throw std::runtime_error(msg + "output");
    }
    if (materials.empty()) {
        throw std::runtime_error(msg + "material");
    }
    if (!found_rays) {
        throw std::runtime_error(msg + "rays");
    }
}

Camera JsonParser::get_camera() {
    return Camera{camera_position, camera_target, camera_up, camera_fov, aspect};
}

Pixels JsonParser::get_pixels() {
    return Pixels{columns, rows};
}

World JsonParser::get_world() {
    return std::move(world);
}
