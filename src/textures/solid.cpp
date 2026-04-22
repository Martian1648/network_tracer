/*
ὄνομα: solid
αἰών: 3/22/2025
βούλημα: raytracer
*/

#include "solid.h"
Solid::Solid(Color color)
:Texture{}, color {color} {
}

Color Solid::value(double u, double v) const {
    return color;
}
