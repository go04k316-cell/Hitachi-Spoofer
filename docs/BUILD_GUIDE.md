# Build Guide - Hitachi Spoofer

## Quick Start

### Windows (Visual Studio)

```bash
# 1. Clone repository
git clone https://github.com/go04k316-cell/Hitachi-Spoofer.git
cd Hitachi-Spoofer

# 2. Create build directory
mkdir build
cd build

# 3. Generate Visual Studio project
cmake .. -G "Visual Studio 17 2022" -A x64

# 4. Build
cmake --build . --config Release

# 5. Output
# Binary: build/bin/Release/HitachiSpoofer.exe
```

---

## Prerequisites

### System Requirements

| Component | Version | Required |
|-----------|---------|----------|
| OS | Windows 10+ | ✅ |
| Processor | x64 | ✅ |
| RAM | 4GB | ✅ |
| Disk Space | 500MB | ✅ |
| Admin Privileges | Required | ✅ |

### Software Requirements

| Software | Version | Purpose |
|----------|---------|----------|
| Visual Studio | 2019+ | C++ compiler |
| CMake | 3.16+ | Build system |
| Git | 2.30+ | Version control |
| Python | 3.8+ | Scripts (optional) |

### Installation

**Visual Studio 2022**:
1. Download from https://visualstudio.microsoft.com/
2. Install with "Desktop development with C++" workload
3. Include MSVC compiler and Windows SDK

**CMake**:
1. Download from https://cmake.org/download/
2. Add to PATH: `C:\Program Files\CMake\bin`

**Git**:
1. Download from https://git-scm.com/
2. Install with default settings

---

## Build Configurations

### Debug Build

```bash
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
```

**Features**:
- Debug symbols included
- Optimizations disabled
- Full logging enabled
- Runtime checks enabled

### Release Build

```bash
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

**Features**:
- Optimized for performance
- Minimal debug information
- Reduced binary size
- Suitable for distribution

---

## Detailed Build Steps

### Step 1: Clone Repository

```bash
git clone https://github.com/go04k316-cell/Hitachi-Spoofer.git
cd Hitachi-Spoofer
```

### Step 2: Create Build Directory

```bash
mkdir build
cd build
```

### Step 3: Configure with CMake

**Visual Studio 2022 (x64)**:
```bash
cmake .. -G "Visual Studio 17 2022" -A x64
```

**Visual Studio 2022 (x86)**:
```bash
cmake .. -G "Visual Studio 17 2022" -A Win32
```

**Visual Studio 2019**:
```bash
cmake .. -G "Visual Studio 16 2019" -A x64
```

**Custom Configuration Options**:
```bash
cmake .. -G "Visual Studio 17 2022" -A x64 \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTS=ON \
  -DENABLE_LOGGING=ON
```

### Step 4: Build Project

**Build All**:
```bash
cmake --build . --config Release
```

**Build Specific Target**:
```bash
cmake --build . --config Release --target HitachiSpoofer
```

**Build with Multiple Threads**:
```bash
cmake --build . --config Release -- /MP4
```

**Verbose Output**:
```bash
cmake --build . --config Release --verbose
```

### Step 5: Verify Build

```bash
# Check output
dir bin\Release\HitachiSpoofer.exe

# Run executable
.\bin\Release\HitachiSpoofer.exe
```

---

## Using Build Scripts

### Windows Batch Script

**scripts/build.bat**:
```bash
scripts\build.bat Release
```

**Supported options**:
- `Debug` - Debug build
- `Release` - Release build
- `Clean` - Clean build

### Python Build Script

**scripts/format.py** - Format code:
```bash
python scripts\format.py
```

---

## CMakeLists.txt Reference

### Key Options

```cmake
# Minimum CMake version
cmake_minimum_required(VERSION 3.16)

# Project definition
project(HitachiSpoofer VERSION 4.0.0 LANGUAGES CXX C)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable testing
enable_testing()

# Add source files
add_executable(HitachiSpoofer ${ALL_SOURCES})

# Link libraries
target_link_libraries(HitachiSpoofer PRIVATE user32 advapi32 shell32)
```

---

## Troubleshooting

### CMake Configuration Issues

**Error**: `Visual Studio 17 2022 not found`

**Solution**:
```bash
# List available generators
cmake --help

# Use available generator
cmake .. -G "Visual Studio 16 2019" -A x64
```

**Error**: `CMake version too old`

**Solution**:
```bash
# Update CMake
choco upgrade cmake
# or
scoop update cmake
```

### Compiler Issues

**Error**: `MSVC compiler not found`

**Solution**:
```bash
# Set Visual Studio path
set PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\bin\Hostx64\x64;%PATH%
```

**Error**: `Missing header files`

**Solution**:
```bash
# Ensure Windows SDK is installed
# Visual Studio Installer → Modify → Check Windows 10/11 SDK
```

### Linker Issues

**Error**: `Unresolved external symbol`

**Solution**:
```cmake
# Add missing library to CMakeLists.txt
target_link_libraries(HitachiSpoofer PRIVATE
    user32      # Add this
    advapi32
    shell32
)
```

### Build Failures

**Error**: `Out of memory during build`

**Solution**:
```bash
# Reduce parallel jobs
cmake --build . --config Release -- /MP1
```

**Error**: `Incremental build fails`

**Solution**:
```bash
# Clean and rebuild
rmdir /s /q build
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

---

## Testing Build

### Build Tests

```bash
cmake .. -DBUILD_TESTS=ON
cmake --build . --config Release
```

### Run Tests

```bash
# All tests
bin\Release\tests.exe

# Specific test
bin\Release\tests.exe --gtest_filter=HWIDGeneratorTest.*

# With verbose output
bin\Release\tests.exe --gtest_filter=* --gtest_print_time=1
```

---

## Installation

### Local Installation

```bash
# Install to system
cmake --install . --prefix "C:\Program Files\HitachiSpoofer"
```

### Portable Build

```bash
# Create portable directory
mkdir portable
copy bin\Release\HitachiSpoofer.exe portable
copy config\*.json portable
# Distribute portable directory
```

---

## Distribution

### Create Release Package

```bash
# Build release version
cmake --build . --config Release

# Create package
mkdir HitachiSpoofer-4.0.0
copy bin\Release\HitachiSpoofer.exe HitachiSpoofer-4.0.0
copy README.md HitachiSpoofer-4.0.0
copy LICENSE HitachiSpoofer-4.0.0

# Compress
tar -czf HitachiSpoofer-4.0.0.tar.gz HitachiSpoofer-4.0.0
```

---

## Development Workflow

### Recommended Workflow

```bash
# 1. Clone and setup
git clone <repo>
cd Hitachi-Spoofer
mkdir build
cd build

# 2. Generate project (first time)
cmake .. -G "Visual Studio 17 2022" -A x64

# 3. Open in Visual Studio
start HitachiSpoofer.sln

# 4. Make changes and build in IDE (F7)

# 5. Or build from command line
cmake --build . --config Debug

# 6. Run executable
.\bin\Debug\HitachiSpoofer.exe
```

### CI/CD Considerations

For automated builds, ensure:
- CMake 3.16+ installed
- Visual Studio compiler available
- Admin privileges for tests
- Sufficient disk space (500MB+)

---

## Advanced Configuration

### Custom CMake Variables

```cmake
# In CMakeLists.txt or command line
cmake .. -DCUSTOM_VAR=value

# Examples
cmake .. -DENABLE_TESTS=ON -DENABLE_LOGGING=ON
```

### Profile-Guided Optimization

```bash
# Requires MSVC 2019+
cmake .. -DCMAKE_CXX_FLAGS="/GL /Gw"
```

---

**Last Updated**: 2026-07-21
