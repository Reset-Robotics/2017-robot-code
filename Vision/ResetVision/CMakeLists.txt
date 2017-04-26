#Require correct version of cmake
cmake_minimum_required (VERSION 2.8)

#Set project name
project (resetVision)

#Find libraries
find_package(OpenCV REQUIRED)
find_package(ZeroMQ REQUIRED)

#Set flags for C++ standard libraries
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#Include directories
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${allwpilib/wpilibc/athena/include})
include_directories(${WPILib.h})
include_directories(/usr/local/include)
include_directories(/usr/local/lib)

#Disabling unneeded CUDA stuff
unset(CUDA_USE_STATIC_CUDA_RUNTIME CACHE)
option(CUDA_USE_STATIC_CUDA_RUNTIME OFF)

#Creating executable for ahoy script
add_executable(singleStreamVision singleStreamVision.cpp)
add_executable(doubleStreamVision doubleStreamVision.cpp)

#Link libraries to ahoy executable
target_link_libraries(singleStreamVision ${OpenCV_LIBS})
target_link_libraries(singleStreamVision zmq)
target_link_libraries(singleStreamVision ${allwpilib/wpilibc/athena/include})
target_link_libraries(singleStreamVision ${WPILib.h})
target_link_libraries(singleStreamVision ${zmq.h})

target_link_libraries(doubleStreamVision ${OpenCV_LIBS})
target_link_libraries(doubleStreamVision zmq)
target_link_libraries(doubleStreamVision ${allwpilib/wpilibc/athena/include})
target_link_libraries(doubleStreamVision ${WPILib.h})
target_link_libraries(doubleStreamVision ${zmq.h})
