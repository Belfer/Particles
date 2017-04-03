cmake_minimum_required(VERSION 2.8)

project(Particles)
set(${PROJECT_NAME}_MAJOR_VERSION 0)
set(${PROJECT_NAME}_MINOR_VERSION 1)
set(${PROJECT_NAME}_REVISION_VERSION 1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

# Header files
file (GLOB_RECURSE HDRS
    src/*.hpp
    src/*.h
)

# Source files
file (GLOB_RECURSE SRCS
    src/*.cpp
    src/*.cc
    src/*.c
)

add_executable(${PROJECT_NAME} ${HDRS} ${SRCS})

# Define external modules cmake path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

# Find packages.
find_package(OpenGL REQUIRED)
find_package(SFML 2.3 REQUIRED graphics window system audio network)

include_directories(src
    src/core src/gui src/lua src/physics src/serialize src/util
    src/editor
    ${SFML_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    ${OPENGL_LIBRARY}
    ${SFML_LIBRARIES}
)
