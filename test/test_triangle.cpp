/*
ὄνομα: test_triangle
αἰών: 3/31/2025
βούλημα: raytracer
*/
#include "ray.h"
#include "triangle.h"
#include "pixels.h"
#include "solid.h"
#include "diffuse.h"

int main() {
    Solid blue{{0, 0, 1}};
    Diffuse diffuse{(&blue), false};

    Triangle t{{-1, 0, 0}, {1, 0, 0}, {0, 0, 1}, &diffuse};

    int rows = 720;
    int cols = 1280;
    double scale = .01;
    Pixels pixels{cols, rows};
    for (int row = 0; row < rows; ++row) {
        double z = scale * (row- rows/2);
        for (int col = 0; col < cols; ++col) {
            double x = scale * (col- cols/2);
            Ray ray{{x,-1,z}, {0, 1, 0}};
            if (t.intersect(ray)) {
                pixels(row, col) = {1, 0, 0};
            }
        }

    }
    pixels.save_png("triangle.png");

}
