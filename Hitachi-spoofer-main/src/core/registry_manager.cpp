#include "core/registry_manager.h"
#include "utils/logger.h"
#include <map>
#include <sstream>

using namespace UchihaSpoofer::Core;
using namespace UchihaSpoofer::Utils;

// Map to store backups
static std::map<std::string, std::pair<std::string, std::string>> g_backups;

RegistryManager::~RegistryManager()
{
    // Close all open keys
    for (HKEY key : open_keys_) {
        if (key != NULL && key != HKEY_LOCAL_MACHINE && key != HKEY_CURRENT_USER) {
            RegCloseKey(key);
        }
    }
    open_keys_.clear();
}

std::string RegistryManager::GetErrorMessage(LONG error_code)
{
    switch (error_code) {
        case ERROR_SUCCESS:
            return "Success";
        case ERROR_FILE_NOT_FOUND:
            return "Registry key or value not found";
        case ERROR_ACCESS_DENIED:
            return "Access denied - admin privileges required";
        case ERROR_INVALID_PARAMETER:
            return "Invalid parameter";
        case ERROR_OUTOFMEMORY:
            return "Out of memory";
        default: {
            std::ostringstream oss;
            oss << "Registry error (code: " << error_code << ")";
            return oss.str();
        }
    }
}

std::string RegistryManager::GetSystemErrorMessage(DWORD error_code)
{
    LPSTR message_buffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&message_buffer,
        0,
        NULL
    );
    
    std::string message(message_buffer, size);
    LocalFree(message_buffer);
    return message;
}

std::string RegistryManager::ReadStringValue(
    HKEY hkey,
    const std::string& subkey,
    const std::string& value_name
)
{
    HKEY key_handle = NULL;
    LONG result = RegOpenKeyExA(hkey, subkey.c_str(), 0, KEY_READ, &key_handle);
    
    if (result != ERROR_SUCCESS) {
        Logger::GetInstance().Error(
            "Failed to open registry key: " + GetErrorMessage(result)
        );
        throw RegistryException("Failed to open registry key: " + GetErrorMessage(result));
    }
    
    try {
        char value_buffer[512] = {0};
        DWORD value_size = sizeof(value_buffer);
        DWORD value_type = 0;
        
        result = RegQueryValueExA(
            key_handle,
            value_name.c_str(),
            NULL,
            &value_type,
            (LPBYTE)value_buffer,
            &value_size
        );
        
        if (result != ERROR_SUCCESS) {
            RegCloseKey(key_handle);
            Logger::GetInstance().Warning(
                "Failed to read registry value: " + GetErrorMessage(result)
            );
            throw RegistryException("Failed to read registry value: " + GetErrorMessage(result));
        }
        
        if (value_type != REG_SZ) {
            RegCloseKey(key_handle);
            Logger::GetInstance().Warning("Registry value is not a string type");
            throw RegistryException("Registry value is not a string type");
        }
        
        std::string result_string(value_buffer);
        RegCloseKey(key_handle);
        return result_string;
    }
    catch (...) {
        RegCloseKey(key_handle);
        throw;
    }
}

void RegistryManager::WriteStringValue(
    HKEY hkey,
    const std::string& subkey,
    const std::string& value_name,
    const std::string& value
)
{
    HKEY key_handle = NULL;
    DWORD disposition = 0;
    LONG result = RegCreateKeyExA(
        hkey,
        subkey.c_str(),
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_WRITE,
        NULL,
        &key_handle,
        &disposition
    );
    
    if (result != ERROR_SUCCESS) {
        Logger::GetInstance().Error(
            "Failed to create/open registry key: " + GetErrorMessage(result)
        );
        throw RegistryException(
            "Failed to create/open registry key: " + GetErrorMessage(result)
        );
    }
    
    try {
        result = RegSetValueExA(
            key_handle,
            value_name.c_str(),
            0,
            REG_SZ,
            (const BYTE*)value.c_str(),
            static_cast<DWORD>(value.length()) + 1
        );
        
        if (result != ERROR_SUCCESS) {
            RegCloseKey(key_handle);
            Logger::GetInstance().Error(
                "Failed to write registry value: " + GetErrorMessage(result)
            );
            throw RegistryException(
                "Failed to write registry value: " + GetErrorMessage(result)
            );
        }
        
        Logger::GetInstance().Info(
            "Successfully wrote registry value: " + value_name
        );
        RegCloseKey(key_handle);
    }
    catch (...) {
        RegCloseKey(key_handle);
        throw;
    }
}

DWORD RegistryManager::ReadDWordValue(
    HKEY hkey,
    const std::string& subkey,
    const std::string& value_name
)
{
    HKEY key_handle = NULL;
    LONG result = RegOpenKeyExA(hkey, subkey.c_str(), 0, KEY_READ, &key_handle);
    
    if (result != ERROR_SUCCESS) {
        Logger::GetInstance().Error(
            "Failed to open registry key: " + GetErrorMessage(result)
        );
        throw RegistryException("Failed to open registry key: " + GetErrorMessage(result));
    }
    
    try {
        DWORD value_buffer = 0;
        DWORD value_size = sizeof(value_buffer);
        DWORD value_type = 0;
        
        result = RegQueryValueExA(
            key_handle,
            value_name.c_str(),
            NULL,
            &value_type,
            (LPBYTE)&value_buffer,
            &value_size
        );
        
        if (result != ERROR_SUCCESS) {
            RegCloseKey(key_handle);
            Logger::GetInstance().Warning(
                "Failed to read registry value: " + GetErrorMessage(result)
            );
            throw RegistryException("Failed to read registry value: " + GetErrorMessage(result));
        }
        
        if (value_type != REG_DWORD) {
            RegCloseKey(key_handle);
            Logger::GetInstance().Warning("Registry value is not a DWORD type");
            throw RegistryException("Registry value is not a DWORD type");
        }
        
        RegCloseKey(key_handle);
        return value_buffer;
    }
    catch (...) {
        RegCloseKey(key_handle);
        throw;
    }
}

void RegistryManager::WriteDWordValue(
    HKEY hkey,
    const std::string& subkey,
    const std::string& value_name,
    DWORD value
)
{
    HKEY key_handle = NULL;
    DWORD disposition = 0;
    LONG result = RegCreateKeyExA(
        hkey,
        subkey.c_str(),
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_WRITE,
        NULL,
        &key_handle,
        &disposition
    );
    
    if (result != ERROR_SUCCESS) {
        Logger::GetInstance().Error(
            "Failed to create/open registry key: " + GetErrorMessage(result)
        );
        throw RegistryException(
            "Failed to create/open registry key: " + GetErrorMessage(result)
        );
    }
    
    try {
        result = RegSetValueExA(
            key_handle,
            value_name.c_str(),
            0,
            REG_DWORD,
            (const BYTE*)&value,
            sizeof(value)
        );
        
        if (result != ERROR_SUCCESS) {
            RegCloseKey(key_handle);
            Logger::GetInstance().Error(
                "Failed to write registry value: " + GetErrorMessage(result)
            );
            throw RegistryException(
                "Failed to write registry value: " + GetErrorMessage(result)
            );
        }
        
        Logger::GetInstance().Info(
            "Successfully wrote DWORD registry value: " + value_name
        );
        RegCloseKey(key_handle);
    }
    catch (...) {
        RegCloseKey(key_handle);
        throw;
    }
}

bool RegistryManager::BackupValue(
    HKEY hkey,
    const std::string& subkey,
    const std::string& value_name,
    const std::string& backup_name
)
{
    try {
        std::string current_value = ReadStringValue(hkey, subkey, value_name);
        g_backups[backup_name] = {subkey + "\\" + value_name, current_value};
        Logger::GetInstance().Info("Backed up registry value: " + backup_name);
        return true;
    }
    catch (const std::exception& e) {
        Logger::GetInstance().Error(std::string("Backup failed: ") + e.what());
        return false;
    }
}

bool RegistryManager::RestoreValue(const std::string& backup_name)
{
    try {
        auto it = g_backups.find(backup_name);
        if (it == g_backups.end()) {
            Logger::GetInstance().Warning("Backup not found: " + backup_name);
            return false;
        }
        
        // Parse the key path
        const auto& backup_data = it->second;
        size_t separator_pos = backup_data.first.find_last_of('\\');
        if (separator_pos == std::string::npos) {
            Logger::GetInstance().Error("Invalid backup data format");
            return false;
        }
        
        std::string subkey = backup_data.first.substr(0, separator_pos);
        std::string value_name = backup_data.first.substr(separator_pos + 1);
        std::string value = backup_data.second;
        
        WriteStringValue(HKEY_LOCAL_MACHINE, subkey, value_name, value);
        Logger::GetInstance().Info("Restored registry value from backup: " + backup_name);
        return true;
    }
    catch (const std::exception& e) {
        Logger::GetInstance().Error(std::string("Restore failed: ") + e.what());
        return false;
    }
}
