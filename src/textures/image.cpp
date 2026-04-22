/*
ὄνομα: image
αἰών: 3/28/2025
βούλημα: raytracer
*/

#include "image.h"

Image::Image(const std::string &filename):pixels{Pixels{filename}}
{
}

Color Image::value(double u, double v) const {
    u = (1-u) * pixels.columns;
    v *= pixels.rows;

    return pixels(v, u);
}
