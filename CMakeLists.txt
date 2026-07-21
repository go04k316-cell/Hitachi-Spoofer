cmake_minimum_required(VERSION 3.16)
project(HitachiSpoofer VERSION 4.0.0 LANGUAGES CXX C)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Enable testing
enable_testing()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Hitachi-spoofer-main/include
)

# Source files for core module
set(CORE_SOURCES
    Hitachi-spoofer-main/src/core/hwid_generator.cpp
    Hitachi-spoofer-main/src/core/spoofing_engine.cpp
    Hitachi-spoofer-main/src/core/security/privilege_checker.cpp
    Hitachi-spoofer-main/src/core/registry_manager.cpp
)

# Source files for utils module
set(UTILS_SOURCES
    Hitachi-spoofer-main/src/utils/logger.cpp
)

# Source files for app module
set(APP_SOURCES
    Hitachi-spoofer-main/src/app/application.cpp
    Hitachi-spoofer-main/src/app/state_manager.cpp
)

# Main executable
set(MAIN_SOURCES
    Hitachi-spoofer-main/src/main.cpp
)

# Create executable
add_executable(HitachiSpoofer
    ${CORE_SOURCES}
    ${UTILS_SOURCES}
    ${APP_SOURCES}
    ${MAIN_SOURCES}
)

# Link Windows libraries
target_link_libraries(HitachiSpoofer PRIVATE
    user32      # Windows UI
    advapi32    # Registry and security
    shell32     # Shell functions
    userenv     # User environment
)

# Compiler specific settings
if(MSVC)
    # MSVC compiler options
    target_compile_options(HitachiSpoofer PRIVATE
        /W4                 # Warning level 4
        /WX                 # Treat warnings as errors
        /permissive-        # Strict standards conformance
        /EHsc               # Standard exception handling
        /Gm-                # Disable minimal rebuild
    )
    
    # Release optimizations
    if(CMAKE_BUILD_TYPE MATCHES Release)
        target_compile_options(HitachiSpoofer PRIVATE
            /O2             # Maximum optimization
            /Ob2            # Inline function expansion
            /Oi             # Intrinsic functions
            /Oy             # Frame pointer omission
            /GL             # Whole program optimization
        )
        target_link_options(HitachiSpoofer PRIVATE
            /LTCG            # Link-time code generation
        )
    endif()
endif()

# Set output name
set_target_properties(HitachiSpoofer PROPERTIES
    VERSION ${PROJECT_VERSION}
    OUTPUT_NAME "HitachiSpoofer"
)

# Print configuration info
message(STATUS "========================================")
message(STATUS "Hitachi Spoofer v${PROJECT_VERSION}")
message(STATUS "========================================")
message(STATUS "Generator: ${CMAKE_GENERATOR}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "========================================")
