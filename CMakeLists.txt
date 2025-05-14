# Set Project and Minimum Version
cmake_minimum_required(VERSION 3.10)
project(Epimeros)

# Exports compile_commands.json (which clangd uses)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add all source files to "sources"
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

# Create Compiler Executable
add_executable(Epimeros ${SOURCES})

# Require C23 for binary constants
set_property(TARGET Epimeros PROPERTY C_STANDARD 23)

# Include only Epimeros headers (not internal libs) for external projects
target_include_directories(Epimeros
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# Enable extensive compile errors w/ debugging
target_compile_options(Epimeros
    PRIVATE
        -Wall
        -Wpedantic
        -Werror
        -ggdb
)