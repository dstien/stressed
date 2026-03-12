cmake_minimum_required(VERSION 3.16)

find_package(OpenGL REQUIRED)

find_package(Qt5 REQUIRED COMPONENTS Widgets OpenGL)

add_library(shape STATIC
    flagdelegate.cpp
    materialdelegate.cpp
    materialsmodel.cpp
    shapemodel.cpp
    shaperesource.cpp
    shapeview.cpp
    typedelegate.cpp
    verticesmodel.cpp

    flagdelegate.h
    materialdelegate.h
    materialsmodel.h
    shapemodel.h
    shaperesource.h
    shapeview.h
    typedelegate.h
    types.h
    verticesmodel.h

    shaperesource.ui
)

target_link_libraries(shape
    PRIVATE
        Qt5::Widgets
        Qt5::OpenGL
        OpenGL::GL
)

target_include_directories(shape
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

set_target_properties(shape PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
)

target_compile_options(shape PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)

