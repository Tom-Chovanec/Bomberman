cmake_minimum_required(VERSION 3.16)
project(bomberman)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)
add_subdirectory(src)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY levels DESTINATION ${CMAKE_BINARY_DIR})

target_link_libraries(bomberman PRIVATE SDL3::SDL3)
target_link_libraries(bomberman PRIVATE SDL3_image::SDL3_image)
target_link_libraries(bomberman PRIVATE SDL3_ttf::SDL3_ttf)
