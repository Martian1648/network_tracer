/*
ὄνομα: world
αἰών: 2/10/2025
βούλημα: raytracer
*/

#pragma once
#include <vector>
#include <memory>
#include "core/hit.h"
#include <optional>
#include "sphere.h"
#include "random.h"
#include "color.h"
#include "diffuse.h"
#include "glass.h"
#include "lambertian.h"
#include "metal.h"
#include "specular.h"


class Sphere;
class Ray;

class World {
public:
    void add(std::unique_ptr<Object>&& object);
    std::optional<Hit> find_nearest(const Ray& ray) const; //O(N)

    std::vector<std::unique_ptr<Object>> objects;

};
