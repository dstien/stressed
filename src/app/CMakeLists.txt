cmake_minimum_required(VERSION 3.16)

find_package(Qt5 REQUIRED COMPONENTS Widgets OpenGL)

add_executable(app
    main.cpp
    mainwindow.cpp
    resource.cpp
    resourcesmodel.cpp
    settings.cpp
    stunpack.c
    gpcunpack.c

    mainwindow.ui

    mainwindow.h
    resource.h
    resourcesmodel.h
    settings.h
    stunpack.h
    gpcunpack.h

    ../../resources/resources.qrc
)

set_target_properties(app PROPERTIES OUTPUT_NAME "stressed")

target_include_directories(app
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(app
    PRIVATE
        Qt5::Widgets
        Qt5::OpenGL
        animation
        bitmap
        raw
        shape
        speed
        text
)

set_target_properties(app PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

if(WIN32)
    target_link_libraries(app PRIVATE opengl32 glu32)
    target_sources(app PRIVATE ../../resources/resources-win32.rc)
else()
    target_link_libraries(app PRIVATE  GLU)
endif()

 if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_link_options(app PRIVATE
     -static-libgcc
     -static-libstdc++
 )
endif()

target_compile_options(app PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)

