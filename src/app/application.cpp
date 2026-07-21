#include "app/application.h"
#include "app/state_manager.h"
#include "utils/logger.h"
#include <windows.h>
#include <chrono>
#include <thread>

using namespace UchihaSpoofer;
using namespace UchihaSpoofer::App;
using namespace UchihaSpoofer::Utils;

Application::Application() 
    : state_manager_(std::make_unique<AppStateManager>())
{
}

Application::~Application()
{
    if (is_initialized_) {
        Shutdown();
    }
}

bool Application::Initialize(const AppConfig& config)
{
    try {
        Logger::GetInstance().Info("\n========================================");
        Logger::GetInstance().Info("Hitachi Spoofer v4.0.0 - Starting");
        Logger::GetInstance().Info("========================================\n");
        
        config_ = config;
        
        // Step 1: Initialize logger
        Logger::GetInstance().Info("Initializing logger...");
        if (!InitializeLogger(config_.log_file_path)) {
            Logger::GetInstance().Error("Failed to initialize logger");
            return false;
        }
        Logger::GetInstance().Info("Logger initialized successfully");
        
        // Step 2: Validate system requirements
        Logger::GetInstance().Info("Validating system requirements...");
        if (!ValidateSystemRequirements()) {
            Logger::GetInstance().Error("System requirements not met");
            return false;
        }
        Logger::GetInstance().Info("System requirements validated");
        
        // Step 3: Initialize configuration
        Logger::GetInstance().Info("Loading configuration...");
        if (!InitializeConfig()) {
            Logger::GetInstance().Error("Failed to initialize configuration");
            return false;
        }
        Logger::GetInstance().Info("Configuration loaded");
        
        // Step 4: Initialize UI
        Logger::GetInstance().Info("Initializing UI...");
        if (!InitializeUI()) {
            Logger::GetInstance().Error("Failed to initialize UI");
            return false;
        }
        Logger::GetInstance().Info("UI initialized");
        
        // Step 5: Initialize state manager
        Logger::GetInstance().Info("Initializing state manager...");
        state_manager_->Initialize();
        Logger::GetInstance().Info("State manager initialized");
        
        // Mark as initialized and ready
        is_initialized_ = true;
        current_state_ = ApplicationState::Ready;
        state_manager_->SetState(ApplicationState::Ready);
        
        Logger::GetInstance().Info("\n========================================");
        Logger::GetInstance().Info("Application initialized successfully!");
        Logger::GetInstance().Info("========================================\n");
        
        return true;
    }
    catch (const std::exception& e) {
        Logger::GetInstance().Critical("Initialization failed: " + std::string(e.what()));
        return false;
    }
}

int Application::Run()
{
    if (!is_initialized_) {
        Logger::GetInstance().Error("Application not initialized");
        return 1;
    }
    
    Logger::GetInstance().Info("Entering main application loop...");
    last_frame_time_ = std::chrono::high_resolution_clock::now();
    
    while (!should_exit_) {
        if (!ProcessFrame()) {
            break;
        }
        
        // Frame rate limiting
        auto current_time = std::chrono::high_resolution_clock::now();
        auto frame_duration = std::chrono::duration<float>(current_time - last_frame_time_).count();
        
        if (frame_duration < TARGET_FRAME_TIME) {
            auto sleep_ms = (int)((TARGET_FRAME_TIME - frame_duration) * 1000.0f);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
        }
        
        last_frame_time_ = std::chrono::high_resolution_clock::now();
    }
    
    Logger::GetInstance().Info("Main application loop exited");
    return 0;
}

void Application::Shutdown()
{
    Logger::GetInstance().Info("\nShutting down application...");
    
    current_state_ = ApplicationState::Shutting Down;
    state_manager_->SetState(ApplicationState::Shutting Down);
    
    // TODO: Clean up UI resources
    if (window_handle_ != nullptr) {
        // DestroyWindow((HWND)window_handle_);
        window_handle_ = nullptr;
    }
    
    Logger::GetInstance().Info("Saving application state...");
    // TODO: Save state to configuration
    
    Logger::GetInstance().Info("Application shutdown complete\n");
    is_initialized_ = false;
}

void Application::RequestExit()
{
    Logger::GetInstance().Info("Exit requested");
    should_exit_ = true;
}

ApplicationState Application::GetState() const
{
    std::lock_guard<std::mutex> lock(state_mutex_);
    return current_state_;
}

AppStateManager& Application::GetStateManager()
{
    return *state_manager_;
}

bool Application::InitializeLogger(const std::string& log_file_path)
{
    try {
        auto& logger = Logger::GetInstance();
        logger.SetMinLogLevel(LogLevel::Info);
        logger.InitializeFileLogging(log_file_path);
        return true;
    }
    catch (const std::exception& e) {
        // Log to console since file logging failed
        std::cerr << "Failed to initialize file logging: " << e.what() << std::endl;
        return false;
    }
}

bool Application::InitializeConfig()
{
    try {
        // TODO: Load configuration from file
        // ConfigManager::GetInstance().LoadConfig("config.json");
        return true;
    }
    catch (const std::exception& e) {
        Logger::GetInstance().Error("Configuration initialization failed: " + std::string(e.what()));
        return false;
    }
}

bool Application::ValidateSystemRequirements()
{
    try {
        // Check Windows version
        OSVERSIONINFO os_info = {};
        os_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        GetVersionExA(&os_info);
        
        if (os_info.dwMajorVersion < 10) {
            Logger::GetInstance().Error("Windows 10 or later required");
            return false;
        }
        
        Logger::GetInstance().Debug("Windows version: " + std::to_string(os_info.dwMajorVersion));
        
        // Check admin privileges
        // This will be checked more thoroughly in actual operations
        Logger::GetInstance().Debug("System requirements validated");
        
        return true;
    }
    catch (const std::exception& e) {
        Logger::GetInstance().Error("System validation failed: " + std::string(e.what()));
        return false;
    }
}

bool Application::InitializeUI()
{
    try {
        // TODO: Initialize ImGui and window
        // - Create window
        // - Setup ImGui context
        // - Load theme/style
        
        Logger::GetInstance().Debug("UI initialization");
        return true;
    }
    catch (const std::exception& e) {
        Logger::GetInstance().Error("UI initialization failed: " + std::string(e.what()));
        return false;
    }
}

bool Application::ProcessFrame()
{
    try {
        // Process system messages
        // TODO: Message loop handling for Windows
        
        // Update application state
        UpdateState();
        
        // Render frame
        RenderFrame();
        
        return !should_exit_;
    }
    catch (const std::exception& e) {
        Logger::GetInstance().Error("Frame processing error: " + std::string(e.what()));
        current_state_ = ApplicationState::Error;
        return false;
    }
}

void Application::RenderFrame()
{
    // TODO: ImGui rendering
    // - ImGui::NewFrame()
    // - Render tabs and dialogs
    // - ImGui::Render()
}

void Application::UpdateState()
{
    // TODO: Update application state
    // - Check for operation completion
    // - Update progress
    // - Handle state transitions
}
