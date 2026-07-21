#pragma once

#include <string>
#include <windows.h>

namespace UchihaSpoofer {
namespace Core {

/**
 * @brief Custom exception for registry operations
 */
class RegistryException : public std::exception {
private:
    std::string message_;
    
public:
    RegistryException(const std::string& msg) : message_(msg) {}
    virtual const char* what() const noexcept override {
        return message_.c_str();
    }
};

/**
 * @brief Registry Manager - Handles all registry operations
 * 
 * This class provides safe access to Windows registry for reading and writing
 * HWID-related values. All operations are wrapped with error handling.
 */
class RegistryManager {
public:
    /**
     * @brief Constructor
     */
    RegistryManager() = default;
    
    /**
     * @brief Destructor - Closes any open registry keys
     */
    ~RegistryManager();
    
    /**
     * @brief Read string value from registry
     * 
     * @param hkey Root key (HKEY_LOCAL_MACHINE, etc.)
     * @param subkey Subkey path
     * @param value_name Value name
     * @return String value
     * @throws RegistryException if operation fails
     */
    std::string ReadStringValue(
        HKEY hkey,
        const std::string& subkey,
        const std::string& value_name
    );
    
    /**
     * @brief Write string value to registry
     * 
     * @param hkey Root key
     * @param subkey Subkey path
     * @param value_name Value name
     * @param value Value to write
     * @throws RegistryException if operation fails
     */
    void WriteStringValue(
        HKEY hkey,
        const std::string& subkey,
        const std::string& value_name,
        const std::string& value
    );
    
    /**
     * @brief Read DWORD value from registry
     * 
     * @param hkey Root key
     * @param subkey Subkey path
     * @param value_name Value name
     * @return DWORD value
     * @throws RegistryException if operation fails
     */
    DWORD ReadDWordValue(
        HKEY hkey,
        const std::string& subkey,
        const std::string& value_name
    );
    
    /**
     * @brief Write DWORD value to registry
     * 
     * @param hkey Root key
     * @param subkey Subkey path
     * @param value_name Value name
     * @param value DWORD value to write
     * @throws RegistryException if operation fails
     */
    void WriteDWordValue(
        HKEY hkey,
        const std::string& subkey,
        const std::string& value_name,
        DWORD value
    );
    
    /**
     * @brief Backup registry value before modification
     * 
     * @param hkey Root key
     * @param subkey Subkey path
     * @param value_name Value name
     * @param backup_name Backup identifier
     * @return True if backup successful
     */
    bool BackupValue(
        HKEY hkey,
        const std::string& subkey,
        const std::string& value_name,
        const std::string& backup_name
    );
    
    /**
     * @brief Restore registry value from backup
     * 
     * @param backup_name Backup identifier
     * @return True if restore successful
     */
    bool RestoreValue(const std::string& backup_name);
    
private:
    /**
     * @brief Convert LONG error code to error message
     * 
     * @param error_code Windows registry error code
     * @return Error message string
     */
    static std::string GetErrorMessage(LONG error_code);
    
    /**
     * @brief Get system error string
     * 
     * @param error_code Windows error code
     * @return Error message string
     */
    static std::string GetSystemErrorMessage(DWORD error_code);
    
    // Open registry key handles (kept for cleanup)
    std::vector<HKEY> open_keys_;
};

} // namespace Core
} // namespace UchihaSpoofer
