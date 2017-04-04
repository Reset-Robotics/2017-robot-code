cmake_minimum_required (VERSION 2.8)

project (ahoy)

find_package(OpenCV REQUIRED)
find_package(ZeroMQ REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${allwpilib/wpilibc/athena/include})
include_directories(${ahoy.h})
include_directories(${WPILib.h})
include_directories(/usr/local/include)
include_directories(/usr/local/lib)

#disabling unneeded CUDA stuff
unset(CUDA_USE_STATIC_CUDA_RUNTIME CACHE)
option(CUDA_USE_STATIC_CUDA_RUNTIME OFF)


#add_executable(ahoy ahoy.cpp)
add_executable(ahoy22 ahoy22.cpp)

#link_directories("ntcore/lib")
#target_link_libraries(ahoy ${OpenCV_LIBS})
target_link_libraries(ahoy22 ${OpenCV_LIBS})
target_link_libraries(ahoy22 zmq)
target_link_libraries(ahoy22 ${allwpilib/wpilibc/athena/include})
target_link_libraries(ahoy22 ${WPILib.h})
target_link_libraries(ahoy22 ${zmq.h})
