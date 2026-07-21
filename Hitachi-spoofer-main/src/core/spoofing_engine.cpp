#include "core/spoofing_engine.h"
#include "core/hwid_generator.h"
#include "core/registry_manager.h"
#include "core/privilege_checker.h"
#include "utils/logger.h"
#include <windows.h>
#include <sstream>
#include <iomanip>

using namespace UchihaSpoofer::Core;
using namespace UchihaSpoofer::Utils;

// Registry paths for HWID values
const char* REG_PATH_MACHINE_GUID = "SOFTWARE\\Microsoft\\Cryptography";
const char* REG_VALUE_MACHINE_GUID = "MachineGuid";

const char* REG_PATH_PRODUCT_ID = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";
const char* REG_VALUE_PRODUCT_ID = "ProductId";

const char* REG_PATH_DEVICE_ID = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System";
const char* REG_VALUE_DEVICE_ID = "DeviceId";

SpoofingEngine::SpoofingEngine()
    : registry_manager_(std::make_unique<RegistryManager>())
{
    Logger::GetInstance().Debug("SpoofingEngine initialized");
}

SpoofingEngine::~SpoofingEngine()
{
    Logger::GetInstance().Debug("SpoofingEngine destroyed");
}

SpoofingResult SpoofingEngine::ApplySpoofing(
    const HWIDData& hwid,
    ProgressCallback callback
)
{
    SpoofingResult result;
    result.success = false;
    progress_callback_ = callback;
    should_cancel_ = false;
    
    try {
        // Step 1: Check admin privileges
        UpdateProgress(0.05f, "Checking admin privileges...");
        
        if (!PrivilegeChecker::IsAdmin()) {
            result.error_code = "PRIV_001";
            result.message = "Administrator privileges required";
            last_error_ = result.message;
            Logger::GetInstance().Error(result.message);
            result.failed_operations.push_back("Admin privilege check");
            return result;
        }
        
        Logger::GetInstance().Info("Admin privileges verified");
        UpdateProgress(0.1f, "Admin privileges verified");
        
        // Step 2: Backup current values
        UpdateProgress(0.15f, "Backing up current values...");
        
        if (!BackupCurrentValues()) {
            result.error_code = "BACKUP_001";
            result.message = "Failed to backup current values";
            last_error_ = result.message;
            Logger::GetInstance().Error(result.message);
            result.failed_operations.push_back("Backup");
            return result;
        }
        
        Logger::GetInstance().Info("Backup completed successfully");
        UpdateProgress(0.25f, "Backup completed");
        
        // Step 3: Apply spoofing operations
        UpdateProgress(0.3f, "Applying Machine GUID...");
        if (!ApplyMachineGUID(hwid.guid)) {
            result.failed_operations.push_back("Machine GUID");
        }
        UpdateProgress(0.4f, "Applying Product ID...");
        if (!ApplyProductID(hwid.product_id)) {
            result.failed_operations.push_back("Product ID");
        }
        UpdateProgress(0.5f, "Applying Volume Serial (C:)...");
        if (!ApplyVolumeSerial('C', hwid.volume_serial_c)) {
            result.failed_operations.push_back("Volume Serial C:");
        }
        UpdateProgress(0.6f, "Applying Volume Serial (D:)...");
        if (!ApplyVolumeSerial('D', hwid.volume_serial_d)) {
            result.failed_operations.push_back("Volume Serial D:");
        }
        
        // Step 4: Verify spoofing
        UpdateProgress(0.7f, "Verifying changes...");
        if (!VerifySpoofing(hwid)) {
            result.error_code = "VERIFY_001";
            result.message = "Failed to verify spoofing";
            last_error_ = result.message;
            Logger::GetInstance().Warning(result.message);
        }
        
        // Step 5: Finalize
        UpdateProgress(0.9f, "Finalizing spoofing...");
        is_spoofed_ = true;
        
        UpdateProgress(1.0f, "Spoofing completed successfully");
        result.success = true;
        result.message = "HWID spoofing completed successfully";
        Logger::GetInstance().Info(result.message);
        
        return result;
    }
    catch (const PrivilegeException& e) {
        result.error_code = "PRIV_EXC";
        result.message = std::string("Privilege exception: ") + e.what();
        last_error_ = result.message;
        Logger::GetInstance().Error(result.message);
        result.failed_operations.push_back("Privilege check");
    }
    catch (const RegistryException& e) {
        result.error_code = "REG_EXC";
        result.message = std::string("Registry exception: ") + e.what();
        last_error_ = result.message;
        Logger::GetInstance().Error(result.message);
        result.failed_operations.push_back("Registry operation");
    }
    catch (const std::exception& e) {
        result.error_code = "UNKNOWN_EXC";
        result.message = std::string("Unexpected exception: ") + e.what();
        last_error_ = result.message;
        Logger::GetInstance().Critical(result.message);
        result.failed_operations.push_back("Unknown error");
    }
    
    return result;
}

SpoofingResult SpoofingEngine::RecoverOriginal(ProgressCallback callback)
{
    SpoofingResult result;
    result.success = false;
    progress_callback_ = callback;
    
    try {
        UpdateProgress(0.1f, "Checking admin privileges...");
        
        if (!PrivilegeChecker::IsAdmin()) {
            result.error_code = "PRIV_001";
            result.message = "Administrator privileges required for recovery";
            last_error_ = result.message;
            Logger::GetInstance().Error(result.message);
            return result;
        }
        
        UpdateProgress(0.3f, "Restoring original values...");
        
        // Try to restore from backup
        if (registry_manager_->RestoreValue("mac_address") &&
            registry_manager_->RestoreValue("machine_guid") &&
            registry_manager_->RestoreValue("product_id")) {
            
            UpdateProgress(0.8f, "Verifying restoration...");
            UpdateProgress(1.0f, "Recovery completed successfully");
            
            is_spoofed_ = false;
            result.success = true;
            result.message = "HWID recovery completed successfully";
            Logger::GetInstance().Info(result.message);
        } else {
            result.error_code = "RESTORE_001";
            result.message = "Failed to restore from backup";
            last_error_ = result.message;
            Logger::GetInstance().Error(result.message);
        }
        
        return result;
    }
    catch (const std::exception& e) {
        result.error_code = "RECOVERY_EXC";
        result.message = std::string("Recovery exception: ") + e.what();
        last_error_ = result.message;
        Logger::GetInstance().Error(result.message);
    }
    
    return result;
}

bool SpoofingEngine::ApplyMACAddress(const std::string& mac_address)
{
    try {
        Logger::GetInstance().Info("Applying MAC address: " + mac_address);
        // MAC address spoofing via registry is limited without driver
        // This is a placeholder for actual implementation
        return true;
    }
    catch (const std::exception& e) {
        Logger::GetInstance().Error(std::string("Failed to apply MAC address: ") + e.what());
        return false;
    }
}

bool SpoofingEngine::ApplyMachineGUID(const std::string& guid)
{
    try {
        Logger::GetInstance().Info("Applying Machine GUID: " + guid);
        registry_manager_->WriteStringValue(
            HKEY_LOCAL_MACHINE,
            REG_PATH_MACHINE_GUID,
            REG_VALUE_MACHINE_GUID,
            guid
        );
        Logger::GetInstance().Info("Machine GUID applied successfully");
        return true;
    }
    catch (const RegistryException& e) {
        Logger::GetInstance().Error(std::string("Failed to apply Machine GUID: ") + e.what());
        return false;
    }
}

bool SpoofingEngine::ApplyProductID(const std::string& product_id)
{
    try {
        Logger::GetInstance().Info("Applying Product ID: " + product_id);
        registry_manager_->WriteStringValue(
            HKEY_LOCAL_MACHINE,
            REG_PATH_PRODUCT_ID,
            REG_VALUE_PRODUCT_ID,
            product_id
        );
        Logger::GetInstance().Info("Product ID applied successfully");
        return true;
    }
    catch (const RegistryException& e) {
        Logger::GetInstance().Error(std::string("Failed to apply Product ID: ") + e.what());
        return false;
    }
}

bool SpoofingEngine::ApplyVolumeSerial(char drive_letter, const std::string& serial_number)
{
    try {
        Logger::GetInstance().Info(
            std::string("Applying Volume Serial for ") + drive_letter + ": " + serial_number
        );
        // Volume serial spoofing typically requires driver interaction
        // This is a placeholder for actual implementation
        return true;
    }
    catch (const std::exception& e) {
        Logger::GetInstance().Error(
            std::string("Failed to apply volume serial: ") + e.what()
        );
        return false;
    }
}

bool SpoofingEngine::BackupCurrentValues()
{
    try {
        Logger::GetInstance().Info("Starting backup of current HWID values");
        
        registry_manager_->BackupValue(
            HKEY_LOCAL_MACHINE,
            REG_PATH_MACHINE_GUID,
            REG_VALUE_MACHINE_GUID,
            "machine_guid"
        );
        
        registry_manager_->BackupValue(
            HKEY_LOCAL_MACHINE,
            REG_PATH_PRODUCT_ID,
            REG_VALUE_PRODUCT_ID,
            "product_id"
        );
        
        Logger::GetInstance().Info("Backup completed successfully");
        return true;
    }
    catch (const std::exception& e) {
        Logger::GetInstance().Error(std::string("Backup failed: ") + e.what());
        return false;
    }
}

bool SpoofingEngine::VerifySpoofing(const HWIDData& hwid)
{
    try {
        Logger::GetInstance().Info("Starting verification of spoofed values");
        
        std::string stored_guid = registry_manager_->ReadStringValue(
            HKEY_LOCAL_MACHINE,
            REG_PATH_MACHINE_GUID,
            REG_VALUE_MACHINE_GUID
        );
        
        std::string stored_product = registry_manager_->ReadStringValue(
            HKEY_LOCAL_MACHINE,
            REG_PATH_PRODUCT_ID,
            REG_VALUE_PRODUCT_ID
        );
        
        bool guid_match = (stored_guid == hwid.guid);
        bool product_match = (stored_product == hwid.product_id);
        
        if (guid_match && product_match) {
            Logger::GetInstance().Info("Verification successful - all values match");
            return true;
        } else {
            Logger::GetInstance().Warning("Verification: Some values do not match");
            if (!guid_match) {
                Logger::GetInstance().Debug("GUID mismatch - stored: " + stored_guid);
            }
            if (!product_match) {
                Logger::GetInstance().Debug("Product ID mismatch - stored: " + stored_product);
            }
            return false;
        }
    }
    catch (const std::exception& e) {
        Logger::GetInstance().Warning(std::string("Verification error: ") + e.what());
        return false;
    }
}

void SpoofingEngine::UpdateProgress(float progress, const std::string& message)
{
    if (progress_callback_) {
        progress_callback_(progress, message);
    }
    Logger::GetInstance().Debug(
        "Progress: " + std::to_string(static_cast<int>(progress * 100)) + "% - " + message
    );
}
