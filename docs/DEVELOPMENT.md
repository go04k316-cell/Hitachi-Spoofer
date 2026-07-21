# Development Guide - Hitachi Spoofer

## Getting Started

This guide provides comprehensive instructions for developing features and maintaining the Hitachi Spoofer codebase.

### Prerequisites

- Windows 10 or later
- Visual Studio 2019 or later
- CMake 3.16 or later
- Git
- Administrator privileges (for testing)

### Initial Setup

```bash
# 1. Clone the repository
git clone https://github.com/go04k316-cell/Hitachi-Spoofer.git
cd Hitachi-Spoofer

# 2. Create build directory
mkdir build
cd build

# 3. Generate project files
cmake .. -G "Visual Studio 17 2022" -A x64

# 4. Build project
cmake --build . --config Release
```

---

## Code Style Guidelines

### Naming Conventions

**Classes and Types**:
```cpp
class HWIDGenerator { };      // PascalCase
struct OperationProgress { }; // PascalCase
using Callback = std::function<...>; // PascalCase for type aliases
```

**Variables and Functions**:
```cpp
std::string hwid_value;       // snake_case for variables
bool is_admin = false;        // is_/has_ prefix for booleans

void ApplySpoofing();         // PascalCase for public methods
void apply_spoofing_internal(); // snake_case for private methods
```

**Constants and Macros**:
```cpp
const int MAX_RETRIES = 3;           // UPPER_CASE
#define WIN32_LEAN_AND_MEAN          // UPPER_CASE for macros
const std::string CONFIG_FILE_NAME = "config.json";
```

**Member Variables**:
```cpp
class MyClass {
private:
    std::string member_var_;    // Trailing underscore for private members
    mutable std::mutex lock_;   // Trailing underscore
};
```

### Code Formatting

All code must follow `.clang-format` configuration. Run formatter before committing:

```bash
# Format all files
python scripts/format.py

# Or format specific file
clang-format -i src/core/hwid/generator.cpp
```

### Comment Style

**File Header**:
```cpp
/**
 * @file hwid_generator.h
 * @brief HWID generation module
 * @author your-name
 * @date 2026-07-21
 */
```

**Function Documentation**:
```cpp
/**
 * @brief Generate random MAC address
 * @return MAC address in format XX:XX:XX:XX:XX:XX
 * @throws std::runtime_error if generation fails
 */
static std::string GenerateMAC();
```

**Inline Comments**:
```cpp
// Validate admin privileges before operation
if (!PrivilegeChecker::IsAdmin()) {
    throw PrivilegeException("Admin rights required");
}
```

---

## Project Structure Guidelines

### Module Organization

**Each module should have**:

```
src/core/hwid/
├── generator.h           # Public interface
├── generator.cpp         # Implementation
├── hwid_data.h          # Data structures
└── hwid_registry.h      # Related functionality
```

**Header File Organization**:

```cpp
#pragma once

// Standard library includes
#include <string>
#include <vector>

// Third-party includes
#include <windows.h>

// Project includes
#include "core/exception.h"
#include "utils/logger.h"

namespace UchihaSpoofer {
namespace Core {

// Forward declarations
class HWIDRegistry;

/**
 * @brief HWID Generator class
 */
class HWIDGenerator {
public:
    // Public methods
    static std::string GenerateMAC();
    
private:
    // Private methods
    static std::string BytesToHex(const unsigned char* data, size_t size);
    
    // Private members
    static thread_local std::mt19937 rng_;
};

} // namespace Core
} // namespace UchihaSpoofer
```

---

## Adding New Features

### Step 1: Create Issue

```markdown
Title: [FEATURE] Add profile management system

Description:
- Allow users to save multiple spoofing configurations
- Each profile contains specific HWID values
- Support import/export of profiles

Acceptance Criteria:
- [ ] Can create new profile
- [ ] Can save/load profiles
- [ ] Can delete profiles
- [ ] UI shows profile list
```

### Step 2: Create Feature Branch

```bash
git checkout -b feature/profile-management
```

### Step 3: Implement Feature

**Example: Adding profile management**

```cpp
// include/core/profile.h
#pragma once

namespace UchihaSpoofer {
namespace Core {

struct Profile {
    std::string name;
    HWIDData hwid;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point last_used;
};

class ProfileManager {
public:
    bool CreateProfile(const std::string& name, const HWIDData& hwid);
    bool LoadProfile(const std::string& name, HWIDData& hwid);
    bool DeleteProfile(const std::string& name);
    std::vector<Profile> ListProfiles() const;
    
private:
    std::string profiles_dir_;
    std::vector<Profile> loaded_profiles_;
};

} // namespace Core
} // namespace UchihaSpoofer
```

### Step 4: Write Tests

```cpp
// tests/unit/test_profile_manager.cpp
#include <gtest/gtest.h>
#include "core/profile.h"

class ProfileManagerTest : public ::testing::Test {
protected:
    UchihaSpoofer::Core::ProfileManager pm_;
};

TEST_F(ProfileManagerTest, CreateProfile) {
    HWIDData hwid = /* ... */;
    EXPECT_TRUE(pm_.CreateProfile("test_profile", hwid));
}

TEST_F(ProfileManagerTest, LoadProfile) {
    HWIDData hwid, loaded;
    pm_.CreateProfile("test", hwid);
    EXPECT_TRUE(pm_.LoadProfile("test", loaded));
    EXPECT_EQ(hwid.mac_address, loaded.mac_address);
}
```

### Step 5: Submit Pull Request

```bash
git add .
git commit -m "feat: Add profile management system

- Implement ProfileManager class
- Add create, load, delete operations
- Add unit tests
- Update documentation

Closes #123"

git push origin feature/profile-management
```

---

## Git Workflow

### Branch Naming

```
feature/description        # New feature
fix/description            # Bug fix
refactor/description       # Code refactoring
docs/description           # Documentation
chore/description          # Maintenance
```

### Commit Messages

Follow Conventional Commits format:

```
type(scope): subject

body

footer
```

**Types**:
- `feat`: New feature
- `fix`: Bug fix
- `refactor`: Code refactoring
- `docs`: Documentation
- `test`: Test files
- `chore`: Build, deps, config
- `style`: Formatting

**Example**:

```
feat(core/hwid): add CPU serial number generation

Implement HWIDGenerator::GenerateCPUSerial() method that generates
random CPU serial numbers in valid format.

- Add GenerateCPUSerial() method
- Add unit tests
- Update HWID data structure

Closes #456
```

---

## Debugging

### Enable Debug Logging

```cpp
// In main.cpp
auto& logger = Utils::Logger::GetInstance();
logger.SetMinLogLevel(Utils::LogLevel::Debug);
logger.InitializeFileLogging("debug.log");
```

### Visual Studio Debugging

1. Set breakpoint (F9)
2. Run project (F5)
3. Step through code (F10 - step over, F11 - step into)
4. View locals/watch windows
5. Attach debugger to running process

### Common Debug Tasks

**Check registry values**:
```cpp
Utils::Logger::GetInstance().Debug(
    "Registry value: " + registry_manager.ReadValue("GUID")
);
```

**Trace operation flow**:
```cpp
logger.Info("Starting spoofing operation...");
logger.Debug("Generated HWID: " + hwid.mac_address);
logger.Info("Spoofing completed successfully");
```

---

## Building and Testing

### Build Configurations

```bash
# Debug build
cmake --build build --config Debug

# Release build
cmake --build build --config Release

# With specific generator
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
```

### Running Tests

```bash
# Run all tests
build/bin/tests.exe

# Run specific test
build/bin/tests.exe --gtest_filter=ProfileManagerTest.*

# Generate coverage report
cmake --build build --config Debug --target COVERAGE
```

### Using Batch Scripts

```bash
# Build (Windows)
scripts\build.bat

# Format code
python scripts\format.py

# Run tests
scripts\test.bat
```

---

## Performance Considerations

### Profiling

```cpp
// Measure operation duration
auto start = std::chrono::high_resolution_clock::now();
// Perform operation
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
logger.Info("Operation took: " + std::to_string(duration.count()) + "ms");
```

### Optimization Guidelines

1. **Avoid unnecessary allocations**
2. **Use move semantics** for large objects
3. **Profile before optimizing**
4. **Document performance assumptions**
5. **Consider memory usage** for long-running operations

---

## Common Issues

### Build Errors

**Issue**: `MSVC not found`

**Solution**: Ensure Visual Studio is installed and in PATH

```bash
# Set Visual Studio path
set PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\bin\Hostx64\x64;%PATH%
```

### Link Errors

**Issue**: `Unresolved external symbol`

**Solution**: Check that all libraries are linked in CMakeLists.txt

### Runtime Errors

**Issue**: `Access violation` when accessing registry

**Solution**: Ensure admin privileges and handle exceptions

```cpp
try {
    registry_manager.OpenKey(HKEY_LOCAL_MACHINE, "SYSTEM\\...");
} catch (const RegistryException& e) {
    logger.Error("Registry access denied: " + std::string(e.what()));
}
```

---

## Resources

- [ARCHITECTURE.md](./ARCHITECTURE.md) - System architecture
- [API_DESIGN.md](./API_DESIGN.md) - API design guidelines
- [BUILD_GUIDE.md](./BUILD_GUIDE.md) - Build instructions
- [Windows Registry](https://docs.microsoft.com/en-us/windows/win32/sysinfo/registry)
- [ImGui Documentation](https://github.com/ocornut/imgui)

---

**Last Updated**: 2026-07-21
