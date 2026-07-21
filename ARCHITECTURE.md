# Hitachi Spoofer - Architecture Documentation

## Overview

Hitachi Spoofer v4.0.0 is a professional Hardware Identifier (HWID) spoofing tool with a modern ImGui-based user interface. This document provides a comprehensive overview of the system architecture, design patterns, and development guidelines.

## Project Vision

**Goal**: Create a modular, maintainable, and extensible HWID spoofing solution with clean separation of concerns.

**Core Principles**:
- 🏗️ **Modularity** - Independent, replaceable components
- 🔒 **Security** - Privilege validation and safe operations
- 📊 **Observability** - Comprehensive logging and state tracking
- 🎨 **User Experience** - Intuitive UI with real-time feedback
- 🧪 **Testability** - Loosely coupled code for easy testing

---

## High-Level Architecture

```
┌─────────────────────────────────────────────────────┐
│              User Interface Layer (ImGui)            │
│  ┌──────────┬──────────┬──────────┬──────────────┐  │
│  │Spoof Tab │Clean Tab │Settings  │Recovery Dlg  │  │
│  └──────────┴──────────┴──────────┴──────────────┘  │
└─────────────────────────────────────────────────────┘
                        ↓
┌─────────────────────────────────────────────────────┐
│          Application State Management               │
│  ┌────────────────────────────────────────────────┐ │
│  │  AppStateManager (Singleton Pattern)           │ │
│  │  - Operation Progress Tracking                 │ │
│  │  - Configuration Management                    │ │
│  └────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────┘
                        ↓
┌─────────────────────────────────────────────────────┐
│             Core Business Logic Layer               │
│  ┌──────────────┬──────────────┬─────────────────┐ │
│  │ HWID Module  │ Spoofing     │ Security Module │ │
│  │ - Generator  │ - Engine     │ - Priv Check    │ │
│  │ - Registry   │ - Recovery   │ - Validation    │ │
│  └──────────────┴──────────────┴─────────────────┘ │
└─────────────────────────────────────────────────────┘
                        ↓
┌─────────────────────────────────────────────────────┐
│             Driver & System Layer                   │
│  ┌────────────────────────────────────────────────┐ │
│  │  Driver Interface & Communication              │ │
│  │  - Registry Access                             │ │
│  │  - WMI Queries                                 │ │
│  │  - Windows API Calls                           │ │
│  └────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────┘
                        ↓
┌─────────────────────────────────────────────────────┐
│             Support & Utilities Layer               │
│  ┌──────────┬──────────┬──────────┬──────────────┐ │
│  │ Logger   │ Config   │ Backup   │ Helpers      │ │
│  └──────────┴──────────┴──────────┴──────────────┘ │
└─────────────────────────────────────────────────────┘
```

---

## Directory Structure

```
Hitachi-Spoofer/
│
├── src/                                 # Source code
│   ├── core/                           # Core business logic
│   │   ├── hwid/
│   │   │   ├── generator.h             # HWID generation interface
│   │   │   ├── generator.cpp           # HWID implementation
│   │   │   ├── hwid_data.h             # Data structures
│   │   │   └── hwid_registry.h         # Registry operations
│   │   │
│   │   ├── spoofing/
│   │   │   ├── engine.h                # Spoofing engine interface
│   │   │   ├── engine.cpp              # Spoofing implementation
│   │   │   ├── operation.h             # Operation abstractions
│   │   │   └── recovery.h              # Recovery logic
│   │   │
│   │   └── security/
│   │       ├── privilege_check.h       # Admin privilege validation
│   │       └── privilege_check.cpp
│   │
│   ├── driver/                         # Driver communication
│   │   ├── interface.h                 # Driver interface
│   │   ├── interface.cpp               # Implementation
│   │   └── communication.h             # Communication protocol
│   │
│   ├── ui/                             # User interface layer
│   │   ├── imgui/
│   │   │   ├── main_window.h           # Main window
│   │   │   ├── main_window.cpp
│   │   │   ├── tabs/
│   │   │   │   ├── spoof_tab.h         # Spoofing interface
│   │   │   │   ├── spoof_tab.cpp
│   │   │   │   ├── clean_tab.h         # Deep clean interface
│   │   │   │   ├── clean_tab.cpp
│   │   │   │   ├── settings_tab.h      # Settings interface
│   │   │   │   └── settings_tab.cpp
│   │   │   └── dialogs/
│   │   │       ├── backup_dialog.h
│   │   │       ├── recovery_dialog.h
│   │   │       └── profile_dialog.h
│   │   ├── theme.h                     # UI themes
│   │   └── styles.cpp                  # Style definitions
│   │
│   ├── utils/                          # Utility modules
│   │   ├── logger/
│   │   │   ├── logger.h                # Logging system
│   │   │   └── logger.cpp
│   │   ├── config/
│   │   │   ├── config_manager.h        # Configuration management
│   │   │   └── config.json             # Default config
│   │   ├── backup/
│   │   │   └── backup_manager.h        # Backup management
│   │   └── helpers.h                   # Helper functions
│   │
│   ├── app/                            # Application layer
│   │   ├── application.h               # Main application class
│   │   ├── application.cpp
│   │   └── state_manager.h             # State management
│   │
│   └── main.cpp                        # Entry point
│
├── include/                            # Public API headers
│   └── public headers (exposed API)
│
├── tests/                              # Test suite
│   ├── unit/
│   │   ├── test_hwid_generator.cpp
│   │   ├── test_spoofing_engine.cpp
│   │   └── test_logger.cpp
│   └── integration/
│       └── test_integration.cpp
│
├── docs/                               # Documentation
│   ├── ARCHITECTURE.md                 # This file
│   ├── API_DESIGN.md                   # API design guide
│   ├── BUILD_GUIDE.md                  # Build instructions
│   ├── DEVELOPMENT.md                  # Development guide
│   └── SECURITY.md                     # Security considerations
│
├── scripts/                            # Build & utility scripts
│   ├── build.bat                       # Windows build script
│   ├── format.py                       # Code formatting
│   └── test.bat                        # Test runner
│
├── CMakeLists.txt                      # Build configuration
├── .clang-format                       # Code style
├── .gitignore
├── README.md
└── LICENSE
```

---

## Design Patterns Used

### 1. **Singleton Pattern**
```cpp
// AppStateManager - centralized state management
class AppStateManager {
public:
    static AppStateManager& Get() {
        static AppStateManager instance;
        return instance;
    }
    // ...
private:
    AppStateManager() = default;
    AppStateManager(const AppStateManager&) = delete;
};
```

### 2. **Strategy Pattern**
```cpp
// Different spoofing strategies for different hardware identifiers
class SpoofingStrategy {
public:
    virtual ~SpoofingStrategy() = default;
    virtual bool Execute(const std::string& value) = 0;
};

class MacAddressStrategy : public SpoofingStrategy { /* ... */ };
class GuidStrategy : public SpoofingStrategy { /* ... */ };
```

### 3. **Observer Pattern**
```cpp
// Progress callbacks for async operations
using ProgressCallback = std::function<void(float, const std::string&)>;

class SpoofingEngine {
public:
    bool ApplySpoofing(const std::string& hwid, ProgressCallback callback);
};
```

### 4. **Factory Pattern**
```cpp
// Create appropriate strategy based on HWID type
class StrategyFactory {
public:
    static std::unique_ptr<SpoofingStrategy> Create(HWIDType type);
};
```

### 5. **RAII Pattern**
```cpp
// Resource management for file handles, registry keys, etc.
class RegistryKey {
public:
    RegistryKey(const std::string& path);
    ~RegistryKey(); // Cleanup resources
private:
    HKEY handle_;
};
```

---

## Module Responsibilities

### **HWID Module** (`core/hwid/`)
- **Responsibility**: Generate and manage hardware identifiers
- **Key Classes**:
  - `HWIDGenerator` - Generate random HWIDs
  - `HWIDRegistry` - Read/write to Windows registry
  - `HWIDData` - Data structure for HWID info
- **Dependencies**: Windows API

### **Spoofing Module** (`core/spoofing/`)
- **Responsibility**: Execute spoofing operations
- **Key Classes**:
  - `SpoofingEngine` - Main spoofing logic
  - `RecoveryManager` - Handle recovery operations
  - `OperationContext` - Track ongoing operations
- **Dependencies**: HWID Module, Driver Interface, Logger

### **Security Module** (`core/security/`)
- **Responsibility**: Validate privileges and permissions
- **Key Classes**:
  - `PrivilegeChecker` - Check admin privileges
  - `ValidationManager` - Validate operations
- **Dependencies**: Windows API

### **Driver Module** (`driver/`)
- **Responsibility**: Communicate with kernel driver
- **Key Classes**:
  - `DriverInterface` - Driver communication
  - `Communication` - Protocol handling
- **Dependencies**: Windows API

### **UI Module** (`ui/`)
- **Responsibility**: Render user interface
- **Key Classes**:
  - `MainWindow` - Main window management
  - `SpoofTab`, `CleanTab`, `SettingsTab` - Tab implementations
  - `BackupDialog`, `RecoveryDialog` - Modal dialogs
- **Dependencies**: ImGui, App State Manager

### **Utils Module** (`utils/`)
- **Responsibility**: Provide utility functions
- **Sub-modules**:
  - **Logger**: Logging system
  - **Config**: Configuration management
  - **Backup**: Backup/restore operations
  - **Helpers**: Common helper functions

### **App Module** (`app/`)
- **Responsibility**: Application orchestration
- **Key Classes**:
  - `Application` - Main application orchestrator
  - `AppStateManager` - Centralized state management
- **Dependencies**: All other modules

---

## Data Flow

### Spoofing Operation Flow

```
1. User clicks "Start Spoof" in UI
   ↓
2. SpoofTab::OnApplyClicked()
   ↓
3. AppStateManager::SetOperationInProgress(true)
   ↓
4. SpoofingEngine::ApplySpoofing(hwid, progressCallback)
   ↓
5. For each hardware component:
   a. Generate new HWID (HWIDGenerator)
   b. Validate operation (PrivilegeChecker)
   c. Apply to registry via driver (DriverInterface)
   d. Log progress (Logger)
   e. Update UI callback (ProgressCallback)
   ↓
6. Create backup of original HWID (BackupManager)
   ↓
7. Update AppStateManager::SetSpoofStatus(true)
   ↓
8. UI reflects completion
```

### Recovery Flow

```
1. User clicks "Recover Original"
   ↓
2. RecoveryDialog opens (if needed)
   ↓
3. RecoveryManager::RestoreFromBackup(backupName)
   ↓
4. Validate restore operation
   ↓
5. Apply original values to registry
   ↓
6. Update AppStateManager::SetSpoofStatus(false)
   ↓
7. Log recovery operation
```

---

## State Management

Centralized state through `AppStateManager`:

```cpp
struct AppState {
    // Operation states
    bool is_spoofed = false;
    bool is_admin = false;
    
    // Progress tracking
    struct OperationProgress {
        bool is_running = false;
        float progress = 0.0f;
        std::string current_task;
        std::string status_message;
    };
    
    OperationProgress spoof_progress;
    OperationProgress recovery_progress;
    OperationProgress clean_progress;
    
    // Configuration
    std::vector<std::string> profiles;
    std::string active_profile;
    
    // UI state
    int active_tab = 0;
    bool show_settings = false;
    bool show_backup_dialog = false;
};
```

---

## Error Handling

### Exception Hierarchy

```cpp
std::exception
  ↓
UchihaException (Base custom exception)
  ├── PrivilegeException
  ├── RegistryException
  ├── DriverException
  ├── ConfigException
  └── BackupException
```

### Error Handling Strategy

```cpp
try {
    // Operation
    spoofing_engine.ApplySpoofing(hwid, callback);
} 
catch (const PrivilegeException& e) {
    logger.Error("Insufficient privileges: " + e.what());
    ui_manager.ShowError("Admin privileges required");
} 
catch (const RegistryException& e) {
    logger.Error("Registry operation failed: " + e.what());
    ui_manager.ShowError("System operation failed");
} 
catch (const std::exception& e) {
    logger.Critical("Unexpected error: " + e.what());
    ui_manager.ShowError("An unexpected error occurred");
}
```

---

## Thread Safety

### Async Operations

- Spoofing operations run on background thread
- Progress updates marshalled to main thread
- State mutations protected by locks

```cpp
class SpoofingEngine {
private:
    std::mutex operation_mutex_;
    std::thread operation_thread_;
    std::atomic<bool> should_cancel_{false};
};
```

---

## Configuration Management

### Config File Structure

```json
{
  "version": "4.0.0",
  "logging": {
    "level": "info",
    "file_path": "logs/spoofer.log"
  },
  "backup": {
    "enabled": true,
    "auto_backup": true,
    "backup_dir": "backups/"
  },
  "ui": {
    "theme": "dark",
    "remember_last_tab": true
  },
  "profiles": [
    {
      "name": "Profile 1",
      "spoof_mac": true,
      "spoof_guid": true
    }
  ]
}
```

---

## Testing Strategy

### Unit Tests
- HWID Generator
- Spoofing Engine
- Logger
- Config Manager

### Integration Tests
- Full spoofing workflow
- Recovery workflow
- State management

### Manual Testing
- UI functionality
- Error handling
- Backup/recovery

---

## Security Considerations

1. **Admin Privilege Validation**: Always verify admin rights before operations
2. **Registry Access**: Use safe registry operations with error handling
3. **Backup Strategy**: Always create backups before modifications
4. **Logging**: Log all operations for audit trail
5. **Error Messages**: Avoid exposing sensitive system information in errors

---

## Future Enhancements

- [ ] Multi-language support
- [ ] Portable mode (no installation)
- [ ] Command-line interface
- [ ] Scheduled operations
- [ ] Cloud backup integration
- [ ] Advanced analytics dashboard
- [ ] Plugin system for extensibility

---

## Contributing

Refer to `DEVELOPMENT.md` for development guidelines and `BUILD_GUIDE.md` for build instructions.

---

**Document Version**: 1.0.0  
**Last Updated**: 2026-07-21  
**Maintainer**: go04k316-cell
