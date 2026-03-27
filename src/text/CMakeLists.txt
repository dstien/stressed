cmake_minimum_required(VERSION 3.16)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_library(text STATIC
    textresource.cpp
    textresource.h
    textresource.ui
)

target_link_libraries(text
    PRIVATE Qt5::Widgets
)

target_include_directories(text
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

set_target_properties(text PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
)

target_compile_options(text PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)

