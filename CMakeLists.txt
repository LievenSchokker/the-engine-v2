cmake_minimum_required(VERSION 3.16)

project(GameEngine
        VERSION 0.1.0
        DESCRIPTION "Cross-platform SpelMotor"
        LANGUAGES CXX
)

# ============================================================================
# Build Options
# ============================================================================
option(ENGINE_BUILD_TESTS "Build the test suite" ON)
option(ENGINE_ENABLE_WARNINGS "Enable compiler warnings" ON)
option(ENGINE_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)

# ============================================================================
# C++ Standard
# ============================================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================================
# Global Settings
# ============================================================================
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ============================================================================
# Platform Detection & Configuration
# ============================================================================
if(WIN32)
    add_compile_definitions(ENGINE_PLATFORM_WINDOWS)
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN)
elseif(APPLE)
    add_compile_definitions(ENGINE_PLATFORM_MACOS)
elseif(UNIX)
    add_compile_definitions(ENGINE_PLATFORM_LINUX)
endif()

# ============================================================================
# Dependencies
# ============================================================================
#find_package(SDL3 REQUIRED)
#find_package(GameNetworkingSockets REQUIRED)

#if(ENGINE_BUILD_TESTS)
#    find_package(GTest REQUIRED)
#endif()

# ============================================================================
# Engine Core Files
# ============================================================================

file(GLOB_RECURSE ENGINE_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/*.cpp"
)

file(GLOB_RECURSE ENGINE_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_library(engine_core STATIC
        ${ENGINE_SOURCES}
        ${ENGINE_HEADERS}
)

add_library(Engine::Core ALIAS engine_core)

target_include_directories(engine_core
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/engine
)

#target_link_libraries(engine_core
#       PUBLIC
#        SDL3::SDL3
#        GameNetworkingSockets
#)

if(WIN32)
    target_link_libraries(engine_core PRIVATE ws2_32)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(engine_core PRIVATE pthread dl)
endif()

set_target_properties(engine_core PROPERTIES
        FOLDER "Engine"
        VERSION ${PROJECT_VERSION}
)

# ============================================================================
# Tests
# ============================================================================
if(ENGINE_BUILD_TESTS)
    enable_testing()

    file(GLOB_RECURSE TEST_SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
    )

#    add_executable(engine_tests ${TEST_SOURCES})

#    target_link_libraries(engine_tests
#            PRIVATE
#            Engine::Core
#            GTest::gtest_main
#    )

#    set_target_properties(engine_tests PROPERTIES
#            FOLDER "Tests"
#    )

#    include(GoogleTest)
#    gtest_discover_tests(engine_tests)
endif()

# ============================================================================
# Print Configuration Summary
# ============================================================================
message(STATUS "==================== Configuration ====================")
message(STATUS "Game Engine Version: ${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Tests: ${ENGINE_BUILD_TESTS}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "========================================================")