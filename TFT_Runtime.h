/***************************************************
  TFT_eSPI Runtime Configuration
  
  This file defines the runtime configuration structures
  and interfaces for the TFT_eSPI library.
  
  Created: December 2024
 ****************************************************/

#ifndef _TFT_RUNTIME_H_
#define _TFT_RUNTIME_H_

#include <Arduino.h>

namespace TFT_Runtime {

// Display driver types with specific model variants
enum class DisplayDriver : uint8_t {
    NONE = 0,
    ILI9341,
    ILI9341_2,    // Alternative initialization
    ILI9342,
    ST7735,
    ST7789,
    ST7789_2,     // Alternative initialization
    ST7796,
    ILI9163,
    ILI9486,
    ILI9488,
    HX8357B,
    HX8357C,
    HX8357D,
    S6D02A1,
    RM68120,
    RM68140,
    SSD1351,
    SSD1963,
    GC9A01,
    ILI9225
};

// ST7735 specific variant types
enum class ST7735Variant : uint8_t {
    INITR_GREENTAB = 0,
    INITR_REDTAB = 1,
    INITR_BLACKTAB = 2,    // Display with no offsets
    INITR_GREENTAB2 = 3,   // Use if you get random pixels on two edges
    INITR_GREENTAB3 = 4,   // Use if you get random pixels on edge(s) of 128x128
    INITR_GREENTAB128 = 5, // For 128x128 displays
    INITR_GREENTAB160x80 = 6,
    INITR_REDTAB160x80 = 7,
    INITR_ROBOTLCD = 8,
    INITB = 0xB
};

// Interface types
enum class InterfaceMode : uint8_t {
    NONE = 0,
    SPI,           // Standard SPI
    PARALLEL_8BIT, // 8-bit parallel
    PARALLEL_16BIT // 16-bit parallel
};

// Memory Address Control (MADCTL) bits
struct MADCtl {
    bool rowAddressOrder = false;    // MY: 0=top to bottom, 1=bottom to top
    bool colAddressOrder = false;    // MX: 0=left to right, 1=right to left
    bool rowColExchange = false;     // MV: 0=normal, 1=exchange
    bool vertRefreshOrder = false;   // ML: 0=refresh top to bottom, 1=refresh bottom to top
    bool colorOrder = false;         // RGB: 0=RGB, 1=BGR
    bool horizRefreshOrder = false;  // MH: 0=refresh left to right, 1=refresh right to left
};

// Display offset and orientation
struct DisplayGeometry {
    int16_t colStart = 0;       // Column offset
    int16_t rowStart = 0;       // Row offset
    uint16_t width = 240;       // Display width in default orientation
    uint16_t height = 320;      // Display height in default orientation
    bool invertDisplay = false;  // Invert display colors
    MADCtl madctl;              // Memory Address Control settings
    uint8_t rotation = 0;       // Current rotation (0-7)
    int16_t xOffset = 0;        // Additional x offset for specific variants
    int16_t yOffset = 0;        // Additional y offset for specific variants
};

// Color configuration
struct ColorConfig {
    bool rgbOrder = true;           // true = RGB, false = BGR
    uint8_t bitsPerChannel = 5;     // Bits per color channel (usually 5 or 6)
    bool invertColors = false;      // Invert all colors
    uint32_t backgroundColor = 0;   // Default background color
};

// Interface timing
struct InterfaceTiming {
    uint8_t rd_pulse = 0;    // Read pulse width
    uint8_t wr_pulse = 0;    // Write pulse width
    uint8_t setup_time = 0;  // Setup time
    uint8_t hold_time = 0;   // Hold time
    uint8_t spi_mode = 0;    // SPI mode (0-3)
    uint32_t freq_write = 40000000;  // Write frequency
    uint32_t freq_read = 20000000;   // Read frequency
    bool spi_read_mode = 0;  // Some displays need different SPI mode for reads
};

// Main configuration structure
struct Configuration {
    // Basic display identification
    DisplayDriver driver = DisplayDriver::NONE;
    
    // Display geometry and orientation
    DisplayGeometry geometry;
    
    // Color configuration
    ColorConfig color;
    
    // Interface configuration
    InterfaceMode interface = InterfaceMode::SPI;
    InterfaceTiming timing;
    
    // Driver-specific configurations
    union {
        struct {
            ST7735Variant variant;
            uint8_t tabcolor;
            bool blacktab_colorfix;  // Special color fix for some black tabs
        } st7735;
        
        struct {
            bool alt_mode;  // Some ILI9341 need alternative initialization
            bool use_reset; // Some don't need/want reset
        } ili9341;
        
        struct {
            uint8_t rgbmode;  // Different RGB modes for HX8357
        } hx8357;
        
        // Add other driver-specific configurations as needed
    } driver_config;
    
    // Pin assignments (-1 = not used)
    struct {
        int8_t mosi = -1;    // SPI MOSI pin
        int8_t miso = -1;    // SPI MISO pin
        int8_t sclk = -1;    // SPI SCLK pin
        int8_t cs = -1;      // Chip select pin
        int8_t dc = -1;      // Data/Command pin
        int8_t rst = -1;     // Reset pin
        int8_t wr = -1;      // Write pin (parallel)
        int8_t rd = -1;      // Read pin (parallel)
        int8_t d0_d15[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
        int8_t backlight = -1;  // Backlight control pin
    } pins;
    
    // Performance options
    struct {
        bool use_dma = false;          // Use DMA transfers if available
        bool swap_bytes = false;       // Swap bytes in 16-bit transfers
        bool use_buf = false;          // Use transaction buffers
        size_t buf_size = 0;          // Buffer size if used
        bool busy_check = true;       // Check if SPI is busy before transfer
    } performance;
    
    // Constructor with defaults
    Configuration() = default;
};

// Base class for display drivers
class Driver {
public:
    virtual ~Driver() = default;
    
    // Initialization
    virtual bool init() = 0;
    virtual void reset() = 0;
    
    // Basic operations
    virtual void writeCommand(uint8_t cmd) = 0;
    virtual void writeData(uint8_t data) = 0;
    virtual void writeData16(uint16_t data) = 0;
    virtual uint8_t readData() = 0;
    
    // Block operations
    virtual void writeBlock(const uint16_t* data, uint32_t len) = 0;
    virtual void readBlock(uint16_t* data, uint32_t len) = 0;
    
    // Display control
    virtual void setRotation(uint8_t r) = 0;
    virtual void invertDisplay(bool i) = 0;
    virtual void displayOn() = 0;
    virtual void displayOff() = 0;
    
    // Window operations
    virtual void setWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1) = 0;
    
    // Configuration validation
    static bool validateConfig(const Configuration& config) {
        // Common validation for all drivers
        if (config.interface == InterfaceMode::NONE) {
            return false;
        }
        
        // Pin validation for SPI mode
        if (config.interface == InterfaceMode::SPI) {
            if (config.pins.mosi < 0 || config.pins.sclk < 0) {
                return false;
            }
        }
        
        // Pin validation for parallel mode
        if (config.interface == InterfaceMode::PARALLEL_8BIT ||
            config.interface == InterfaceMode::PARALLEL_16BIT) {
            if (config.pins.wr < 0) {
                return false;
            }
            
            // Check data pins
            int requiredPins = (config.interface == InterfaceMode::PARALLEL_8BIT) ? 8 : 16;
            for (int i = 0; i < requiredPins; i++) {
                if (config.pins.d0_d15[i] < 0) {
                    return false;
                }
            }
        }
        
        return true;
    }
    
    // Configuration access
    const Configuration& getConfig() const { return config; }
    
protected:
    Configuration config;
    
    explicit Driver(const Configuration& cfg) : config(cfg) {}
};

} // namespace TFT_Runtime

#endif // _TFT_RUNTIME_H_
