add_library(core
        vector3d.h vector3d.cpp
        point3d.h
        ray.h ray.cpp
        hit.h hit.cpp
        random.h random.cpp
        lodepng.h lodepng.cpp
        material.h material.cpp
        color.h constants.h
        pcg_uint128.hpp pcg_random.hpp pcg_extras.hpp
        object.h object.cpp
        texture.h texture.cpp
        aabb.cpp
        aabb.h
        noise.h
        noise.cpp
)

target_include_directories(core PUBLIC .)