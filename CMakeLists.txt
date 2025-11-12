cmake_minimum_required(VERSION 3.2...4.0)
project(engine)

set(CMAKE_CXX_STANDARD 20)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(SDL2_image REQUIRED)
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})

file(GLOB_RECURSE SRC
        "src/*.cpp"
)

add_executable(engine src/main.cpp ${SRC})
target_link_libraries(engine SDL2::SDL2 SDL2_image::SDL2_image)
