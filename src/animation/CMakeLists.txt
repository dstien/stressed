cmake_minimum_required(VERSION 3.16)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_library(animation STATIC
    animationresource.cpp
    animationresource.h
    animationresource.ui
)

target_link_libraries(animation
    PRIVATE Qt5::Widgets
)

target_include_directories(animation
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

set_target_properties(animation PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)
