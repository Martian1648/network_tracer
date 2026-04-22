/*
ὄνομα: gradient
αἰών: 3/25/2025
βούλημα: raytracer
*/

#pragma once
#include "texture.h"
class Gradient : public Texture {
public:
    Gradient(Color color_a, Color color_b, bool vertical = false);
    Color value(double u, double v) const override;

    Color color_a, color_b;
    bool vertical;
};
