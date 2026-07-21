#pragma once

#include <string>
#include <memory>
#include <functional>
#include <atomic>
#include <mutex>

namespace UchihaSpoofer {
namespace App {

/**
 * @brief Application state enumeration
 */
enum class ApplicationState {
    Initializing,           ///< Application starting
    Ready,                  ///< Application ready for operations
    OperationInProgress,    ///< Async operation running
    Error,                  ///< Error state
    Shutting Down           ///< Application shutting down
};

/**
 * @brief Progress information for operations
 */
struct ProgressInfo {
    float percentage = 0.0f;            ///< Progress 0.0 to 1.0
    std::string message;                ///< Progress message
    std::string task_name;              ///< Current task name
    std::chrono::milliseconds elapsed;  ///< Time elapsed
};

/**
 * @brief Application configuration structure
 */
struct AppConfig {
    bool enable_logging = true;
    std::string log_file_path = "logs/spoofer.log";
    bool auto_backup = true;
    std::string backup_directory = "backups/";
    bool remember_ui_state = true;
};

// Forward declarations
class AppStateManager;

/**
 * @brief Main Application class
 * 
 * Orchestrates all application components including UI, state management,
 * and core business logic.
 */
class Application {
public:
    /**
     * @brief Constructor
     */
    Application();
    
    /**
     * @brief Destructor
     */
    ~Application();
    
    /**
     * @brief Initialize application
     * 
     * Sets up all components, validates privileges, initializes UI.
     * 
     * @param config Application configuration
     * @return True if initialization successful
     */
    bool Initialize(const AppConfig& config = AppConfig());
    
    /**
     * @brief Run main application loop
     * 
     * Blocks until application should exit.
     * 
     * @return Exit code
     */
    int Run();
    
    /**
     * @brief Shutdown application
     * 
     * Cleanup resources, save state, close UI.
     */
    void Shutdown();
    
    /**
     * @brief Request application exit
     * 
     * Gracefully exit the application.
     */
    void RequestExit();
    
    /**
     * @brief Check if application should exit
     * @return True if exit requested
     */
    bool ShouldExit() const { return should_exit_; }
    
    /**
     * @brief Get current application state
     * @return Current application state
     */
    ApplicationState GetState() const;
    
    /**
     * @brief Get state manager instance
     * @return Reference to state manager
     */
    AppStateManager& GetStateManager();
    
private:
    /**
     * @brief Initialize logger
     * @param log_file_path Path to log file
     * @return True if successful
     */
    bool InitializeLogger(const std::string& log_file_path);
    
    /**
     * @brief Initialize configuration
     * @return True if successful
     */
    bool InitializeConfig();
    
    /**
     * @brief Validate system requirements
     * @return True if requirements met
     */
    bool ValidateSystemRequirements();
    
    /**
     * @brief Initialize UI components
     * @return True if successful
     */
    bool InitializeUI();
    
    /**
     * @brief Process main loop frame
     * @return True if should continue running
     */
    bool ProcessFrame();
    
    /**
     * @brief Render UI frame
     */
    void RenderFrame();
    
    /**
     * @brief Update application state
     */
    void UpdateState();
    
    // Member variables
    std::unique_ptr<AppStateManager> state_manager_;
    ApplicationState current_state_ = ApplicationState::Initializing;
    
    std::atomic<bool> should_exit_{false};
    std::atomic<bool> is_initialized_{false};
    
    mutable std::mutex state_mutex_;
    
    AppConfig config_;
    
    // UI window handle (Windows-specific)
    void* window_handle_ = nullptr;
    
    // Frame timing
    std::chrono::high_resolution_clock::time_point last_frame_time_;
    const float TARGET_FRAME_TIME = 1.0f / 60.0f; // 60 FPS
};

} // namespace App
} // namespace UchihaSpoofer
