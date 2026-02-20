cmake_minimum_required(VERSION 3.16)
project(Gravity LANGUAGES CXX VERSION 0.5.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

find_package(SDL3 REQUIRED)

add_compile_options(-Wall -Wextra -Werror -pedantic -Wshadow -O0 -g)

set(IMGUI_DIR "third_party/imgui")

add_executable(Gravity
    src/main.cpp
    src/simulation.cpp
    src/map.cpp
    src/viewport.cpp
    src/particle.cpp

    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp

    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)

target_include_directories(Gravity PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/errors
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends
)

target_link_libraries(Gravity PRIVATE
    SDL3::SDL3
)
