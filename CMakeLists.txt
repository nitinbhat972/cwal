cmake_minimum_required(VERSION 3.12)
project(cwal LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(MagickWand REQUIRED MagickWand)
pkg_check_modules(imagequant REQUIRED imagequant)

# Collect source files
file(GLOB_RECURSE SOURCES 
    "${PROJECT_SOURCE_DIR}/src/*.c"
    "${PROJECT_SOURCE_DIR}/src/utils/*.c"
    "${PROJECT_SOURCE_DIR}/src/backends/*.c"
)

# Create executable
add_executable(cwal ${SOURCES})

# Include directories
target_include_directories(cwal PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src/utils
    ${MagickWand_INCLUDE_DIRS}
    ${imagequant_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(cwal
    ${MagickWand_LIBRARIES}
    ${imagequant_LIBRARIES}
    m
)

# Set compiler flags for dependencies
target_compile_options(cwal PRIVATE 
    ${MagickWand_CFLAGS_OTHER}
    ${imagequant_CFLAGS_OTHER}
)

# Installation
install(TARGETS cwal DESTINATION bin)
install(DIRECTORY templates DESTINATION share/cwal)
