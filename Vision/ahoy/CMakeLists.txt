#Require correct version of cmake
cmake_minimum_required (VERSION 2.8)

#Set project name
project (ahoy)

#Find libraries
find_package(OpenCV REQUIRED)
find_package(ZeroMQ REQUIRED)

#Set flags for C++ standard libraries
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#Include directories
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${allwpilib/wpilibc/athena/include})
include_directories(${ahoy.h})
include_directories(${WPILib.h})
include_directories(/usr/local/include)
include_directories(/usr/local/lib)

#Disabling unneeded CUDA stuff
unset(CUDA_USE_STATIC_CUDA_RUNTIME CACHE)
option(CUDA_USE_STATIC_CUDA_RUNTIME OFF)

#Creating executable for ahoy script
add_executable(ahoy22 ahoy22.cpp)
add_executable(ahoy44 ahoy44.cpp)

#Link libraries to ahoy executable
target_link_libraries(ahoy22 ${OpenCV_LIBS})
target_link_libraries(ahoy22 zmq)
target_link_libraries(ahoy22 ${allwpilib/wpilibc/athena/include})
target_link_libraries(ahoy22 ${WPILib.h})
target_link_libraries(ahoy22 ${zmq.h})

target_link_libraries(ahoy44 ${OpenCV_LIBS})
target_link_libraries(ahoy44 zmq)
target_link_libraries(ahoy44 ${allwpilib/wpilibc/athena/include})
target_link_libraries(ahoy44 ${WPILib.h})
target_link_libraries(ahoy44 ${zmq.h})
