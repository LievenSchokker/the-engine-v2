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
The easiest way to install the prerequisited libraries on Windows is to make use of vcpkg.
A quick setup guide for vcpkg can be found via this [link](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-powershell).

To permanently recognize vcpkg as a system-wide command use this in POWERSHELL AS ADMIN:
```shell
[System.Environment]::SetEnvironmentVariable(
  "Path",
  $env:Path + ";C:path\to\vcpkg",
  [System.EnvironmentVariableTarget]::Machine
)
```
After successfully installing vcpkg run the following command to install all dependencies:

```bash
vcpkg install
```

**Note:** For documentation generation, you'll also need Doxygen. Download it from [doxygen.org](https://www.doxygen.nl/download.html) or try installing via vcpkg (availability may vary by platform).

The following command is to make sure CMake can find each package.
A variable will be shown, copy that variable and place it inside the CMAKE build config.

For Clion go to -> File | Settings | Build, Execution, Deployment | CMake,
then copy and paste the command into launch options.
```bash
vcpkg integrate install
```


### MacOS Setup
The easiest way to get started on macOS  is to use Brew to install all 
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
```bash
brew install doxygen
```

### Linux Setup
Most external packages can be installed from your distro's package manager.

Apt:
```
# apt install libsdl2-2.0-0 libsdl2-dev ibjpeg-dev libwebp-dev
libtiff5-dev libsdl2-image-dev libsdl2-image-2.0-0
libmikmod-dev libfishsound1-dev libsmpeg-dev liboggz2-dev libflac-dev
libfluidsynth-dev libsdl2-mixer-dev libsdl2-mixer-2.0-0
libfreetype6-dev libsdl2-ttf-dev libsdl2-ttf-2.0-0
googletest libgtest-dev
```

Some packages need to be manually installed, like Valve's  GameNetworkingSockets:
```
# apt install libssl-dev libprotobuf-dev protobuf-compiler

$ git clone https://github.com/ValveSoftware/GameNetworkingSockets.git

$ mkdir build

$ cd build

$ cmake -G Ninja ..

$ ninja

# cmake --install .
```

For documentation generation, install Doxygen:
```bash
# apt install doxygen
```

## Dependencies
The project uses the following external libraries:
- SDL2 - Simple DirectMedia Layer
- GameNetworkingSockets - Valve's networking library
- GoogleTest - For unit testing

These dependencies are installed with vcpkg on windows, and it also has been confirmed that it is possible to install the packaged with MSYS2 on Windows.

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

## Generating Documentation

The engine includes Doxygen comments in all public headers. To generate HTML documentation:

### Prerequisites

**Windows:**
- Download and install Doxygen from [doxygen.org](https://www.doxygen.nl/download.html)
- Or use vcpkg: `vcpkg install doxygen` (if available for your platform)

**macOS:**
```bash
brew install doxygen
```

**Linux:**
```bash
# Debian/Ubuntu
sudo apt install doxygen

# Fedora
sudo dnf install doxygen

# Arch Linux
sudo pacman -S doxygen
```

### Building Documentation

After installing Doxygen and configuring CMake, generate the documentation:

**Using CMake:**
```bash
cd build
cmake --build . --target docs
```

**Using Make (if using Unix Makefiles generator):**
```bash
cd build
make docs
```

The documentation will be generated in `build/docs/html/index.html`. Open this file in your web browser to view the API documentation.

To regenerate documentation after making changes to Doxygen comments, simply run the build command again.