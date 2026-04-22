add_subdirectory(core)
add_subdirectory(materials)
add_subdirectory(objects)
add_subdirectory(textures)
add_library(misc
        world.h world.cpp
        parser.h parser.cpp
        objects/sphere.h objects/sphere.cpp
        camera.h camera.cpp
        pixels.h pixels.cpp
        bvh.h bvh.cpp
        ../json.hpp
        jsonparser.cpp
        jsonparser.h
)
target_include_directories(misc PUBLIC .)
target_link_libraries(misc PUBLIC materials core objects textures)