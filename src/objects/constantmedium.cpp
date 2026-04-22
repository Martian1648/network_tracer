/*
ὄνομα: constant_medium
αἰών: 4/14/2025
βούλημα: raytracer
*/

#include "constantmedium.h"

#include "constants.h"
#include "hit.h"
#include "random.h"
#include "ray.h"
#include "aabb.h"

Constant_Medium::Constant_Medium(Object *boundary, double density, const Material* material):
Object(material), boundary{boundary}, density{density}{

}

Constant_Medium::~Constant_Medium() {
    if (boundary) {
        delete boundary;
    }
}


std::optional<double> Constant_Medium::intersect(const Ray& ray) const {
    auto t1 = boundary->intersect(ray);
    if (!t1) {
        return {};
    }

    Vector3D little_further = ray.at(*t1 + 2*Constants::epsilon);
    auto t2 = boundary->intersect(Ray{little_further, ray.direction});
    //case 1:
    //t1 and t2
    //t2 += t1

    //case 2
    //t1 and not t2
    // t2 = t1
    //t1 = 0

    if (t2) {
        *t2 += *t1;
    }
    else {
        t2 = t1;
        t1 = 0;
    }

    double distance_inside_boundary = t2.value() - t1.value();

    double hit_distance = -1/density * std::log(1-random_double());

    if (hit_distance > distance_inside_boundary) {
        return {};
    }

    double time = t1.value() + hit_distance;
    return time;
}

AABB Constant_Medium::bounding_box() const {
    return boundary->bounding_box();
}

std::pair<double, double> Constant_Medium::get_uv(const Hit& hit) const {
    return boundary->get_uv(hit);
}

Hit Constant_Medium::construct_hit(const Ray &ray, double time) const {
    Point3D position = ray.at(time);
    return Hit{time, position, {1,0,0}, this};
}