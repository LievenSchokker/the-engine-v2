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

- ### External Libraries:
  - SDL2 - Simple DirectMedia Layer
  - SDL2_Image - Simple DirectMedia Layer
  - GameNetworkingSockets - Valve's networking library
  - GoogleTest - For unit testing

### Windows  Setup
For windows the easiest way to install each of the prerequisited librarie's
We recommand VCPK a quick setup guide for VCPK can be found via this [link](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-powershell).

To permantly recognize VCPKG as a system wide command use this in POWERSHELL AS ADMIN:
```shell
[System.Environment]::SetEnvironmentVariable(
  "Path",
  $env:Path + ";C:path\to\vcpkg",
  [System.EnvironmentVariableTarget]::Machine
)
```
After succesfully installing VCPKG just run the following command to install all dependencies

```bash
vcpkg install
```
And then the following command to make sure Cmake can find each package A variable will be shown
copy that variable and place it inside the CMAKE build config or for 
Clion go to -> File | Settings | Build, Execution, Deployment | CMake 
then copy paste the command into launch options.
```bash
vcpkg integrate install
```


### MacOs Setup
The easiest way to get started on MacOs  is to use Brew to install all 
the dependencies. HomeBrew can be installed via this [link](https://brew.sh/).

Each of the packages can be installed via the following commands
```bash
brew install sdl2
```
```bash
brew install sdl2_image
```
```bash
brew install gamenetworkingsockets
```
```bash
brew install googletest
```

### Linux Setup
sorry don't own a linux pc :( good luck!

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

## Rendering Simple Shapes

GameObjects can render solid 2D primitives without touching SDL directly. Attach a `ShapeRenderer`
component and describe the shape, color, and transform.

```
auto circleGO = std::make_unique<GameObject>();
circleGO->getTransform()->setPosition({150.0, 140.0});
circleGO->getTransform()->setScale({1.0, 1.0});
circleGO->addComponent<ShapeRenderer>()
    ->setCircle(50.0)
    .setColor(Color::blue());

auto rectGO = std::make_unique<GameObject>();
rectGO->getTransform()->setPosition({320.0, 240.0});
rectGO->getTransform()->setRotationAngle(25.0);
rectGO->addComponent<ShapeRenderer>()
    ->setRectangle({140.0, 80.0})
    .setColor(Color::yellow());

scene->addGameObject(std::move(circleGO));
scene->addGameObject(std::move(rectGO));

RenderQueue queue;
sceneManager.buildRenderQueue(queue);
executeRenderQueue(renderer, queue); // Draws both shapes using the object's transforms
```

Shapes automatically follow the owning object's position, rotation, and scale every frame. Build a
`RenderQueue` from the active scene when you're ready and let the renderer consume it on its own timing.