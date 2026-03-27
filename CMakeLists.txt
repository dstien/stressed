cmake_minimum_required(VERSION 3.16)

project(stressed LANGUAGES C CXX)

find_package(Qt5 5.0 REQUIRED COMPONENTS Widgets)

add_subdirectory(./src/animation)
add_subdirectory(./src/bitmap)
add_subdirectory(./src/raw)
add_subdirectory(./src/shape)
add_subdirectory(./src/speed)
add_subdirectory(./src/text)
add_subdirectory(./src/app)
