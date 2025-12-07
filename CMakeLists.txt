cmake_minimum_required(VERSION 3.10)
project(game_of_life LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)
include(FetchContent)

if(DEFINED ENV{CMAKE_LIBS_DIR})
    set(FETCHCONTENT_BASE_DIR "$ENV{CMAKE_LIBS_DIR}")
endif()

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG master
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)
set(SFML_STATIC_LIBRARIES TRUE)

add_executable(game_of_life src/main.cpp)

target_compile_options(game_of_life PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(game_of_life PRIVATE SFML::Graphics SFML::Audio)
target_link_options(game_of_life PRIVATE -static)