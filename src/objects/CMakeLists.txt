add_library(objects
        sphere.h sphere.cpp
        triangle.cpp
        triangle.h
        rectangle.cpp
        rectangle.h
        constantmedium.cpp
        constantmedium.h
)

target_link_libraries(objects PUBLIC core)
target_include_directories(objects PUBLIC .)