# The Engine

A modern C++20 project.

## Overview

This project uses modern C++20 features and is built with CMake. The engine
code is built as a static library that powers both the sandbox application and
the automated test suite.

## Prerequisites

- C++20 compatible compiler
  - GCC 10+ or
  - Clang 11+ or
  - MSVC 2019+ with latest updates
- CMake 3.28.3 or higher

## Dependencies
The project uses the following external libraries:
- SDL2 - Simple DirectMedia Layer
- GameNetworkingSockets - Valve's networking library
- GoogleTest - For unit testing
These dependencies are loaded with vcpkg on windows

## Project Structure
```
the-engine/
│
├─ CMakeLists.txt
├─ README.md
├─ Engine/
│   ├─ inc/                 # Public engine headers
│   └─ src/
├─ Sandbox/
│   ├─ main.cpp
│   └─ input.cpp
└─ Tests/
    ├─ Input/
    └─ Rendering/
```