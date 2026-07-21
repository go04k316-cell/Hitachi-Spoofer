#pragma once

#include <atomic>
#include <mutex>
#include <vector>
#include <functional>
#include <chrono>
#include <string>

namespace UchihaSpoofer {
namespace App {

enum class ApplicationState;

/**
 * @brief Progress tracking information
 */
struct OperationProgress {
    float progress = 0.0f;              ///< Progress percentage 0.0-1.0
    std::string message;                ///< Status message
    std::string task_name;              ///< Current task name
    std::chrono::milliseconds elapsed;  ///< Time elapsed
};

/**
 * @brief Application State Manager
 * 
 * Singleton class managing centralized application state including:
 * - Operation progress tracking
 * - Spoofing status
 * - Configuration state
 * - UI state synchronization
 * 
 * Thread-safe implementation using atomic variables and mutex locks.
 */
class AppStateManager {
public:
    /**
     * @brief Get singleton instance
     * @return Reference to AppStateManager instance
     */
    static AppStateManager& Get();
    
    /**
     * @brief Initialize state manager
     * @return True if successful
     */
    bool Initialize();
    
    /**
     * @brief Shutdown state manager
     */
    void Shutdown();
    
    // ============ State Management ============
    
    /**
     * @brief Get current application state
     * @return Current application state
     */
    ApplicationState GetState() const;
    
    /**
     * @brief Set application state
     * @param state New state
     */
    void SetState(ApplicationState state);
    
    /**
     * @brief Register state change callback
     * @param callback Function called when state changes
     */
    using StateChangeCallback = std::function<void(ApplicationState)>;
    void OnStateChanged(StateChangeCallback callback);
    
    // ============ Spoofing Status ============
    
    /**
     * @brief Check if system is currently spoofed
     * @return True if spoofed
     */
    bool IsSpoofed() const { return is_spoofed_; }
    
    /**
     * @brief Set spoof status
     * @param spoofed True if spoofed
     */
    void SetSpoofStatus(bool spoofed);
    
    /**
     * @brief Get last successful spoof time
     * @return Timestamp of last successful spoof
     */
    std::chrono::system_clock::time_point GetLastSpoofTime() const;
    
    // ============ Privilege Status ============
    
    /**
     * @brief Check if running with admin privileges
     * @return True if admin
     */
    bool IsAdmin() const { return is_admin_; }
    
    /**
     * @brief Set admin privilege status
     * @param is_admin True if admin
     */
    void SetAdminStatus(bool is_admin);
    
    // ============ Progress Tracking ============
    
    /**
     * @brief Update spoofing operation progress
     * @param progress Progress 0.0-1.0
     * @param message Status message
     */
    void UpdateSpoofProgress(float progress, const std::string& message);
    
    /**
     * @brief Get spoofing operation progress
     * @return Current progress information
     */
    OperationProgress GetSpoofProgress() const;
    
    /**
     * @brief Update recovery operation progress
     * @param progress Progress 0.0-1.0
     * @param message Status message
     */
    void UpdateRecoveryProgress(float progress, const std::string& message);
    
    /**
     * @brief Get recovery operation progress
     * @return Current progress information
     */
    OperationProgress GetRecoveryProgress() const;
    
    /**
     * @brief Update clean operation progress
     * @param progress Progress 0.0-1.0
     * @param message Status message
     */
    void UpdateCleanProgress(float progress, const std::string& message);
    
    /**
     * @brief Get clean operation progress
     * @return Current progress information
     */
    OperationProgress GetCleanProgress() const;
    
    /**
     * @brief Reset all progress trackers
     */
    void ResetAllProgress();
    
    // ============ Operation Control ============
    
    /**
     * @brief Check if operation is in progress
     * @return True if any operation running
     */
    bool IsOperationInProgress() const;
    
    /**
     * @brief Request cancel of current operation
     */
    void RequestCancel();
    
    /**
     * @brief Check if cancel was requested
     * @return True if cancel requested
     */
    bool IsCancelRequested() const { return cancel_requested_; }
    
    /**
     * @brief Clear cancel request
     */
    void ClearCancelRequest() { cancel_requested_ = false; }
    
    // ============ Configuration State ============
    
    /**
     * @brief Set current active profile
     * @param profile_name Name of profile
     */
    void SetActiveProfile(const std::string& profile_name);
    
    /**
     * @brief Get current active profile
     * @return Name of active profile
     */
    std::string GetActiveProfile() const;
    
    /**
     * @brief Set last used backup name
     * @param backup_name Name of backup
     */
    void SetLastBackup(const std::string& backup_name);
    
    /**
     * @brief Get last used backup name
     * @return Name of last backup
     */
    std::string GetLastBackup() const;
    
    // ============ Error Handling ============
    
    /**
     * @brief Set last error message
     * @param error_code Error code
     * @param message Error message
     */
    void SetLastError(const std::string& error_code, const std::string& message);
    
    /**
     * @brief Get last error message
     * @return Last error message
     */
    std::string GetLastErrorMessage() const;
    
    /**
     * @brief Get last error code
     * @return Last error code
     */
    std::string GetLastErrorCode() const;
    
    /**
     * @brief Clear error state
     */
    void ClearError();
    
private:
    /**
     * @brief Private constructor (singleton)
     */
    AppStateManager() = default;
    
    /**
     * @brief Deleted copy constructor
     */
    AppStateManager(const AppStateManager&) = delete;
    
    /**
     * @brief Deleted move constructor
     */
    AppStateManager(AppStateManager&&) = delete;
    
    /**
     * @brief Deleted assignment operator
     */
    AppStateManager& operator=(const AppStateManager&) = delete;
    
    // Notify state change listeners
    void NotifyStateChange(ApplicationState new_state);
    
    // Thread safety
    mutable std::mutex state_mutex_;
    mutable std::mutex progress_mutex_;
    
    // State variables
    ApplicationState current_state_ = ApplicationState::Initializing;
    std::atomic<bool> is_spoofed_{false};
    std::atomic<bool> is_admin_{false};
    std::atomic<bool> cancel_requested_{false};
    
    // Progress tracking
    OperationProgress spoof_progress_;
    OperationProgress recovery_progress_;
    OperationProgress clean_progress_;
    
    // Configuration state
    std::string active_profile_;
    std::string last_backup_;
    
    // Error state
    std::string last_error_code_;
    std::string last_error_message_;
    
    // Timestamps
    std::chrono::system_clock::time_point last_spoof_time_;
    
    // Callbacks
    std::vector<StateChangeCallback> state_change_callbacks_;
};

} // namespace App
} // namespace UchihaSpoofer
