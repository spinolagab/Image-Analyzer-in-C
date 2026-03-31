cmake_minimum_required(VERSION 3.16)
project(proj1_sdl3_ttf C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)

add_executable(proj1
    src/main.c
    src/app.c
    src/font_utils.c
    src/image_processing.c
    src/ui.c
)

target_include_directories(proj1 PRIVATE src)

target_link_libraries(proj1 PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    m
)

if(MSVC)
    target_compile_options(proj1 PRIVATE /W4)
else()
    target_compile_options(proj1 PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(proj1 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
