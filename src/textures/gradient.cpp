/*
ὄνομα: gradient
αἰών: 3/25/2025
βούλημα: raytracer
*/

#include "gradient.h"

#include <cmath>

#include "constants.h"

Gradient::Gradient(Color color_a, Color color_b, bool vertical)
    :color_a{color_a}, color_b{color_b}, vertical {vertical}{

}


Color Gradient::value(double u, double v) const {
    double blend;
    if (vertical) {
        blend = .5*(1- std::cos(v * Constants::pi));
        return (color_a*blend) + (color_b*(1-blend));
    }
    else {
        blend = .5*(1-std::cos(2*(u * Constants::pi)));
        return (color_a*blend) + (color_b*(1-blend));
    }
}
