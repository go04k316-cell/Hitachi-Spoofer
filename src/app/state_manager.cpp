#include "app/state_manager.h"
#include "utils/logger.h"
#include <algorithm>

using namespace UchihaSpoofer;
using namespace UchihaSpoofer::App;
using namespace UchihaSpoofer::Utils;

// Forward declare ApplicationState enum
enum class ApplicationState;

AppStateManager& AppStateManager::Get()
{
    static AppStateManager instance;
    return instance;
}

bool AppStateManager::Initialize()
{
    Logger::GetInstance().Debug("Initializing AppStateManager");
    ResetAllProgress();
    current_state_ = ApplicationState::Ready;
    return true;
}

void AppStateManager::Shutdown()
{
    Logger::GetInstance().Debug("Shutting down AppStateManager");
}

ApplicationState AppStateManager::GetState() const
{
    std::lock_guard<std::mutex> lock(state_mutex_);
    return current_state_;
}

void AppStateManager::SetState(ApplicationState state)
{
    {
        std::lock_guard<std::mutex> lock(state_mutex_);
        if (current_state_ == state) {
            return; // No state change
        }
        current_state_ = state;
    }
    NotifyStateChange(state);
}

void AppStateManager::OnStateChanged(StateChangeCallback callback)
{
    std::lock_guard<std::mutex> lock(state_mutex_);
    state_change_callbacks_.push_back(callback);
}

void AppStateManager::SetSpoofStatus(bool spoofed)
{
    is_spoofed_ = spoofed;
    if (spoofed) {
        last_spoof_time_ = std::chrono::system_clock::now();
        Logger::GetInstance().Info("Spoof status changed to: SPOOFED");
    } else {
        Logger::GetInstance().Info("Spoof status changed to: ORIGINAL");
    }
}

std::chrono::system_clock::time_point AppStateManager::GetLastSpoofTime() const
{
    return last_spoof_time_;
}

void AppStateManager::SetAdminStatus(bool is_admin)
{
    is_admin_ = is_admin;
    Logger::GetInstance().Debug("Admin status: " + std::string(is_admin ? "YES" : "NO"));
}

void AppStateManager::UpdateSpoofProgress(float progress, const std::string& message)
{
    {
        std::lock_guard<std::mutex> lock(progress_mutex_);
        spoof_progress_.progress = std::clamp(progress, 0.0f, 1.0f);
        spoof_progress_.message = message;
        spoof_progress_.elapsed = std::chrono::milliseconds{
            static_cast<long long>(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count())
        };
    }
    Logger::GetInstance().Debug("Spoof progress: " + std::to_string((int)(progress * 100)) + "% - " + message);
}

OperationProgress AppStateManager::GetSpoofProgress() const
{
    std::lock_guard<std::mutex> lock(progress_mutex_);
    return spoof_progress_;
}

void AppStateManager::UpdateRecoveryProgress(float progress, const std::string& message)
{
    {
        std::lock_guard<std::mutex> lock(progress_mutex_);
        recovery_progress_.progress = std::clamp(progress, 0.0f, 1.0f);
        recovery_progress_.message = message;
    }
    Logger::GetInstance().Debug("Recovery progress: " + std::to_string((int)(progress * 100)) + "% - " + message);
}

OperationProgress AppStateManager::GetRecoveryProgress() const
{
    std::lock_guard<std::mutex> lock(progress_mutex_);
    return recovery_progress_;
}

void AppStateManager::UpdateCleanProgress(float progress, const std::string& message)
{
    {
        std::lock_guard<std::mutex> lock(progress_mutex_);
        clean_progress_.progress = std::clamp(progress, 0.0f, 1.0f);
        clean_progress_.message = message;
    }
    Logger::GetInstance().Debug("Clean progress: " + std::to_string((int)(progress * 100)) + "% - " + message);
}

OperationProgress AppStateManager::GetCleanProgress() const
{
    std::lock_guard<std::mutex> lock(progress_mutex_);
    return clean_progress_;
}

void AppStateManager::ResetAllProgress()
{
    std::lock_guard<std::mutex> lock(progress_mutex_);
    spoof_progress_ = OperationProgress{};
    recovery_progress_ = OperationProgress{};
    clean_progress_ = OperationProgress{};
}

bool AppStateManager::IsOperationInProgress() const
{
    std::lock_guard<std::mutex> lock(progress_mutex_);
    return (spoof_progress_.progress > 0.0f && spoof_progress_.progress < 1.0f) ||
           (recovery_progress_.progress > 0.0f && recovery_progress_.progress < 1.0f) ||
           (clean_progress_.progress > 0.0f && clean_progress_.progress < 1.0f);
}

void AppStateManager::RequestCancel()
{
    cancel_requested_ = true;
    Logger::GetInstance().Info("Operation cancel requested");
}

void AppStateManager::SetActiveProfile(const std::string& profile_name)
{
    active_profile_ = profile_name;
    Logger::GetInstance().Debug("Active profile changed to: " + profile_name);
}

std::string AppStateManager::GetActiveProfile() const
{
    return active_profile_;
}

void AppStateManager::SetLastBackup(const std::string& backup_name)
{
    last_backup_ = backup_name;
    Logger::GetInstance().Debug("Last backup: " + backup_name);
}

std::string AppStateManager::GetLastBackup() const
{
    return last_backup_;
}

void AppStateManager::SetLastError(const std::string& error_code, const std::string& message)
{
    std::lock_guard<std::mutex> lock(state_mutex_);
    last_error_code_ = error_code;
    last_error_message_ = message;
    Logger::GetInstance().Error("[" + error_code + "] " + message);
}

std::string AppStateManager::GetLastErrorMessage() const
{
    std::lock_guard<std::mutex> lock(state_mutex_);
    return last_error_message_;
}

std::string AppStateManager::GetLastErrorCode() const
{
    std::lock_guard<std::mutex> lock(state_mutex_);
    return last_error_code_;
}

void AppStateManager::ClearError()
{
    std::lock_guard<std::mutex> lock(state_mutex_);
    last_error_code_.clear();
    last_error_message_.clear();
}

void AppStateManager::NotifyStateChange(ApplicationState new_state)
{
    std::lock_guard<std::mutex> lock(state_mutex_);
    for (auto& callback : state_change_callbacks_) {
        if (callback) {
            callback(new_state);
        }
    }
}
