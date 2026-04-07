cmake_minimum_required(VERSION 3.16)
project(maint C)

set(CMAKE_C_STANDARD 99)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.5
)

FetchContent_MakeAvailable(raylib)

add_executable(maint main.c)
target_link_libraries(maint PRIVATE raylib)
