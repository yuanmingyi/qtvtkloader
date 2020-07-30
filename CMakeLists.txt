cmake_minimum_required(VERSION 2.8.11)

project(QtObjLoader)

# Find package Qt
find_package (Qt5 COMPONENTS Core Gui Widgets OpenGL REQUIRED)

IF(Qt5_FOUND)
    MESSAGE(STATUS "Qt5 found.")
ENDIF()

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

if (VTK_LIB_DIR)
    message(STATUS "VTK_LIB_DIR: ${VTK_LIB_DIR}")
else()
    message(FATAL_ERROR "VTK_LIB_DIR not set")
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# List source files & resources
set (LibSources
    scenewidget.cpp cameraanimation.cpp objimporter.cpp dongfengvis.cpp dongfenganimation.cpp animationsceneobserver.cpp dongfenganimationcue.cpp)
set (LibHeaders
    scenewidget.h timerutil.h util.h cameraanimation.h objimporter.h stringutil.h dongfengvis.h dongfenganimation.h animationsceneobserver.h dongfenganimationcue.h)
set (DemoSources
    main.cpp mainwindow.cpp mainwindow.h)
file (GLOB Resources *.qrc)
file (GLOB UIs *.ui)
file (STRINGS lib.txt VTKLibs)

# Use Qt
qt5_add_resources(Resources ${Resources})

# Build executable
add_library(DongfengQt_dep STATIC ${LibSources} ${LibHeaders})
target_link_libraries(DongfengQt_dep Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL ${VTK_LIBRARIES})

IF (WIN32)
    message(STATUS "on windows")
    set(DongfengQt_LIB DongfengQt.lib)
    add_custom_target(DongfengQt_cmb
        ALL
        COMMAND echo "combine vtk libs to DongfengQt.lib"
        COMMAND lib.exe /libpath:${VTK_LIB_DIR} /out:${DongfengQt_LIB} $<TARGET_FILE:DongfengQt_dep> ${VTKLibs}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS DongfengQt_dep
    )
ELSEIF (UNIX)
    message(STATUS "on unix-like system")
    set(DongfengQt_LIB libDongfengQt.a)
    add_custom_command(TARGET DongfengQt_dep
        PRE_BUILD
        COMMAND rm -f *.o
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    foreach (lib ${VTKLibs})
        get_filename_component(libname ${lib} NAME_WLE)
        add_custom_command(TARGET DongfengQt_dep
            PRE_BUILD
            COMMAND ar x ${VTK_LIB_DIR}/lib${libname}.a
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    endforeach()
    add_custom_target(DongfengQt_cmb
        ALL
        COMMAND ar x $<TARGET_FILE:DongfengQt_dep>
        COMMAND ar qcs ${DongfengQt_LIB} *.o
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS DongfengQt_dep
    )
ELSE()
    message(FATAL_ERROR "unsupported OS")
ENDIF()

add_executable(QtObjLoader ${DemoSources} ${Resources} ${UIs})
add_dependencies(QtObjLoader DongfengQt_cmb)
target_link_libraries(QtObjLoader
    Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL
    ${DongfengQt_LIB})
