# The Engine

A modern C++20 project.

## Overview

This project uses modern C++20 features and is built using CMake. It includes several third-party dependencies managed through CMake's dependency system as git submodules.

## Prerequisites

- C++20 compatible compiler
  - GCC 10+ or
  - Clang 11+ or
  - MSVC 2019+ with latest updates
- CMake 3.28.3 or higher

## Dependencies
The project uses the following external libraries:
- SDL3 - Simple DirectMedia Layer
- GameNetworkingSockets - Valve's networking library
- GoogleTest - For unit testing

## Project Structure
```
the-engine/
│
├─ CMakeLists.txt
├─ README.md
├─ LICENSE
│
├─ include/                 # Public headers for engine_core, client, and server
│   ├─ core/
│   │   ├─ Engine.hpp
│   │   ├─ Renderer.hpp
│   │   └─ Network.hpp
│   │
│   ├─ client/
│   │   └─ Client.hpp
│   │
│   └─ server/
│       └─ Server.hpp
│
├─ src/                     # Source files
│   ├─ core/                # Core engine implementation
│   │   ├─ Engine.cpp
│   │   ├─ Renderer.cpp
│   │   └─ Network.cpp
│   │
│   ├─ client/              # Client application
│   │   └─ main.cpp
│   │
│   └─ server/              # Server application
│       └─ main.cpp
│
├─ tests/                   # Unit tests
│   ├─ test_main.cpp
│   └─ test_engine.cpp
│
├─ external/                # Dependencies (SDL3, GoogleTests, ValveNetworkingSockets etc)
│
├─ cmake-build-debug/       # CMake build output folder (ignored by git)
│
└─ .gitignore
```
