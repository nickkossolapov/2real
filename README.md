# 2real

My journey to build a game engine that's not quite unreal

## Getting started

### Prerequisites

#### Windows

1. Install the [Microsoft C++ Build Tools](https://visualstudio.microsoft.com/visual-cpp-build-tools/)

   In the installer, select the "Desktop development with C++" workload with these components:
    - MSVC Build Tools for x64/x86
    - Windows 11 SDK
    - C++ CMake tools for Windows

2. Set up [vcpkg](https://learn.microsoft.com/en-gb/vcpkg/get_started/get-started?pivots=shell-powershell) (with `VCPKG_ROOT` environment variable).

3. Start the VS PowerShell, or start it up. To start it for the default VS installation location:

   ```sh
   ./dev.ps1
   ```

### Building

1. Configure with CMake

   ```sh
   cmake --preset default
   ```

2. Build

   ```sh
   cmake --build build
   ```

3. Run a demo

   ```sh
   cd build/Debug
   ./basic-rendering.exe
   ```
