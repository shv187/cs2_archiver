cmake_minimum_required(VERSION 3.25)

project(cs2_archiver 
    VERSION 1.0 
    LANGUAGES CXX
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_executable(cs2_archiver 
    src/main.cpp
)

target_compile_features(cs2_archiver PUBLIC cxx_std_23)
set_target_properties(cs2_archiver PROPERTIES CXX_EXTENSIONS OFF)

if(MSVC)
    target_compile_options(cs2_archiver PRIVATE /W4 /WX)
else()
    target_compile_options(cs2_archiver PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR} 
    FILES src/main.cpp
)

add_definitions(-DUNICODE -D_UNICODE)