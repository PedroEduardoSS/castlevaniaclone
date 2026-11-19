# ============================================================================
# Alternative Makefile (copy below into a file named "Makefile"):
# ============================================================================
#
# CXX       := g++
# CXXFLAGS  := -std=c++17 -Wall -Wextra -O2
# SDL_CFLAGS  := $(shell pkg-config --cflags sdl2)
# SDL_LDFLAGS := $(shell pkg-config --libs sdl2)
#
# SRCS := $(wildcard src/*.cpp)
# OBJS := $(SRCS:.cpp=.o)
# TARGET := shadow_knight
#
# all: $(TARGET)
#
# $(TARGET): $(OBJS)
# 	$(CXX) $(CXXFLAGS) -o $@ $^ $(SDL_LDFLAGS)
#
# %.o: %.cpp
# 	$(CXX) $(CXXFLAGS) $(SDL_CFLAGS) -c $< -o $@
#
# clean:
# 	rm -f $(OBJS) $(TARGET)
#
# .PHONY: all clean
# ============================================================================

cmake_minimum_required(VERSION 3.10)
project(ShadowKnight
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "Shadow Knight – A 2D action platformer built with SDL2"
)

# --- Default build type ------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
    message(STATUS "No build type specified – defaulting to Release")
endif()

# --- C++17 standard ----------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Compiler warnings -------------------------------------------------------
add_compile_options(-Wall -Wextra)

# --- Find SDL2 via pkg-config ------------------------------------------------
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

# --- Gather source files -----------------------------------------------------
set(SOURCES
    src/main.cpp
    src/renderer.cpp
    src/player.cpp
    src/enemy.cpp
    src/level.cpp
    src/game.cpp
)

# --- Build executable --------------------------------------------------------
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${SDL2_LIBRARIES}
)

# Pass any extra pkg-config compiler flags (e.g. -D defines)
target_compile_options(${PROJECT_NAME} PRIVATE
    ${SDL2_CFLAGS_OTHER}
)

# --- Output binary name ------------------------------------------------------
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "shadow_knight"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
