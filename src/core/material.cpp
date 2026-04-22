/*
ὄνομα: material
αἰών: 2/19/2025
βούλημα: raytracer
*/

#include "material.h"

#include <valarray>

Material::Material(std::string name,  Texture* texture, bool emitting)
:name{name}, texture{texture}, emitting{emitting}{

}

Vector3D reflect(const Vector3D& v, const Vector3D& n) {
    return v - 2 * dot(v, n) * n;
}

Vector3D refract(const Vector3D& v, const Vector3D& n, double index_ratio) {
    double cos_theta = -dot(n, v);
    double discriminant = 1 - std::pow(index_ratio, 2) * (1-std::pow(cos_theta, 2));
    if (discriminant >= 0) {
    //refraction
        return index_ratio * v + (index_ratio * cos_theta - std::sqrt(discriminant)) * n;

    }
    else {
    //internal reflection
        return reflect(v, n);
    }
}

double schlick(double cos_theta, double index_ratio) {
    double r0 = std::pow((1-index_ratio)/ (1+ index_ratio), 2);
    return r0 + (1-r0) * std::pow((1-cos_theta), 5);
}
