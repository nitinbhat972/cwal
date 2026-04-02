cmake_minimum_required(VERSION 3.12)

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" CWAL_VERSION LIMIT_COUNT 1)
string(STRIP "${CWAL_VERSION}" CWAL_VERSION)

project(cwal VERSION ${CWAL_VERSION} LANGUAGES C)

# Set C standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Find dependencies
find_package(PkgConfig REQUIRED)


pkg_check_modules(MagickWand REQUIRED IMPORTED_TARGET MagickWand)
pkg_check_modules(imagequant REQUIRED IMPORTED_TARGET imagequant)
pkg_check_modules(Lua REQUIRED IMPORTED_TARGET luajit)

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
    src/modules/reload/reload.c
    src/modules/template/template.c
    src/modules/theme/themes.c
    src/utils/format_conversion.c
    src/utils/path.c
    src/utils/utils.c
)

# Create executable
add_executable(cwal ${SOURCES})

target_compile_definitions(cwal PRIVATE CWAL_VERSION="${PROJECT_VERSION}")

# Link libraries and include directories via imported targets
target_link_libraries(cwal
    PRIVATE
    PkgConfig::MagickWand
    PkgConfig::imagequant
    PkgConfig::Lua
    m
)

# Include internal project directories
target_include_directories(cwal PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)

# Installation
install(TARGETS cwal DESTINATION bin)
install(DIRECTORY templates DESTINATION share/cwal)
install(DIRECTORY themes DESTINATION share/cwal)

# Install shell completions
install(FILES shell/bash/cwal DESTINATION share/bash-completion/completions)
install(FILES shell/zsh/_cwal DESTINATION share/zsh/site-functions)
install(FILES shell/fish/cwal.fish DESTINATION share/fish/vendor_completions.d)
