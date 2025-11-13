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
These dependencies are loaded with vcpkg on windows

## Project Structure
```
the-engine/
│
├─ CMakeLists.txt
├─ README.md
├─ LICENSE
│
├─ include/                 # Public header files
│   ├─ Engine/
│   │   ├─ Core/
│   │   ├─ Debug/
│   │   ├─ Input/
│   │   ├─ Window/
│   │   ├─ Physics/
│   │   ├─ Renderer/
│   │   ├─ Scene/
│   │   ├─ Utils/
│   │   ├─ GameObject/
│   │   ├─ Network/
│   │
│   ├─ client/
│   │   └─ Client.hpp
│   │
│   └─ server/
│       └─ Server.hpp
│
├─ src/                     # Source files
│   ├─ Engine/
│   │   ├─ Core/
│   │   ├─ Debug/
│   │   ├─ Input/
│   │   ├─ Window/
│   │   ├─ Physics/
│   │   ├─ Renderer/
│   │   ├─ Scene/
│   │   ├─ Utils/
│   │   ├─ GameObject/
│   │   ├─ Network/
│   │
│   ├─ client/
│   │   └─ main.cpp
│   │
│   └─ server/
│       └─ main.cpp
│
├─ tests/                   # Unit tests
│   ├─ Engine/
│   │   ├─ Core/
│   │   ├─ Debug/
│   │   ├─ Input/
│   │   ├─ Window/
│   │   ├─ Physics/
│   │   ├─ Renderer/
│   │   ├─ Scene/
│   │   ├─ Utils/
│   │   ├─ GameObject/
│   │   ├─ Network/
│   │
│   ├─ client/
│   │   └─ main.cpp
│   │
│   └─ server/
│       └─ main.cpp 
│
├─ cmake-build-debug/       # CMake build output folder (ignored by git)
│
└─ .gitignore
```



