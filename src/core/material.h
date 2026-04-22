/*
ὄνομα: material
αἰών: 2/19/2025
βούλημα: raytracer
*/

#pragma once
#include "color.h"
#include "texture.h"
class Hit;
class Ray;

class Material {
public:
  Material(std::string name,  Texture* texture, bool emitting);

  virtual Ray scatter(const Ray& ray, const Hit& hit) const = 0;
  virtual ~Material() = default;
  std::string name;
   Texture* texture;
  bool emitting;
private:

};


Vector3D reflect(const Vector3D& v, const Vector3D& n);
Vector3D refract(const Vector3D& v, const Vector3D& n, double index_ratio);
double schlick(double cos_theta, double index_ratio);