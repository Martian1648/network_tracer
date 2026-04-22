/*
ὄνομα: texture
αἰών: 3/21/2025
βούλημα: raytracer
*/

#pragma once
#include "color.h"
class Texture {
public:
    Texture();
    virtual ~Texture() {};
    virtual Color value(double u, double v) const = 0 ;

};

/*
Solid: texture
    Color value(u,v){
    return c;
}

Gradient(a, b)
value(u,v){
    u * a + (1-u) + b;

Image{

}

Color value(u, v){
u = 12u % 12
v = 6v % 6
if((u+v)%2==0){

{
}
}
 */
