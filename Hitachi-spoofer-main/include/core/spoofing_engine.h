#pragma once

#include <string>
#include <functional>
#include <vector>

namespace UchihaSpoofer {
namespace Core {

// Forward declarations
struct HWIDData;
class RegistryManager;
class PrivilegeChecker;

/**
 * @brief Spoofing operation result
 */
struct SpoofingResult {
    bool success = false;                          ///< Operation succeeded
    std::string message;                           ///< Result message
    std::string error_code;                        ///< Error code if failed
    std::vector<std::string> failed_operations;    ///< List of failed operations
};

/**
 * @brief Progress callback function type
 * 
 * @param progress Progress from 0.0 to 1.0
 * @param message Status message
 */
using ProgressCallback = std::function<void(float, const std::string&)>;

/**
 * @brief Spoofing Engine - Applies HWID spoofing operations
 * 
 * This class is responsible for applying hardware identifier spoofing to the system.
 * It handles registry modifications, progress tracking, and error handling.
 * 
 * Thread-safe for concurrent status queries but not for concurrent operations.
 */
class SpoofingEngine {
public:
    /**
     * @brief Constructor
     */
    SpoofingEngine();
    
    /**
     * @brief Destructor
     */
    ~SpoofingEngine();
    
    /**
     * @brief Apply HWID spoofing to the system
     * 
     * This method performs the actual hardware identifier spoofing by:
     * 1. Validating administrator privileges
     * 2. Backing up current values
     * 3. Writing new values to registry
     * 4. Verifying changes
     * 
     * @param hwid Hardware ID data to apply
     * @param callback Optional progress callback function
     * @return SpoofingResult with success status and details
     * 
     * @throws PrivilegeException if admin rights are insufficient
     * @throws RegistryException if registry operations fail
     */
    SpoofingResult ApplySpoofing(
        const HWIDData& hwid,
        ProgressCallback callback = nullptr
    );
    
    /**
     * @brief Recover original HWID from backup
     * 
     * Restores the original hardware identifiers from a previous backup.
     * 
     * @param callback Optional progress callback function
     * @return SpoofingResult with success status and details
     * 
     * @throws std::runtime_error if backup not found
     */
    SpoofingResult RecoverOriginal(ProgressCallback callback = nullptr);
    
    /**
     * @brief Check if system is currently spoofed
     * 
     * @return True if system has been spoofed, false otherwise
     */
    bool IsSpoofed() const { return is_spoofed_; }
    
    /**
     * @brief Get last error message
     * 
     * @return Last error message from recent operation
     */
    std::string GetLastError() const { return last_error_; }
    
    /**
     * @brief Cancel ongoing operation
     * 
     * @note Safe to call from any thread
     */
    void Cancel() { should_cancel_ = true; }
    
private:
    /**
     * @brief Apply MAC address spoofing
     * 
     * @param mac_address New MAC address
     * @return True if successful
     */
    bool ApplyMACAddress(const std::string& mac_address);
    
    /**
     * @brief Apply Machine GUID spoofing
     * 
     * @param guid New GUID value
     * @return True if successful
     */
    bool ApplyMachineGUID(const std::string& guid);
    
    /**
     * @brief Apply Product ID spoofing
     * 
     * @param product_id New Product ID
     * @return True if successful
     */
    bool ApplyProductID(const std::string& product_id);
    
    /**
     * @brief Apply Volume Serial Number spoofing
     * 
     * @param drive_letter Drive letter (C, D, etc.)
     * @param serial_number New serial number
     * @return True if successful
     */
    bool ApplyVolumeSerial(char drive_letter, const std::string& serial_number);
    
    /**
     * @brief Backup current values before spoofing
     * 
     * @return True if backup successful
     */
    bool BackupCurrentValues();
    
    /**
     * @brief Verify spoofing was applied correctly
     * 
     * @param hwid Expected HWID data
     * @return True if all values match
     */
    bool VerifySpoofing(const HWIDData& hwid);
    
    /**
     * @brief Update progress callback
     * 
     * @param progress Progress 0.0 to 1.0
     * @param message Status message
     */
    void UpdateProgress(float progress, const std::string& message);
    
    // Member variables
    std::unique_ptr<RegistryManager> registry_manager_;  ///< Registry operations
    bool is_spoofed_ = false;                             ///< Current spoofing status
    std::string last_error_;                              ///< Last error message
    ProgressCallback progress_callback_;                  ///< Progress callback
    bool should_cancel_ = false;                          ///< Cancel flag
};

} // namespace Core
} // namespace UchihaSpoofer
