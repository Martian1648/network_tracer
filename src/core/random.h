/*
ὄνομα: random
αἰών: 2/17/2025
βούλημα: raytracer
*/

#pragma once
#include "vector3d.h"

double random_double(); //[0,1)

double random_double(double min, double max);

Vector3D random_unit_vector();

Vector3D random_hemisphere(const Vector3D& normal);