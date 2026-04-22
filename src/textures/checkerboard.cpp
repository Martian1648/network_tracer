/*
ὄνομα: checkerboard
αἰών: 3/31/2025
βούλημα: raytracer
*/

#include "checkerboard.h"

#include <cmath>

Checkerboard::Checkerboard(Color primary, Color secondary)
    :primary{primary}, secondary{secondary}{

}

Color Checkerboard::value(double u, double v)const {
    /*u = fmod(12*u, 12.0);
    v = fmod(6*v, 6.0);
    if (fmod(u+v,2)==0) {
        return primary;
    }
    else {
        return secondary;
    }
    */
    int u_tile = static_cast<int>(floor(u*12));
    int v_tile = static_cast<int>(floor(v*6));
    if ( (u_tile + v_tile) % 2 == 0) {
        return primary;
    }
    else {
        return secondary;
    }

}