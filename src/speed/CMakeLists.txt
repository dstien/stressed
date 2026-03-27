cmake_minimum_required(VERSION 3.16)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_library(speed STATIC
    speedresource.cpp
    speedresource.h
    speedresource.ui
)

target_link_libraries(speed
    PRIVATE Qt5::Widgets
)

target_include_directories(speed
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

set_target_properties(speed PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
)

target_compile_options(speed PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)

