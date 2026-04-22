/*
ὄνομα: camera
αἰών: 2/10/2025
βούλημα: raytracer
*/

#include "camera.h"

#include "constants.h"

Camera::Camera(Point3D position, Point3D target, Vector3D up, double fov, double aspect)
    :position{position}{
    Vector3D w = unit(target-position);
    Vector3D temp = cross(up, w);
    Vector3D u{0,0,0};
    if (temp == Vector3D(0, 0, 0)) {
        u = Vector3D(1, 0, 0);
    }
    else {
        u = unit(temp);
    }
    Vector3D v = cross(w, u);
    double theta = (fov * Constants::pi) / 180.0;
    double width = 2* tan(theta/2.0);
    double height = width/aspect;
    horizontal = width*u;
    vertical = -height*v;
    upper_left_corner = (position + w) - (horizontal * 0.5) - (vertical * 0.5);
}

Ray Camera::compute_ray(double s, double t) const {
    Vector3D origin = upper_left_corner + (s * horizontal) + (t * vertical) - position;
    return Ray(position, unit(origin));
}

