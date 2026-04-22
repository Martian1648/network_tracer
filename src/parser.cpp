#include "parser.h"
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
#include "triangle.h"

Parser::Parser(const std::string& filename)
    :filename{filename}, found_camera{false}, found_pixels{false}, found_output{false}, found_rays{false} {
    std::ifstream input{filename};
    if (!input) {
        std::string msg{"Could not open this filename: " + filename};
        throw std::runtime_error(msg);
    }
    parse(input);
    verify();
}


Camera Parser::get_camera() {
    return Camera{camera_position, camera_target, camera_up, camera_fov, aspect};
}

Pixels Parser::get_pixels() {
    return Pixels{columns, rows};
}

World Parser::get_world() {
    return std::move(world);
}


void remove_comment(std::string& line) {
    auto i = line.find('#');
    if (i != std::string::npos) { // found a #
        line = line.substr(0, i);
    }
}

bool is_whitespace(const std::string& line) {
    return std::all_of(std::begin(line), std::end(line), ::isspace);
}

void Parser::parse(std::ifstream& input) {
    for (std::string line; std::getline(input, line);) {
        remove_comment(line); // remove all text after #

        if (is_whitespace(line)) {
            continue;
        }

        try {
            std::stringstream ss{line};
            std::string type;
            ss >> type; // gets the first word
            if (type == "material") {
                parse_material(ss);
            }
            else if (type == "camera") {
                parse_camera(ss);
            }
            else if (type == "sphere") {
                parse_sphere(ss);
            }
            else if (type == "triangle") {
                parse_triangle(ss);
            }
            else if (type == "constant_medium") {
                parse_constant_medium(ss);
            }
            else if (type == "rectangle") {
                parse_rectangle(ss);
            }
            else if (type == "mesh") {
                parse_mesh(ss);
            }
            else if (type == "texture") {
                parse_texture(ss);
            }
            else if (type == "threads") {
                parse_threads(ss);
            }
            else if (type == "output") {
                parse_output(ss);
            }
            else if (type == "pixels") {
                parse_pixels(ss);
            }
            else if (type == "rays") {
                parse_rays(ss);
            }
            else {
                throw std::runtime_error("Uknown type: " + type + " in line " + line);
            }

        }
        catch (std::runtime_error& e) {
            std::cout << "Error when parsing line:\n" << line << '\n';
            throw e;
        }

    }
}

void Parser::verify() {
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
    if (!found_output) {
        throw std::runtime_error(msg + "output");
    }
    if (materials.empty()) {
        throw std::runtime_error(msg + "material");
    }
    if (!found_rays) {
        throw std::runtime_error(msg + "rays");
    }
}

void Parser::parse_texture(std::stringstream &ss) {
    std::string kind, name;
    ss >> name>> kind;
    if (kind == "solid") {
        Color color;
        ss >> color;
        textures[name] = std::make_unique<Solid>(color);
    }
    else if (kind == "gradient") {
        Color color_a, color_b;
        bool vertical;
        ss >> color_a>>color_b>>std::boolalpha>>vertical;
        textures[name] = std::make_unique<Gradient>(color_a, color_b, vertical);
    }
    else if (kind == "normal") {
        textures[name] = std::make_unique<Normal>();
    }
    else if (kind == "image") {
        std::string filename;
        ss >> filename;
        std::filesystem::path scene_dir = std::filesystem::path(filename).parent_path();
        std::string input = scene_dir / filename;
        textures[name] = std::make_unique<Image>(filename);
    }
    else if (kind == "checkerboard") {
        Color primary, secondary;
        ss >> primary >> secondary;
        textures[name] = std::make_unique<Checkerboard>(primary, secondary);
    }
    else if (kind == "swirl") {
        Color primary, secondary, tertiary, base;
        ss >> primary >> secondary >> tertiary >> base;
        textures[name] = std::make_unique<Swirl>(primary, secondary, tertiary, base);
    }
    else if (kind == "marble") {

        textures[name] = std::make_unique<Marble>();
    }
    else {
        throw std::runtime_error("Unknown texture: " + kind);
    }
}Vector3D rotate (double dx, double dy, double dz, Vector3D angle, Vector3D pivot);
void Parser::parse_mesh(std::stringstream &ss) {
    //mesh position filename material_name
    Vector3D position, angle;
    std::string filename, material_name, type;
    if (!(ss>>type>>position>>filename>>material_name>>angle)) {
        throw std::runtime_error("Error when parsing file: " + filename);
    }
    angle *= (Constants::pi / 180);
    const Material* material = get_material(material_name);
    std::filesystem::path scene_dir = std::filesystem::path(filename).parent_path();
    std::string file_name = scene_dir/ filename;
    std::ifstream input{filename};
    if (!input) {
        throw std::runtime_error("Cant find: " + filename);
    }
    std::string temp;
    input >> temp;
    if (temp != "vertices") {
        throw std::runtime_error("no vertices: " + filename);
    }
    std::vector<Vector3D> vertices;
    /**/
    Point3D size;
    input >> size;
    if (type == "pyramid") {
        double dx = 0.5 * size.x;
        double dy = 0.5 * size.y;
        std::array<std::pair<int, int>, 4> base_offsets = {{
            {-1, -1},
            { 1, -1},
            { 1,  1},
            {-1,  1}
        }};

        for (const auto& [sx, sy] : base_offsets) {
            double local_x = sx * dx;
            double local_y = sy * dy;
            double local_z = 0.0;
            vertices.emplace_back(rotate(local_x, local_y, local_z, angle, position));
        }
        vertices.emplace_back(rotate(0.0, 0.0, size.z, angle, position));
    }
    else if (type == "box") {
        Vector3D pivot = position;

        double sin_theta;
        double cos_theta;
        auto rotate_z= [&](double dx, double dy, double dz) ->Vector3D {
            sin_theta = sin(angle.z);
            cos_theta = cos(angle.z);
            double x_rot = cos_theta * dx + sin_theta * dy;
            double y_rot = -sin_theta * dx + cos_theta * dy;
            return Vector3D{pivot.x + x_rot, pivot.y + y_rot, pivot.z+dz};
        };
        auto rotate_y= [&](double dx, double dy, double dz) ->Vector3D {
            sin_theta = sin(angle.y);
            cos_theta = cos(angle.y);
            double x_rot = cos_theta * dx - sin_theta * dz;
            double z_rot = sin_theta * dx + cos_theta * dz;
            return Vector3D{pivot.x + x_rot, pivot.y + dy, pivot.z+z_rot};
        };
        auto rotate_x= [&](double dx, double dy, double dz) ->Vector3D {
            sin_theta = sin(angle.x);
            cos_theta = cos(angle.x);
            double y_rot = cos_theta * dy - sin_theta * dz;
            double z_rot = sin_theta * dy + cos_theta * dz;
            return Vector3D{pivot.x + dx, pivot.y + y_rot, pivot.z+z_rot};
        };

        vertices.emplace_back(rotate(0, 0,0, angle, pivot));
        vertices.emplace_back(rotate(size.x, 0,      0, angle, pivot));
        vertices.emplace_back(rotate(0,      size.y, 0, angle, pivot));
        vertices.emplace_back(rotate(0,      0,      size.z, angle, pivot));
        vertices.emplace_back(rotate(size.x, size.y, 0, angle, pivot));
        vertices.emplace_back(rotate(size.x, 0,      size.z, angle, pivot));
        vertices.emplace_back(rotate(0,      size.y, size.z, angle, pivot));
        vertices.emplace_back(rotate(size.x, size.y, size.z, angle, pivot));

    }
    else if (type == "square") {
        vertices.emplace_back(position.x, position.y, position.z);
        if (size.x == 0) {
            vertices.emplace_back(position.x, position.y + size.y, position.z);
            vertices.emplace_back(position.x, position.y, position.z + size.z);
            vertices.emplace_back(position.x, position.y + size.y, position.z + size.z);
        }
        else if (size.y == 0) {
            vertices.emplace_back(position.x  + size.x, position.y, position.z);
            vertices.emplace_back(position.x, position.y, position.z + size.z);
            vertices.emplace_back(position.x + size.x, position.y, position.z + size.z);
        }
        else if (size.z == 0) {
            vertices.emplace_back(position.x + size.x, position.y, position.z);
            vertices.emplace_back(position.x, position.y + size.y, position.z);
            vertices.emplace_back(position.x + size.x, position.y + size.y, position.z);
        }
        else {
            throw std::runtime_error("Bad square");
        }

    }
    else if (type == "tetrahedron") {
        constexpr double delta = std::sqrt(3.0) / 2.0;

        // Local vertex offsets (relative to position as pivot)
        Vector3D v0{(std::sqrt(3.0) - 0.5) * size.x, 0.0, 0.0};
        Vector3D v1{-0.5 * size.x, -delta * size.y, 0.0};
        Vector3D v2{-0.5 * size.x,  delta * size.y, 0.0};
        Vector3D v3{0.0, 0.0, size.z};

        // Apply rotation to each vertex and store
        vertices.emplace_back(rotate(v0.x, v0.y, v0.z, angle, position));
        vertices.emplace_back(rotate(v1.x, v1.y, v1.z, angle, position));
        vertices.emplace_back(rotate(v2.x, v2.y, v2.z, angle, position));
        vertices.emplace_back(rotate(v3.x, v3.y, v3.z, angle, position));

    }
    else {
        for (Vector3D vertex; input >> vertex;) {
            vertices.push_back(vertex + position);
        }
        if (vertices.size() < 3) {
            throw std::runtime_error("no triangles: " + filename);
        }
    }

    //input attempted to read triangles into a vector3d type
    input.clear(); //clears error bit
    input >> temp;// triangles
    if (temp != "triangles") {
        throw std::runtime_error("Issues with: " + temp);
    }
    if (type == "pyramid") {
        for (int i = 0; i < 4; ++i) {
            int next = (i + 1)%4;
            std::unique_ptr<Object> triangle = std::make_unique<Triangle>(
                vertices.at(i), vertices.at(next), vertices.at(4), material);
            world.add(std::move(triangle));
        }
        for (int a,b,c; input >> a >> b >> c;) {
            std::unique_ptr<Object> triangle = std::make_unique<Triangle>(
                vertices.at(a), vertices.at(b), vertices.at(c), material);
            world.add(std::move(triangle));
        }
    }
    else if (type == "box") {
        std::vector<int> indices = {
            0 ,1 ,2 ,1 ,4 ,2 ,3 ,6 ,5 ,5 ,6 ,7 ,0 ,2 ,3 ,2 ,6 ,3 ,1 ,5 ,4 ,4 ,5 ,7
            ,0 ,3 ,1 ,1 ,3 ,5 ,2 ,4 ,6 ,4 ,7 ,6
        };
        for (int i = 0; i < indices.size(); i += 3) {
            int a = indices.at(i);
            int b = indices.at(i + 1);
            int c = indices.at(i + 2);
            std::unique_ptr<Object> triangle = std::make_unique<Triangle>(
                vertices.at(a), vertices.at(b), vertices.at(c ), material);
            world.add(std::move(triangle));
        }
    }
    else if (type == "square") {
        std::vector<int> indices = {0,1,2,3,1,2};
        for (int i = 0; i < indices.size(); i += 3) {
            int a = indices.at(i);
            int b = indices.at(i + 1);
            int c = indices.at(i + 2);
            std::unique_ptr<Object> triangle = std::make_unique<Triangle>(
                vertices.at(a), vertices.at(b), vertices.at(c), material);
            world.add(std::move(triangle));
        }
    }
    else if (type == "tetrahedron") {
        std::vector<int> indices = {0,1,2,0,1,3,1,2,3,2,0,3};
        for (int i = 0; i < indices.size(); i += 3) {
            int a = indices.at(i);
            int b = indices.at(i + 1);
            int c = indices.at(i + 2);
            std::unique_ptr<Object> triangle = std::make_unique<Triangle>(
                vertices.at(a), vertices.at(b), vertices.at(c), material);
            world.add(std::move(triangle));
        }
    }
    else {
        for (int a,b,c; input >> a >> b >> c;) {
            std::unique_ptr<Object> triangle = std::make_unique<Triangle>(
                vertices.at(a), vertices.at(b), vertices.at(c), material);
            world.add(std::move(triangle));
        }
    }
    //read each line under triangles

}


Texture *Parser::get_texture(const std::string &texture) {
    auto i = textures.find(texture);
    if (i != textures.end()) {
        return i->second.get();
    }
    else {
        throw std::runtime_error("Unknown texture: " + texture);
    }
}



void Parser::parse_material(std::stringstream& ss) {
    std::string name, kind, texture_name;
    bool emitting;
    ss >> name >> kind >> texture_name >> std::boolalpha >> emitting;
    if (kind == "diffuse") {
        materials[name] = std::make_unique<Diffuse>(get_texture(texture_name), emitting);
    }
    else if (kind == "lambertian") {
        materials[name] = std::make_unique<Lambertian>(get_texture(texture_name), emitting);
    }
    else if (kind == "specular") {
        materials[name] = std::make_unique<Specular>(get_texture(texture_name), emitting);
    }
    else if (kind == "metallic") {
        double fuzz;
        if (ss >> fuzz) {
            materials[name] = std::make_unique<Metal>(get_texture(texture_name), emitting, fuzz);
        }
        else {
            throw std::runtime_error("Missing fuzz parameter for metal");
        }
    }
    else if (kind == "glass") {
        materials[name] = std::make_unique<Glass>(get_texture(texture_name), emitting);
    }
    else if (kind == "isotropic") {
        materials[name] = std::make_unique<Isotropic>(get_texture(texture_name), emitting);
    }
    else {
        throw std::runtime_error("Unknown material: " + kind);
    }
}

Material* Parser::get_material(const std::string& material) {
    auto i = materials.find(material);
    if (i != materials.end()) { // found it!
        return i->second.get();
    }
    else {
        throw std::runtime_error("Unknown material: " + material);
    }
}

void Parser::parse_sphere(std::stringstream& ss) {
    Vector3D center;
    double radius;
    std::string material_name;
    Vector3D angle;
    if (ss >> center >> radius >> material_name) {
        ss >> angle;
        const Material* material = get_material(material_name);
        std::unique_ptr<Object> object = std::make_unique<Sphere>(center, radius, material, angle );
        world.add(std::move(object));
    }
    else {
        throw std::runtime_error("Malformed sphere");
    }
}

void Parser::parse_triangle(std::stringstream &ss) {
    Point3D vertex0, vertex1, vertex2;
    std::string material_name;
    if (ss >> vertex0 >> vertex1 >> vertex2 >> material_name) {
        const Material* material = get_material(material_name);
        std::unique_ptr<Object> object = std::make_unique<Triangle>(vertex0, vertex1, vertex2, material);
        world.add(std::move(object));
    }
    else {
        throw std::runtime_error("Malformed triangle");
    }
}

void Parser::parse_rectangle(std::stringstream &ss) {
    Point3D vertex0, vertex1, vertex2, vertex3;
    std::string material_name;
    if (ss >> vertex0 >> vertex1 >> vertex2>> vertex3 >> material_name) {
        const Material* material = get_material(material_name);
        std::unique_ptr<Object> object = std::make_unique<Rectangle>(vertex0, vertex1,
            vertex2, vertex3, material);
        world.add(std::move(object));
    }
    else {
        throw std::runtime_error("Malformed rectangle");
    }
}

void Parser::parse_constant_medium(std::stringstream &ss) {
    Vector3D center;
    double radius, density;
    std::string material_name;
    if (ss >> center >> radius>> density >> material_name) {
        const Material *material = get_material(material_name);
        auto boundary = new Sphere(center, radius, material);
        std::unique_ptr<Object> object = std::make_unique<Constant_Medium>(boundary, density, material);
        world.add(std::move(object));

    }
    else {
        throw std::runtime_error("Malformed constant_medium");
    }
}


void Parser::parse_camera(std::stringstream& ss) {
    if (ss >> camera_position >> camera_target >> camera_up >> camera_fov) {
        found_camera = true;
    }
    else {
        throw std::runtime_error("Malformed camera");
    }

}
void Parser::parse_rays(std::stringstream& ss) {
    if (ss >> ray_depth >> ray_samples) {
        found_rays = true;
    }

}
void Parser::parse_pixels(std::stringstream& ss) {
    if (ss >> columns >> rows) {
        aspect = static_cast<double>(columns) / rows;
        found_pixels = true;
    }

}
void Parser::parse_output(std::stringstream& ss) {
    if (ss >> filename) {
        found_output = true;
    }
}

void Parser::parse_threads(std::stringstream &ss) {
    if (!(ss >> num_threads)) {
        throw std::runtime_error("Malformed threads");
    }
}


Vector3D rotate (double dx, double dy, double dz, Vector3D angle, Vector3D pivot)  {
    double pitch= angle.x;
    double roll= angle.y;
    double yaw= angle.z;
    double cos_x = cos(pitch);
    double sin_x = sin(pitch);
    double cos_y = cos(roll);
    double sin_y = sin(roll);
    double cos_z = cos(yaw);
    double sin_z = sin(yaw);


    double matrix_00 = cos_y * cos_z;
    double matrix_01 = cos_z * sin_x * sin_y - cos_x * sin_z;
    double matrix_02 = sin_x * sin_z + cos_x * cos_z * sin_y;
    double matrix_10 = cos_y * sin_z;
    double matrix_11 = cos_x * cos_z + sin_x * sin_y * sin_z;
    double matrix_12 = cos_x * sin_y * sin_z - cos_z * sin_x;
    double matrix_20 = -sin_y;
    double matrix_21 = cos_y * sin_x;
    double matrix_22 = cos_x * cos_y;
    double x_rot = matrix_00 * dx + matrix_01 * dy + matrix_02 * dz;
    double y_rot = matrix_10 * dx + matrix_11 * dy + matrix_12 * dz;
    double z_rot = matrix_20 * dx + matrix_21 * dy + matrix_22 * dz;

    return Vector3D{pivot.x + x_rot, pivot.y + y_rot, pivot.z + z_rot};
};