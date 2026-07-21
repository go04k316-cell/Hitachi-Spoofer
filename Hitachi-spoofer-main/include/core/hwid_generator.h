#pragma once

#include <string>
#include <windows.h>

namespace UchihaSpoofer {
namespace Core {

/**
 * @brief Structure to hold HWID information
 */
struct HWIDData {
    std::string mac_address;        ///< MAC address (XX:XX:XX:XX:XX:XX)
    std::string guid;               ///< Machine GUID
    std::string serial_number;      ///< Serial number
    std::string product_id;         ///< Windows Product ID
    std::string cpu_serial;         ///< CPU serial number
    std::string bios_serial;        ///< BIOS serial number
    std::string volume_serial_c;    ///< C: drive serial
    std::string volume_serial_d;    ///< D: drive serial
};

/**
 * @brief HWID Generator - Generates and manages hardware identifiers
 * 
 * This class provides static methods to generate random hardware identifiers
 * in valid formats that match Windows system expectations.
 */
class HWIDGenerator {
public:
    /**
     * @brief Generate random MAC address
     * 
     * Generates a MAC address in the format XX:XX:XX:XX:XX:XX
     * where X is a hexadecimal digit (0-9, A-F).
     * 
     * @return MAC address string
     * @example "A5:C3:E1:9B:F2:47"
     */
    static std::string GenerateMAC();
    
    /**
     * @brief Generate random GUID
     * 
     * Generates a GUID (Globally Unique Identifier) in the format:
     * {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
     * 
     * @return GUID string
     * @example "{9A3F5B1C-7E4A-8B2D-C9F3-1E7A4B9C2D5F}"
     */
    static std::string GenerateGUID();
    
    /**
     * @brief Generate random serial number
     * 
     * Generates a hexadecimal serial number (8 hex digits).
     * 
     * @return Serial number string
     * @example "F2A4C8E1B5D9AB7C"
     */
    static std::string GenerateSerialNumber();
    
    /**
     * @brief Generate random Product ID
     * 
     * Generates a Windows Product ID in the format: XXXXX-XXXXX-XXXXX-XXXXX
     * where X is a decimal digit (0-9).
     * 
     * @return Product ID string
     * @example "45623-78912-34567-89012"
     */
    static std::string GenerateProductID();
    
    /**
     * @brief Generate random CPU serial number
     * 
     * Generates a CPU serial number (typically 12 hex digits).
     * 
     * @return CPU serial string
     * @example "F2A4C8E1B5D9"
     */
    static std::string GenerateCPUSerial();
    
    /**
     * @brief Generate random BIOS serial number
     * 
     * Generates a BIOS serial number (typically alphanumeric, 12 characters).
     * 
     * @return BIOS serial string
     * @example "X7K9M2P4Q1R8"
     */
    static std::string GenerateBIOSSerial();
    
    /**
     * @brief Generate random Volume Serial Number
     * 
     * Generates a volume/drive serial number in the format: XXXX-XXXX
     * where X is a hexadecimal digit.
     * 
     * @return Volume serial string
     * @example "3F4A-B2C1"
     */
    static std::string GenerateVolumeSerial();
    
    /**
     * @brief Generate complete HWID data
     * 
     * Generates all hardware identifiers and returns them in a single structure.
     * This is a convenience method that calls all individual generators.
     * 
     * @return HWIDData structure with all generated identifiers
     * @throws std::runtime_error if any generation fails
     */
    static HWIDData GenerateComplete();
    
private:
    /**
     * @brief Convert bytes to hexadecimal string
     * 
     * @param data Pointer to data bytes
     * @param size Number of bytes
     * @return Hexadecimal string representation
     */
    static std::string BytesToHex(const unsigned char* data, size_t size);
    
    /**
     * @brief Generate random byte (0-255)
     * @return Random unsigned char
     */
    static unsigned char RandomByte();
    
    /**
     * @brief Generate random DWORD (32-bit unsigned integer)
     * @return Random uint32_t value
     */
    static uint32_t RandomDword();
};

} // namespace Core
} // namespace UchihaSpoofer
