/*
ὄνομα: swirl
αἰών: 3/31/2025
βούλημα: raytracer
*/

#pragma once
#include "texture.h"

class Swirl : public Texture {
    public:
    Swirl(Color primary, Color secondary, Color tertiary, Color base);
    Color value(double u, double v) const override;

    Color primary, secondary, tertiary, base;
};
