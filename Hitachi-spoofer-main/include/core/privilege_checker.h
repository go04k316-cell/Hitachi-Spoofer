#pragma once

#include <string>
#include <windows.h>

namespace UchihaSpoofer {
namespace Core {

/**
 * @brief Custom exception for privilege operations
 */
class PrivilegeException : public std::exception {
private:
    std::string message_;
    
public:
    PrivilegeException(const std::string& msg) : message_(msg) {}
    virtual const char* what() const noexcept override {
        return message_.c_str();
    }
};

/**
 * @brief Privilege Checker - Validates system privileges
 * 
 * This class provides static methods to check and verify administrator
 * privileges required for HWID spoofing operations.
 */
class PrivilegeChecker {
public:
    /**
     * @brief Check if current process has administrator privileges
     * 
     * @return True if running as administrator, false otherwise
     */
    static bool IsAdmin();
    
    /**
     * @brief Verify admin rights, throw exception if not
     * 
     * @throws PrivilegeException if not running as administrator
     */
    static void RequireAdmin();
    
    /**
     * @brief Get process privilege level
     */
    enum class PrivilegeLevel {
        Unknown,        ///< Could not determine
        User,           ///< Regular user
        Power,          ///< Power user
        Administrator   ///< Administrator
    };
    
    /**
     * @brief Get current process privilege level
     * 
     * @return Current privilege level
     */
    static PrivilegeLevel GetPrivilegeLevel();
    
    /**
     * @brief Get privilege level as string
     * 
     * @param level Privilege level enum
     * @return String representation
     */
    static std::string GetPrivilegeLevelString(PrivilegeLevel level);
    
private:
    /**
     * @brief Get current user's SID
     * 
     * @return User SID string
     */
    static std::string GetUserSID();
};

} // namespace Core
} // namespace UchihaSpoofer
