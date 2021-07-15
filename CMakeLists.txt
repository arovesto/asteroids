
# You are free to modify this file

cmake_minimum_required(VERSION 3.0)
project(game)
find_package(X11 REQUIRED)


set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")
file(GLOB SRC *.cpp)
add_executable(game ${SRC})

add_custom_target(copy-runtime-files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
    DEPENDS ${MY_TARGET})
target_link_libraries(game m X11 sfml-audio sfml-system)
