/*
ὄνομα: solid
αἰών: 3/22/2025
βούλημα: raytracer
*/

#pragma once
#include "texture.h"
class Solid : public Texture {
public:
    Solid(Color color);
    Color value(double u, double v)const  override;

    Color color;
};
