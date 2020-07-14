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
    MESSAGE(STATUS "VTK_QT_VERSION: ${VTK_QT_VERSION}")
    MESSAGE(STATUS "VTK found: ${VTK_USE_FILE}")
    MESSAGE(STATUS "VTK_Libraries: ${VTK_LIBRARIES}")
    INCLUDE(${VTK_USE_FILE})
ENDIF()

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
file (GLOB_RECURSE LibSources
    scenewidget.cpp cameraanimation.cpp objimporter.cpp dongfengvis.cpp dongfenganimation.cpp animationsceneobserver.cpp dongfenganimationcue.cpp)
file (GLOB_RECURSE LibHeaders
    scenewidget.h timerutil.h util.h cameraanimation.h objimporter.h stringutil.h dongfengvis.h dongfenganimation.h animationsceneobserver.h dongfenganimationcue.h)
file (GLOB_RECURSE DemoSources
    main.cpp mainwindow.cpp)
file (GLOB_RECURSE DemoHeaders
    mainwindow.h ${LibHeaders})
file (GLOB_RECURSE Resources *.qrc)
file (GLOB_RECURSE UIs *.ui)

# Use Qt
qt5_add_resources(Resources ${Resources})

# Build executable
add_library(DongfengQt STATIC ${LibSources} ${LibHeaders})
add_executable(QtObjLoader ${DemoSources} ${DemoHeaders} ${Resources} ${UIs})

# Link libraries
target_link_libraries(DongfengQt Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL)
target_link_libraries(DongfengQt ${VTK_LIBRARIES})
target_link_libraries(QtObjLoader Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL)
target_link_libraries(QtObjLoader DongfengQt)
