# API Design - Hitachi Spoofer

## Core Module APIs

### HWID Generation API

```cpp
namespace UchihaSpoofer::Core {

/**
 * @brief Structure containing hardware identifier data
 */
struct HWIDData {
    std::string mac_address;      ///< MAC address (XX:XX:XX:XX:XX:XX)
    std::string guid;              ///< GUID ({XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX})
    std::string serial_number;     ///< Serial number
    std::string product_id;        ///< Windows Product ID
    std::string cpu_serial;        ///< CPU serial number
    std::string bios_serial;       ///< BIOS serial number
    std::string volume_serial_c;   ///< C: drive serial
    std::string volume_serial_d;   ///< D: drive serial
    std::chrono::system_clock::time_point generated_at;
};

/**
 * @brief HWID Generator - Generates random hardware identifiers
 * @note Thread-safe
 */
class HWIDGenerator {
public:
    /**
     * @brief Generate random MAC address
     * @return MAC address in format XX:XX:XX:XX:XX:XX
     * @throws std::runtime_error if generation fails
     */
    static std::string GenerateMAC();
    
    /**
     * @brief Generate random GUID
     * @return GUID in format {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
     */
    static std::string GenerateGUID();
    
    /**
     * @brief Generate random serial number
     * @param format Optional format string
     * @return Generated serial number
     */
    static std::string GenerateSerialNumber(const std::string& format = "");
    
    /**
     * @brief Generate random Product ID (Windows)
     * @return Windows Product ID
     */
    static std::string GenerateProductID();
    
    /**
     * @brief Generate random CPU serial number
     * @return CPU serial number
     */
    static std::string GenerateCPUSerial();
    
    /**
     * @brief Generate random BIOS serial number
     * @return BIOS serial number
     */
    static std::string GenerateBIOSSerial();
    
    /**
     * @brief Generate random Volume Serial Number
     * @param drive_letter Drive letter (C, D, etc.)
     * @return Volume serial number
     */
    static std::string GenerateVolumeSerial(char drive_letter = 'C');
    
    /**
     * @brief Generate complete HWID data
     * @return Complete HWIDData structure
     * @throws std::runtime_error if any generation fails
     */
    static HWIDData GenerateComplete();
    
private:
    static thread_local std::mt19937 random_engine_;
    static std::string BytesToHex(const unsigned char* data, size_t size);
    static unsigned char RandomByte();
    static uint32_t RandomDword();
};

} // namespace UchihaSpoofer::Core
```

### Spoofing Engine API

```cpp
namespace UchihaSpoofer::Core {

/**
 * @brief Callback for progress updates
 * @param progress Progress percentage (0.0 to 1.0)
 * @param message Status message
 */
using ProgressCallback = std::function<void(float, const std::string&)>;

/**
 * @brief Spoofing operation result
 */
struct SpoofingResult {
    bool success = false;
    std::string message;
    std::string error_code;
    std::vector<std::string> failed_operations;
};

/**
 * @brief Spoofing Engine - Applies HWID spoofing
 * @note Thread-safe for concurrent status queries
 */
class SpoofingEngine {
public:
    /**
     * @brief Apply HWID spoofing
     * @param hwid Hardware ID data to apply
     * @param callback Progress callback (optional)
     * @return Success status
     * @throws PrivilegeException if admin rights insufficient
     * @throws RegistryException if registry operations fail
     */
    SpoofingResult ApplySpoofing(
        const HWIDData& hwid,
        ProgressCallback callback = nullptr
    );
    
    /**
     * @brief Recover original HWID from backup
     * @param backup_name Name of backup to restore
     * @param callback Progress callback (optional)
     * @return Success status
     * @throws std::runtime_error if backup not found
     */
    SpoofingResult RecoverOriginal(
        const std::string& backup_name,
        ProgressCallback callback = nullptr
    );
    
    /**
     * @brief Check if system is currently spoofed
     * @return True if spoofed, false otherwise
     */
    bool IsSpoofed() const;
    
    /**
     * @brief Cancel ongoing operation
     * @note Safe to call from any thread
     */
    void Cancel();
    
    /**
     * @brief Get current operation status
     * @return Current operation status
     */
    struct OperationStatus {
        bool is_running = false;
        float progress = 0.0f;
        std::string current_task;
    } GetStatus() const;
    
private:
    std::atomic<bool> is_spoofed_{false};
    std::atomic<bool> should_cancel_{false};
    mutable std::mutex status_mutex_;
    OperationStatus current_status_;
};

} // namespace UchihaSpoofer::Core
```

### Security/Privilege API

```cpp
namespace UchihaSpoofer::Core {

/**
 * @brief Privilege Checker - Validates system privileges
 */
class PrivilegeChecker {
public:
    /**
     * @brief Check if process has admin privileges
     * @return True if admin, false otherwise
     */
    static bool IsAdmin();
    
    /**
     * @brief Verify admin rights before operation
     * @throws PrivilegeException if not admin
     */
    static void RequireAdmin();
    
    /**
     * @brief Get process privilege level
     * @return Privilege level enum
     */
    enum class PrivilegeLevel {
        Unknown,
        User,
        Power,
        Administrator
    };
    
    static PrivilegeLevel GetPrivilegeLevel();
};

} // namespace UchihaSpoofer::Core
```

---

## Driver Module APIs

```cpp
namespace UchihaSpoofer::Driver {

/**
 * @brief Driver Interface - Communicates with kernel driver
 */
class DriverInterface {
public:
    /**
     * @brief Initialize driver communication
     * @param driver_name Name of driver device
     * @return True if initialization successful
     */
    bool Initialize(const std::string& driver_name = "HitachiSpooferDriver");
    
    /**
     * @brief Send command to driver
     * @param command Command code
     * @param data Input data
     * @param data_size Size of input data
     * @param output Output buffer
     * @param output_size Output buffer size
     * @return Bytes written to output
     * @throws DriverException on communication failure
     */
    size_t SendCommand(
        uint32_t command,
        const void* data,
        size_t data_size,
        void* output,
        size_t output_size
    );
    
    /**
     * @brief Check if driver is loaded
     * @return True if driver available
     */
    bool IsDriverLoaded() const;
    
    /**
     * @brief Cleanup driver communication
     */
    void Cleanup();
    
private:
    HANDLE driver_handle_ = INVALID_HANDLE_VALUE;
};

} // namespace UchihaSpoofer::Driver
```

---

## Utility Module APIs

### Logger API

```cpp
namespace UchihaSpoofer::Utils {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

/**
 * @brief Logger - Centralized logging system
 * @note Singleton pattern
 */
class Logger {
public:
    /**
     * @brief Get logger instance
     * @return Reference to singleton logger
     */
    static Logger& GetInstance();
    
    /**
     * @brief Log debug message
     * @param message Debug message
     */
    void Debug(const std::string& message);
    
    /**
     * @brief Log info message
     * @param message Info message
     */
    void Info(const std::string& message);
    
    /**
     * @brief Log warning message
     * @param message Warning message
     */
    void Warning(const std::string& message);
    
    /**
     * @brief Log error message
     * @param message Error message
     */
    void Error(const std::string& message);
    
    /**
     * @brief Log critical message
     * @param message Critical message
     */
    void Critical(const std::string& message);
    
    /**
     * @brief Set minimum log level
     * @param level Minimum log level to output
     */
    void SetMinLogLevel(LogLevel level);
    
    /**
     * @brief Initialize file logging
     * @param file_path Path to log file
     * @param append True to append, false to overwrite
     * @return True if successful
     */
    bool InitializeFileLogging(const std::string& file_path, bool append = true);
    
    /**
     * @brief Save logs to file
     * @param file_path Destination file path
     */
    void SaveToFile(const std::string& file_path);
    
private:
    Logger() = default;
    Logger(const Logger&) = delete;
    
    LogLevel min_level_ = LogLevel::Info;
    std::vector<std::string> log_buffer_;
    std::ofstream log_file_;
    mutable std::mutex log_mutex_;
};

} // namespace UchihaSpoofer::Utils
```

### Configuration Manager API

```cpp
namespace UchihaSpoofer::Utils {

/**
 * @brief Configuration Manager - Manages application configuration
 * @note Singleton pattern
 */
class ConfigManager {
public:
    /**
     * @brief Get configuration manager instance
     */
    static ConfigManager& GetInstance();
    
    /**
     * @brief Load configuration from file
     * @param config_path Path to configuration file
     * @return True if successful
     */
    bool LoadConfig(const std::string& config_path);
    
    /**
     * @brief Save configuration to file
     * @param config_path Path to save configuration
     * @return True if successful
     */
    bool SaveConfig(const std::string& config_path);
    
    /**
     * @brief Get configuration value
     * @param key Configuration key (dot-separated)
     * @param default_value Default if not found
     * @return Configuration value
     */
    template<typename T>
    T GetValue(const std::string& key, const T& default_value) const;
    
    /**
     * @brief Set configuration value
     * @param key Configuration key (dot-separated)
     * @param value New value
     */
    template<typename T>
    void SetValue(const std::string& key, const T& value);
    
private:
    ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    
    nlohmann::json config_;
    std::string config_path_;
};

} // namespace UchihaSpoofer::Utils
```

### Backup Manager API

```cpp
namespace UchihaSpoofer::Utils {

/**
 * @brief Backup information
 */
struct BackupInfo {
    std::string name;
    std::chrono::system_clock::time_point created_at;
    size_t size_bytes = 0;
    std::string description;
};

/**
 * @brief Backup Manager - Manages backup/restore operations
 */
class BackupManager {
public:
    /**
     * @brief Create backup of current HWID
     * @param backup_name Name for backup
     * @param description Optional description
     * @return True if successful
     */
    bool CreateBackup(
        const std::string& backup_name,
        const std::string& description = ""
    );
    
    /**
     * @brief Restore HWID from backup
     * @param backup_name Name of backup to restore
     * @return True if successful
     * @throws std::runtime_error if backup not found
     */
    bool RestoreBackup(const std::string& backup_name);
    
    /**
     * @brief Delete backup
     * @param backup_name Name of backup to delete
     * @return True if successful
     */
    bool DeleteBackup(const std::string& backup_name);
    
    /**
     * @brief List all available backups
     * @return Vector of backup information
     */
    std::vector<BackupInfo> ListBackups() const;
    
    /**
     * @brief Get backup info
     * @param backup_name Name of backup
     * @return Backup information (nullptr if not found)
     */
    std::unique_ptr<BackupInfo> GetBackupInfo(const std::string& backup_name) const;
    
private:
    std::string backup_directory_;
};

} // namespace UchihaSpoofer::Utils
```

---

## Application State API

```cpp
namespace UchihaSpoofer::App {

/**
 * @brief Application state enumeration
 */
enum class AppState {
    Initializing,
    Ready,
    OperationInProgress,
    Error,
    Shutting Down
};

/**
 * @brief Operation progress information
 */
struct ProgressInfo {
    float percentage = 0.0f;      ///< Progress 0.0 to 1.0
    std::string message;           ///< Progress message
    std::string task_name;         ///< Current task name
    std::chrono::milliseconds elapsed;
};

/**
 * @brief Application State Manager - Centralized state management
 * @note Singleton pattern, thread-safe
 */
class AppStateManager {
public:
    /**
     * @brief Get state manager instance
     */
    static AppStateManager& Get();
    
    /**
     * @brief Get current application state
     */
    AppState GetState() const;
    
    /**
     * @brief Set application state
     */
    void SetState(AppState state);
    
    /**
     * @brief Check if currently spoofed
     */
    bool IsSpoofed() const;
    
    /**
     * @brief Set spoof status
     */
    void SetSpoofStatus(bool spoofed);
    
    /**
     * @brief Update operation progress
     */
    void UpdateProgress(float percent, const std::string& message);
    
    /**
     * @brief Get current progress
     */
    ProgressInfo GetProgress() const;
    
    /**
     * @brief Check if admin privileges available
     */
    bool IsAdmin() const;
    
    /**
     * @brief Register state change callback
     */
    using StateChangeCallback = std::function<void(AppState)>;
    void OnStateChanged(StateChangeCallback callback);
    
private:
    AppStateManager() = default;
    AppStateManager(const AppStateManager&) = delete;
    
    AppState current_state_ = AppState::Initializing;
    bool is_spoofed_ = false;
    bool is_admin_ = false;
    ProgressInfo progress_;
    mutable std::mutex state_mutex_;
    std::vector<StateChangeCallback> callbacks_;
};

} // namespace UchihaSpoofer::App
```

---

## Usage Examples

### Example 1: Basic HWID Spoofing

```cpp
using namespace UchihaSpoofer;

// Generate random HWID
Core::HWIDData hwid = Core::HWIDGenerator::GenerateComplete();

// Apply spoofing with progress tracking
Core::SpoofingEngine engine;
auto result = engine.ApplySpoofing(hwid, 
    [](float progress, const std::string& msg) {
        std::cout << "Progress: " << (int)(progress * 100) << "%" << std::endl;
        std::cout << "Message: " << msg << std::endl;
    }
);

if (result.success) {
    std::cout << "Spoofing completed!" << std::endl;
} else {
    std::cout << "Error: " << result.message << std::endl;
}
```

### Example 2: Configuration Management

```cpp
using namespace UchihaSpoofer;

// Load configuration
auto& config = Utils::ConfigManager::GetInstance();
config.LoadConfig("config.json");

// Get values
bool enable_logging = config.GetValue<bool>("logging.enabled", true);
std::string log_file = config.GetValue<std::string>("logging.file", "app.log");

// Set values
config.SetValue<bool>("logging.enabled", false);
config.SaveConfig("config.json");
```

### Example 3: Backup and Recovery

```cpp
using namespace UchihaSpoofer;

// Create backup before spoofing
auto& backup_mgr = Utils::BackupManager();
backup_mgr.CreateBackup("pre-spoof-backup", "Before spoofing operation");

// Perform spoofing...

// Later, restore from backup
if (!backup_mgr.RestoreBackup("pre-spoof-backup")) {
    std::cerr << "Backup restore failed!" << std::endl;
}
```

---

**Last Updated**: 2026-07-21
