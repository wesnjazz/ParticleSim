cmake_minimum_required(VERSION 3.10)
project(ParticleSim)

# set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

add_executable(ParticleSim
    src/main.cpp
    # src/particle.cpp
)

target_include_directories(ParticleSim PRIVATE include)

target_include_directories(ParticleSim PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(ParticleSim ${OpenCV_LIBRARIES})

