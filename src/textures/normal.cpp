/*
ὄνομα: normal
αἰών: 3/25/2025
βούλημα: raytracer
*/

#include "normal.h"

#include <cmath>

#include "constants.h"

Normal::Normal() {

}

Color Normal::value(double u, double v) const {
    double theta = v * Constants::pi;
    double phi = u * (Constants::pi * 2);
    return Color{-std::sin(theta) * std::cos(phi),
                -std::sin(theta) * std::sin(phi),
                std::cos(theta)};
}
