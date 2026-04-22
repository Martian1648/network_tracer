#include "vector3d.h"
#include "constants.h"
#include <iostream>
#include <cmath>

Vector3D::Vector3D()
        :x{0.0}, y{0.0}, z{0.0} {}

Vector3D::Vector3D(double x, double y, double z)
        :x{x}, y{y}, z{z} {}

Vector3D& Vector3D::operator+=(const Vector3D& rhs) {
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
    return *this;
}

Vector3D& Vector3D::operator-=(const Vector3D& rhs) {
    this->x -= rhs.x;
    this->y -= rhs.y;
    this->z -= rhs.z;
    return *this;
}

Vector3D& Vector3D::operator*=(const Vector3D& rhs) {
    this->x *= rhs.x;
    this->y *= rhs.y;
    this->z *= rhs.z;
    return *this;
}

Vector3D& Vector3D::operator*=(const double scalar) {
    this->x *= scalar;
    this->y *= scalar;
    this->z *= scalar;
    return *this;
}

Vector3D& Vector3D::operator/=(const double scalar) {
    if(scalar == 0){throw std::overflow_error("Cannot Divide by 0");}
    this->x *= 1/scalar;
    this->y *= 1/scalar;
    this->z *= 1/scalar;
    return *this;
}

bool almost_equal(double x, double y) {
    return std::abs(x - y) < Constants::epsilon;
}

bool operator==(const Vector3D& lhs, const Vector3D& rhs) {
    return almost_equal(lhs.x, rhs.x)
           && almost_equal(lhs.y, rhs.y)
           && almost_equal(lhs.z, rhs.z);
}

bool operator!=(const Vector3D& lhs, const Vector3D& rhs) {
    return !(lhs == rhs);
}

Vector3D operator+(Vector3D lhs, const Vector3D& rhs) {
    return lhs += rhs;
}

Vector3D operator-(Vector3D lhs, const Vector3D& rhs) {
    return lhs -= rhs;
}

Vector3D operator-(const Vector3D& rhs) {
    return Vector3D{-1 * rhs.x, -1 * rhs.y, -1 * rhs.z};
}

Vector3D operator*(Vector3D lhs, const Vector3D& rhs) {
    return lhs *= rhs;
}

Vector3D operator*(Vector3D lhs, const double scalar) {
    return lhs *= scalar;
}

Vector3D operator*(const double scalar, Vector3D rhs) {
    return rhs *= scalar;
}

Vector3D operator/(Vector3D lhs, const double scalar) {
    return lhs /= scalar;
}

Vector3D pow(Vector3D v, const double n) {
    v.x = std::pow(v.x, n);
    v.y = std::pow(v.y, n);
    v.z = std::pow(v.z, n);
    return v;
}

double dot(const Vector3D& a, const Vector3D& b) {
    Vector3D vector = a * b;
    return vector.x + vector.y + vector.z;
}

Vector3D cross(const Vector3D& a, const Vector3D& b) {
    Vector3D vector{};
    vector.x = (a.y * b.z) - (a.z * b.y);
    vector.y = (a.z * b.x) - (a.x * b.z);
    vector.z = (a.x * b.y) - (a.y * b.x);
    return vector;
}

double length(const Vector3D& v) {
    return std::pow(
    v.x * v.x +
    v.y * v.y +
    v.z * v.z, .5
    );
}
/*
            */
Vector3D unit(const Vector3D& v) {
    if(v.x == 0 && v.y == 0 && v.z == 0)
    {throw std::overflow_error("Unit vector must exist");}
    double l = length(v);
    return Vector3D{v.x/l, v.y/l, v.z/l};
}

std::ostream& operator<<(std::ostream& os, const Vector3D& v) {
    return os << "(" << v.x << " " << v.y << " " << v.z << ")";
}

std::istream& operator>>(std::istream& is, Vector3D& v) {
    char paren;
    return is >> paren >> v.x >> v.y >> v.z >> paren;
}
