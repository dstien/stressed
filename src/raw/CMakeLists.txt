cmake_minimum_required(VERSION 3.16)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_library(raw STATIC
    rawresource.cpp
    rawresource.h
    rawresource.ui
)

target_link_libraries(raw
    PRIVATE Qt5::Widgets
)

target_include_directories(raw
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

set_target_properties(raw PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
)

target_compile_options(raw PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)

