cmake_minimum_required(VERSION 3.16)
project(SDL3Hello LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(TinyXML2 CONFIG REQUIRED)

configure_file(${CMAKE_SOURCE_DIR}/config/settings.xml ${CMAKE_BINARY_DIR}/config/settings.xml COPYONLY)
file(COPY ${CMAKE_SOURCE_DIR}/asset DESTINATION ${CMAKE_BINARY_DIR})

enable_testing()
add_subdirectory(src/core)
add_subdirectory(src/graphics)
add_subdirectory(src/engine)
add_subdirectory(src/game)
add_subdirectory(src/main)
add_subdirectory(tests)
