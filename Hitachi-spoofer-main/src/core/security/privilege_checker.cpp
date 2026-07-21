#include "core/privilege_checker.h"
#include "utils/logger.h"
#include <windows.h>
#include <sddl.h>
#include <userenv.h>
#include <sstream>

#pragma comment(lib, "userenv.lib")
#pragma comment(lib, "advapi32.lib")

using namespace UchihaSpoofer::Core;
using namespace UchihaSpoofer::Utils;

bool PrivilegeChecker::IsAdmin()
{
    BOOL is_admin = FALSE;
    HANDLE token_handle = NULL;
    
    try {
        // Open current process token
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token_handle)) {
            Logger::GetInstance().Error("Failed to open process token");
            return false;
        }
        
        // Get admin group SID
        PSID admin_sid = nullptr;
        SID_IDENTIFIER_AUTHORITY nt_authority = SECURITY_NT_AUTHORITY;
        
        if (!AllocateAndInitializeSid(&nt_authority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                                       DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0,
                                       &admin_sid)) {
            Logger::GetInstance().Error("Failed to initialize admin SID");
            if (token_handle) CloseHandle(token_handle);
            return false;
        }
        
        // Check if token has admin group
        if (!CheckTokenMembership(token_handle, admin_sid, &is_admin)) {
            Logger::GetInstance().Error("Failed to check token membership");
            is_admin = FALSE;
        }
        
        // Cleanup
        if (admin_sid) FreeSid(admin_sid);
        if (token_handle) CloseHandle(token_handle);
        
        return is_admin == TRUE;
    }
    catch (const std::exception& e) {
        Logger::GetInstance().Error(std::string("Exception checking admin: ") + e.what());
        if (token_handle) CloseHandle(token_handle);
        return false;
    }
}

void PrivilegeChecker::RequireAdmin()
{
    if (!IsAdmin()) {
        Logger::GetInstance().Error("Admin privileges required but not available");
        throw PrivilegeException("This operation requires administrator privileges.");
    }
}

PrivilegeChecker::PrivilegeLevel PrivilegeChecker::GetPrivilegeLevel()
{
    try {
        if (IsAdmin()) {
            return PrivilegeLevel::Administrator;
        }
        
        HANDLE token_handle = NULL;
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token_handle)) {
            return PrivilegeLevel::Unknown;
        }
        
        // Check for Power User group
        PSID power_user_sid = nullptr;
        SID_IDENTIFIER_AUTHORITY nt_authority = SECURITY_NT_AUTHORITY;
        
        if (AllocateAndInitializeSid(&nt_authority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                                     DOMAIN_ALIAS_RID_POWER_USERS, 0, 0, 0, 0, 0, 0,
                                     &power_user_sid)) {
            BOOL is_power_user = FALSE;
            if (CheckTokenMembership(token_handle, power_user_sid, &is_power_user)) {
                if (is_power_user) {
                    FreeSid(power_user_sid);
                    CloseHandle(token_handle);
                    return PrivilegeLevel::Power;
                }
            }
            FreeSid(power_user_sid);
        }
        
        CloseHandle(token_handle);
        return PrivilegeLevel::User;
    }
    catch (const std::exception& e) {
        Logger::GetInstance().Error(std::string("Exception getting privilege level: ") + e.what());
        return PrivilegeLevel::Unknown;
    }
}

std::string PrivilegeChecker::GetPrivilegeLevelString(PrivilegeLevel level)
{
    switch (level) {
        case PrivilegeLevel::Administrator:
            return "Administrator";
        case PrivilegeLevel::Power:
            return "Power User";
        case PrivilegeLevel::User:
            return "User";
        default:
            return "Unknown";
    }
}

std::string PrivilegeChecker::GetUserSID()
{
    try {
        HANDLE token_handle = NULL;
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token_handle)) {
            return "";
        }
        
        // Get token info size
        DWORD token_info_length = 0;
        GetTokenInformation(token_handle, TokenUser, NULL, 0, &token_info_length);
        
        if (token_info_length == 0) {
            CloseHandle(token_handle);
            return "";
        }
        
        // Get token info
        PTOKEN_USER token_user = (PTOKEN_USER)malloc(token_info_length);
        if (!token_user) {
            CloseHandle(token_handle);
            return "";
        }
        
        if (!GetTokenInformation(token_handle, TokenUser, token_user, token_info_length,
                                 &token_info_length)) {
            free(token_user);
            CloseHandle(token_handle);
            return "";
        }
        
        // Convert SID to string
        LPSTR sid_string = NULL;
        if (ConvertSidToStringSidA(token_user->User.Sid, &sid_string)) {
            std::string result(sid_string);
            LocalFree(sid_string);
            free(token_user);
            CloseHandle(token_handle);
            return result;
        }
        
        free(token_user);
        CloseHandle(token_handle);
        return "";
    }
    catch (const std::exception& e) {
        Logger::GetInstance().Error(std::string("Exception getting user SID: ") + e.what());
        return "";
    }
}
