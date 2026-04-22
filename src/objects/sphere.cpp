#include "sphere.h"
#include <cmath>

#include "aabb.h"
#include "constants.h"
#include "vector3d.h"
#include "hit.h"
Hit Sphere::construct_hit(const Ray& ray, double time) const {
    //calculate the surface normal
    Point3D point = ray.at(time);
    Vector3D normal = (point - center) / radius;
    return Hit{time, point, normal, this};
}

std::optional<double> Sphere::intersect_alg(const Ray& ray) const{
    //calculate the intersection on an algebraic
    double a = dot(ray.direction, ray.direction);
    double b = 2.0*dot((ray.direction), (this->center - ray.origin));
    double c = (dot((this->center - ray.origin),(this->center - ray.origin)))-std::pow(this->radius, 2);
    double disc = std::pow(b,2) - (4*a*c);
    if(disc < -(Constants::epsilon)){
        return {};
    }
    else if(disc > -(Constants::epsilon) && disc < Constants::epsilon){
        return ((b - sqrt(disc))/(2*a));
    }
    else if(disc > Constants::epsilon){
        return ((b - sqrt(disc))/(2*a));
    }
    ////BE SURE TO INCLUDE BOTH DISC IS 1 & 2
    return {};
}

std::optional<double> Sphere::intersect(const Ray& ray) const {
    Vector3D OC = center - ray.origin;
    double R = dot(ray.direction, OC);
    double h2 = radius*radius - dot(OC, OC) + R*R;
    if (h2 < 0) {
        return {};
    }

    double h = std::sqrt(h2);

    // time = R +/- h
    if ((R-h) > Constants::epsilon) {
        // ray intersects from outside sphere -> R-h is closer to ray origin
        // if time is positive and futher away than Epsilon
        return R-h;
    }
    else if ((R+h) > Constants::epsilon) {
        // ray intersections from inside sphere -> R+h is in the direction of the ray
        // time is positive and further away than Epsilon
        return R+h;
    }
    else {
        return {};
    }
}

Sphere::Sphere(const Point3D& center, double radius, const Material* material, Vector3D angle)
:Object{material}, center{center}, angle{angle}, radius{radius}{
}

std::pair<double, double> Sphere::get_uv(const Hit& hit) const {
    double u, v;

    double pitch = (angle.x)*(Constants::pi/180); // x
    double roll  = (angle.y)*(Constants::pi/180); // y
    double yaw   = (angle.z)*(Constants::pi/180); // z

    Vector3D n = hit.normal;

    // Yaw (around Z axis)
    double x1 = cos(yaw) * n.x - sin(yaw) * n.y;
    double y1 = sin(yaw) * n.x + cos(yaw) * n.y;
    double z1 = n.z;

    // Pitch (around X axis)
    double x2 = x1;
    double y2 = cos(pitch) * y1 - sin(pitch) * z1;
    double z2 = sin(pitch) * y1 + cos(pitch) * z1;

    // Roll (around Y axis)
    double x3 = cos(roll) * x2 + sin(roll) * z2;
    double y3 = y2;
    double z3 = -sin(roll) * x2 + cos(roll) * z2;

    // Now compute UVs with rotated normal
     u = (atan2(y3, x3) / (2.0 * Constants::pi)) + 0.5;
     v = acos(z3) / Constants::pi;
    return {u, v};
}

AABB Sphere::bounding_box() const {
    Point3D R{radius, radius, radius};
    return AABB{center - R, center + R};
}