/*
ὄνομα: marble
αἰών: 4/28/2025
βούλημα: raytracer
*/

#include "marble.h"

#include "constants.h"
#include "noise.h"

Color Marble::value(double u, double v) const {
    double theta = v * Constants::pi;
    double phi = (u-.5) * Constants::pi;
    double x = std::sin(theta) * std::cos(phi);
    double y = std::sin(theta) * std::sin(phi);
    double z = std::cos(theta);

    Color color{.5, .5, .5   };
    double noise = Perlin::turbulence(x, y, z, 5);

    noise = Perlin::turbulence(x + noise, y + noise, z + noise, 5);

    color += .5*(noise + 1) * Color{.5, .5, .2};

    noise = Perlin::turbulence(x + noise, y + noise, z + noise, 5);

    color += .5*(noise + 1) * Color{.2, .2, .5};

    noise = Perlin::turbulence(x + noise, y + noise, z + noise, 5);
    return color * (noise + 1);
}
