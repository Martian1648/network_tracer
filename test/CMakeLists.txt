

add_executable(test_materials
        test_material.cpp)

add_executable(test_triangles
        test_triangle.cpp)

add_executable(tracer_of_rays
        tracer.cpp)
add_executable(test-fog test-fog.cpp
        )

add_executable(network_tracer
        network_tracer.cpp)
target_link_libraries(test-fog PRIVATE misc)
target_link_libraries(test_materials PRIVATE misc)
target_link_libraries(test_triangles PRIVATE misc)
target_link_libraries(tracer_of_rays PRIVATE misc)
target_link_libraries(network_tracer PRIVATE misc)