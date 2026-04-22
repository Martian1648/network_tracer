add_library(textures
        solid.h solid.cpp
        gradient.h gradient.cpp
        normal.cpp
        normal.h
        image.cpp
        image.h
        checkerboard.cpp
        checkerboard.h
        swirl.cpp
        swirl.h
        marble.h
        marble.cpp
)

target_link_libraries(textures PUBLIC core)
target_include_directories(textures PUBLIC .)