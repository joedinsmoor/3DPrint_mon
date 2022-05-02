cmake_minimum_required(VERSION 3.15)
project(3dprint_mon)

set(CMAKE_CXX_STANDARD 17)
find_package(OpenCV REQUIRED)
find_package(WiringPi REQUIRED)

include_directories(${WIRINGPI_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(3dprint_mon 3dprint_mon.cpp)
find_library(WIRINGPI_LIBRARIES NAMES wiringPi)
target_link_libraries(3dprint_mon ${OpenCV_LIBS})
target_link_libraries(3dprint_mon ${WIRINGPI_LIBRARIES})