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
    - vcpkg package manager

2. Set the `VCPKG_ROOT` environment variable to where the tool is installed. Restart your terminal after setting this.

   ```powershell
   [Environment]::SetEnvironmentVariable("VCPKG_ROOT", "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\vcpkg", "User")
   ```

3. Start the VS PowerShell

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

3. Run

   ```sh
   ./build/Debug/2real.exe
   ```
