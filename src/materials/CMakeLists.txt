add_library(materials
        lambertian.h lambertian.cpp
        diffuse.h diffuse.cpp
        glass.h glass.cpp
        specular.h specular.cpp
        metal.h metal.cpp
        isotropic.cpp
        isotropic.h
)

target_link_libraries(materials PUBLIC core)
target_include_directories(materials PUBLIC .)