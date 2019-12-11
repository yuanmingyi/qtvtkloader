cmake_minimum_required(VERSION 2.8.11)

project(QtObjLoader)

set(VTK_DIR "/Volumes/Data/Software/vtk")
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# List source files & resources
file (GLOB_RECURSE Mains *.cxx)
file (GLOB_RECURSE Sources *.cpp)
file (GLOB_RECURSE Headers *.h)
file (GLOB_RECURSE Resources *.qrc)
file (GLOB_RECURSE UIs *.ui)

# Find package Qt
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)

# Use Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
qt5_add_resources(Resources ${Resources})

# Find package VTK
find_package(VTK REQUIRED)

# Use VTK
include(${VTK_USE_FILE})
include_directories(${VTK_INCLUDE_DIRS})

# Build executable
add_executable(QtObjLoader MACOSX_BUNDLE main.cxx ${Sources} ${Headers} ${Resources} ${UIs})
add_executable(vtkObjLoader MACOSX_BUNDLE console.cxx vtk_obj_reader.cpp)

# Link libraries
target_link_libraries(QtObjLoader Qt5::Widgets Qt5::OpenGL ${VTK_LIBRARIES})
target_link_libraries(vtkObjLoader ${VTK_LIBRARIES})
