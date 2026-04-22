#include "vector3d.h"
#include <iostream>

int main() {
    std::cout << "Enter a vector in the form (x y z) and I'll tell you its length: ";
    Vector3D v;
    std::cin >> v;
    std::cout << "You entered: " << v << '\n';
    std::cout << "Its length is " << length(v) << '\n';
}
