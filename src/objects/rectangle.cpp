/*
ὄνομα: rectangle
αἰών: 4/10/2025
βούλημα: raytracer
*/

#include "rectangle.h"

#include "aabb.h"
#include "hit.h"


Rectangle::Rectangle(const Point3D& vertice0, const Point3D& vertice1, const Point3D& vertice2,
                     const Point3D& vertice3, const Material *material)
        :Object(material), vertice0{vertice0}, vertice1{vertice1}, vertice2{vertice2},
vertice3{vertice3}{
    triangle0 = new Triangle{vertice0, vertice1, vertice2, material};
    triangle1 = new Triangle{vertice0, vertice3, vertice2, material};
}

std::optional<double> Rectangle::intersect(const Ray& ray) const {
   std::optional<double> t = triangle0->intersect(ray);
    if (t.has_value()) {
        return t;
    }
    t = triangle1->intersect(ray);
    return t;
}

Hit Rectangle::construct_hit(const Ray &ray, double time) const {
    std::optional<double> t = triangle0->intersect(ray);
    if (t.has_value()) {
        return triangle0->construct_hit(ray, time);
    }
    return triangle1->construct_hit(ray, time);
}

AABB Rectangle::bounding_box() const {
    return surrounding_box(triangle0->bounding_box(), triangle1->bounding_box());
}

std::pair<double, double> Rectangle::get_uv(const Hit& hit) const {
    Ray ray{hit.position, -(hit.normal)};
    std::optional<double> t = triangle0->intersect(ray);
    if (t.has_value()) {
        return triangle0->get_uv(hit);
    }
    return triangle1->get_uv(hit);
}



