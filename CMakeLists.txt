cmake_minimum_required(VERSION 3.16)
project(Gravity LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

add_compile_options(-Wall -Wextra -Werror -pedantic -Wshadow -O0 -g)

add_executable(Gravity
    src/main.cpp
    src/simulation.cpp
    src/map.cpp
    src/viewport.cpp
)

target_include_directories(Gravity PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/errors
)

target_link_libraries(Gravity PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
)
