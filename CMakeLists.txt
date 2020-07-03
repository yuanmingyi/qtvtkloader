cmake_minimum_required(VERSION 2.8.11)

project(QtObjLoader)

# Find package Qt
find_package (Qt5 COMPONENTS Core Gui Widgets OpenGL REQUIRED)

IF(Qt5_FOUND)
    MESSAGE(STATUS "Qt5 found.")
ENDIF()

# Find package VTK
find_package(VTK REQUIRED)
IF (VTK_FOUND)
    MESSAGE(STATUS "VTK found: ${VTK_USE_FILE}")
    INCLUDE(${VTK_USE_FILE})
ENDIF()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# List source files & resources
set (Sources
    main.cpp mainwindow.cpp scenewidget.cpp vtk_obj_reader.cpp demointeractorstyle.cpp cameraanimationcue.cpp objimporter.cpp dongfengvis.cpp dongfenganimation.cpp animationsceneobserver.cpp dongfenganimationcue.cpp)
set (Headers
    mainwindow.h scenewidget.h timerutil.h util.h vtk_obj_reader.h demointeractorstyle.h cameraanimationcue.h objimporter.h stringutil.h dongfengvis.h dongfenganimation.h animationsceneobserver.h dongfenganimationcue.h)
file (GLOB_RECURSE Resources *.qrc)
file (GLOB_RECURSE UIs *.ui)

# Use Qt
qt5_add_resources(Resources ${Resources})

# Build executable
add_executable(QtObjLoader ${Sources} ${Headers} ${Resources} ${UIs})

# Link libraries
target_link_libraries(QtObjLoader ${VTK_LIBRARIES})
