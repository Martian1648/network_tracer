cmake_minimum_required(VERSION 3.25.1)
project(raytracer)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# adjust any compiler settings
add_compile_options(-g -Wall -Wextra -pedantic -std=c++20  -O3)

add_subdirectory(src)
add_subdirectory(test)


#[[

# add library for vector class and operations
add_library(vector3d vector3d.cpp)

# test program for vector3d
add_executable(test test-vector3d.cpp)
target_link_libraries(test PUBLIC vector3d)

# another program using vector3d
add_executable(main main.cpp
        color.h
        pixels.h)
target_link_libraries(main PUBLIC vector3d)

add_executable(test-pixels.out
        test-pixels.cpp
        pixels.cpp
        vector3d.cpp
        lodepng.cpp
)


# shapes
add_library(shapes
        ray.h ray.cpp
        sphere.h sphere.cpp
        color.h
)
target_link_libraries(shapes PUBLIC vector3d)
add_executable(test-intersect test-intersect.cpp
        sphere.cpp
        sphere.h
        hit.cpp
        hit.h
pixels.cpp
lodepng.cpp
        camera.h)
target_link_libraries(test-intersect PRIVATE shapes)

add_executable(test_camera
    test_camera.cpp
    camera.h
    camera.cpp
        pixels.cpp
        lodepng.cpp
        hit.cpp
        #world.cpp
        #world.h
        )
target_link_libraries(test_camera PUBLIC shapes)

add_executable(test_random
        random.cpp
        random.h
        test_random.cpp
        pixels.cpp
        lodepng.cpp
        hit.cpp
        world.cpp
        world.h
        camera.cpp
        glass.cpp
        glass.h
)
target_link_libraries(test_random PUBLIC shapes)
target_link_libraries(test_random PUBLIC material)

add_library(material
        material.h
        material.cpp
        diffuse.h
        diffuse.cpp
        camera.h
        camera.cpp
        world.h
        world.cpp
        pixels.h
        pixels.cpp
        hit.h
        hit.cpp
        random.h
        random.cpp
        lodepng.cpp
        sphere.cpp
        lambertian.cpp
        lambertian.h
        specular.cpp
        specular.h
        metal.cpp
        glass.cpp
        glass.h
        metal.h
        parser.cpp
        parser.h
        input.txt
        pcg_extras.hpp
        pcg_random.hpp
        pcg_uint128.hpp
)

add_executable(test_material
        test_material.cpp
)
target_link_libraries(test_material PUBLIC material)
target_link_libraries(test_material PUBLIC shapes)
]]

