/*
ὄνομα: checkerboard
αἰών: 3/31/2025
βούλημα: raytracer
*/

#pragma once
#include "texture.h"

class Checkerboard : public Texture {
public:
    Checkerboard(Color primary, Color secondary);
    Color value(double u, double v)const  override;

    Color primary, secondary;
};
