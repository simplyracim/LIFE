cmake_minimum_required(VERSION 3.24)

project(3DGameOfLife
    VERSION 1.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# ============================================================
# Dependencies
# ============================================================

# ------------------------------------------------------------
# SFML 3.0.2
# ------------------------------------------------------------

set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW TRUE
)


# ------------------------------------------------------------
# GLFW 3.4
# ------------------------------------------------------------

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
)


# ------------------------------------------------------------
# GLM 1.0.1
# ------------------------------------------------------------

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE
)


# ------------------------------------------------------------
# GLAD 2
# ------------------------------------------------------------

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG glad2
    GIT_SHALLOW TRUE
)


# Download dependencies
FetchContent_MakeAvailable(
    SFML
    glfw
    glm
    glad
)


# ============================================================
# GLAD configuration
# ============================================================

# Generate a static GLAD library for OpenGL 3.3 Core.
glad_add_library(
    glad
    STATIC
    REPRODUCIBLE
    API gl:core=3.3
)


# ============================================================
# Source files
# ============================================================

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)


# ============================================================
# Executable
# ============================================================

add_executable(3DGameOfLife
    ${SOURCES}
)


# ============================================================
# Include directories
# ============================================================

target_include_directories(3DGameOfLife PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui
)


# ============================================================
# Compiler options
# ============================================================

if(MSVC)

    target_compile_options(3DGameOfLife PRIVATE
        /W4
        /O2
    )

else()

    target_compile_options(3DGameOfLife PRIVATE
        -Wall
        -Wextra
        -O2
    )

endif()


# ============================================================
# OpenGL
# ============================================================

find_package(OpenGL REQUIRED)


# ============================================================
# Link libraries
# ============================================================

target_link_libraries(3DGameOfLife PRIVATE

    # GLAD
    glad

    # SFML
    SFML::Graphics
    SFML::Window
    SFML::System

    # GLFW
    glfw

    # GLM
    glm::glm

    # OpenGL
    OpenGL::GL
)


# ============================================================
# Copy shaders
# ============================================================

add_custom_command(TARGET 3DGameOfLife POST_BUILD

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders
        $<TARGET_FILE_DIR:3DGameOfLife>/shaders

)


# ============================================================
# Windows: copy required DLLs
# ============================================================

if(WIN32)

    add_custom_command(TARGET 3DGameOfLife POST_BUILD

        COMMAND ${CMAKE_COMMAND} -E copy
            -t $<TARGET_FILE_DIR:3DGameOfLife>
            $<TARGET_RUNTIME_DLLS:3DGameOfLife>

        COMMAND_EXPAND_LISTS

    )

endif()