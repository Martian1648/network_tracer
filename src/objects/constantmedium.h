/*
ὄνομα: constant_medium
αἰών: 4/14/2025
βούλημα: raytracer
*/

#pragma once
#include "object.h"
class Constant_Medium : public Object {
    public:
    Constant_Medium(Object* boundary, double density, const Material* material);
    ~Constant_Medium() override;
    std::optional<double> intersect(const Ray& ray) const override;
    Hit construct_hit(const Ray &ray, double time) const override;
    std::pair<double, double> get_uv(const Hit &hit) const override;
    AABB bounding_box() const override;
    Object* boundary; //assume ownership
    double density;

};
