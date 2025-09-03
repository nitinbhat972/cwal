cmake_minimum_required(VERSION 3.12)
project(cwal LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(MagickWand REQUIRED MagickWand)
pkg_check_modules(imagequant REQUIRED imagequant)
pkg_check_modules(Lua REQUIRED lua)

# Collect source files
set(SOURCES
    src/app/cli.c
    src/app/config.c
    src/app/main.c
    src/backends/backend.c
    src/backends/cwal.c
    src/backends/libimagequant.c
    src/backends/lua_backend.c
    src/color/color_convertion.c
    src/color/color_operation.c
    src/color/colors.c
    src/color/image.c
    src/modules/cache/cache.c
    src/modules/hooks/hooks.c
    src/modules/reload/reload.c
    src/modules/template/template.c
    src/modules/theme/themes.c
    src/utils/format_conversion.c
    src/utils/path.c
    src/utils/utils.c
)

# Create executable
add_executable(cwal ${SOURCES})

# Include directories
target_include_directories(cwal PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
    ${MagickWand_INCLUDE_DIRS}
    ${imagequant_INCLUDE_DIRS}
    ${Lua_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(cwal
    ${MagickWand_LIBRARIES}
    ${imagequant_LIBRARIES}
    ${Lua_LIBRARIES}
    m
)

# Set compiler flags for dependencies
target_compile_options(cwal PRIVATE
    ${MagickWand_CFLAGS_OTHER}
    ${imagequant_CFLAGS_OTHER}
    ${Lua_CFLAGS_OTHER}
)

# Installation
install(TARGETS cwal DESTINATION bin)
install(DIRECTORY templates DESTINATION share/cwal)
install(DIRECTORY themes DESTINATION share/cwal)
