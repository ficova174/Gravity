cmake_minimum_required(VERSION 3.16)
project(Gravity LANGUAGES CXX VERSION 0.5.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

find_package(SDL3 REQUIRED)

add_compile_options(-Wall -Wextra -Werror -pedantic -Wshadow -O0 -g)

add_executable(Gravity
    src/main.cpp
    src/simulation.cpp
    src/map.cpp
    src/viewport.cpp
    src/particle.cpp
)

target_include_directories(Gravity PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/errors
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty
)

target_link_libraries(Gravity PRIVATE
    SDL3::SDL3
)
