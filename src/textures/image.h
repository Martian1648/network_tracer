/*
ὄνομα: image
αἰών: 3/28/2025
βούλημα: raytracer
*/

#pragma once

#include "color.h"
#include "texture.h"
#include "../pixels.h"

class Image : public Texture {
public:
    Image(const std::string& filename);
    Color value(double u, double v) const override;

    private:
    Pixels pixels;
};
