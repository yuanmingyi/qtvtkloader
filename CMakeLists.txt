cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_VERBOSE_MAKEFILE ON)

project(QtObjLoader)

# Find package Qt
find_package (Qt5 COMPONENTS Core Gui Widgets OpenGL REQUIRED)

if (UNIX)
    find_package (Qt5 COMPONENTS X11Extras REQUIRED)
endif()

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

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
    scenewidget.cpp cameraanimation.cpp objimporter.cpp dongfengvis.cpp dongfenganimation.cpp animationsceneobserver.cpp dongfenganimationcue.cpp importworker.cpp)
set (LibHeaders
    scenewidget.h timerutil.h util.h cameraanimation.h objimporter.h stringutil.h dongfengvis.h dongfenganimation.h animationsceneobserver.h dongfenganimationcue.h importworker.h)
set (DemoSources
    main.cpp mainwindow.cpp mainwindow.h)
file (GLOB Resources *.qrc)
file (GLOB UIs *.ui)

# Use Qt
qt5_add_resources(Resources ${Resources})

# Build executable
add_library(DongfengQt STATIC ${LibSources} ${LibHeaders})
target_link_libraries(DongfengQt Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL ${VTK_LIBRARIES})

add_executable(QtObjLoader ${DemoSources} ${Resources} ${UIs})
add_dependencies(QtObjLoader DongfengQt)
target_link_libraries(QtObjLoader Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL DongfengQt)
