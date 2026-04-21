# 2real

My journey to build a game engine that's not quite unreal

## Getting started

### Prerequisites

#### vcpkg

Set up [vcpkg](https://learn.microsoft.com/en-gb/vcpkg/get_started/get-started?pivots=shell-powershell) (with `VCPKG_ROOT` environment variable).

#### Linux

```sh
sudo apt update
sudo apt install cmake ninja-build pkg-config
```

#### macOS

```sh
brew install cmake ninja pkg-config
```

#### Windows

Install the [Microsoft C++ Build Tools](https://visualstudio.microsoft.com/visual-cpp-build-tools/). In the installer, select the "Desktop development with C++" workload with these components:

- MSVC Build Tools for x64/x86
- Windows 11 SDK
- C++ CMake tools for Windows

### Building

1. **Configure**

   ```sh
   cmake --preset default
   ```

2. **Build**

   ```sh
   cmake --build build
   ```

3. **Run a demo** (e.g. [basic-rendering](./demos/basic_rendering/basic_rendering.cpp))
   
   macOS/Linux:
   ```sh
    cd build/bin/basic-rendering
    ./basic-rendering
    ```

    Windows:
    ```sh
    cd build/bin/basic-rendering
    ./basic-rendering.exe
    ```
