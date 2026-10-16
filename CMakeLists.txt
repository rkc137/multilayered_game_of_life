cmake_minimum_required(VERSION 3.10)
project(game_of_life LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)
include(FetchContent)

option(USE_FETCHCONTENT "fetching sfml" FALSE)
if(USE_FETCHCONTENT)
    FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.2
    )
    FetchContent_MakeAvailable(SFML)
    message("fetching libs")
else()
    message("searching for sfml. FetchContent for it is off")
    find_package(SFML REQUIRED COMPONENTS Graphics Audio Window System)
endif()

set(SFML_STATIC_LIBRARIES TRUE)
FetchContent_Declare(ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.91.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(ImGui)
FetchContent_GetProperties(ImGui SOURCE_DIR IMGUI_DIR)
set(IMGUI_SFML_FIND_SFML OFF)
FetchContent_Declare(ImGui-SFML
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml
    GIT_TAG v3.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(ImGui-SFML)
target_compile_options(ImGui-SFML PRIVATE -w) # to shutup warnings

FetchContent_Declare(magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG master
)
FetchContent_MakeAvailable(magic_enum)

add_executable(game_of_life src/main.cpp)

target_compile_options(game_of_life PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(game_of_life PRIVATE SFML::Graphics SFML::Audio ImGui-SFML::ImGui-SFML magic_enum)
target_link_options(game_of_life PRIVATE -static)