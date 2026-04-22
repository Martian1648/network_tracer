/*
ὄνομα: random
αἰών: 2/17/2025
βούλημα: raytracer
*/

#include "random.h"
#include <random>
#include "vector3d.h"
#include "pcg_random.hpp"
class Camera;


double random_double() {
    static thread_local pcg_extras::seed_seq_from<std::random_device> seed_seq;
    static thread_local pcg32 engine(seed_seq);
    static thread_local std::uniform_real_distribution<double> distribution{0.0, 1.0};
    return distribution(engine);
}



double random_double(double min, double max) {
    return (max-min) * random_double() + min;
}

Vector3D random_unit_vector() {
    while (true) {
        double x = random_double(-1,1);
        double y = random_double(-1,1);
        double z = random_double(-1,1);
        double length_sqrd = (x*x + y*y + z*z);
        if (1e-160 < length_sqrd && length_sqrd < 1) {
            Vector3D v{x,y,z};
            return v/sqrt(length_sqrd);
        }
    }
}

Vector3D random_hemisphere(const Vector3D& normal) {
    Vector3D v = random_unit_vector();
    if (dot(v, normal) > 0) {
        return v;
    }
    else {
        return -v;
    }
}

