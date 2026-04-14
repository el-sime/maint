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

add_executable(maint main.c screen_gameplay.c screen_logo.c modules.c)
target_link_libraries(maint PRIVATE raylib)

add_custom_command(TARGET maint POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:maint>/assets
)
