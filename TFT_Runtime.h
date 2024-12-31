#ifndef _TFT_RUNTIME_H_
#define _TFT_RUNTIME_H_

#include <cstdint>  // For fixed-width integer types

namespace TFT_Runtime {

// Display driver types
enum class DisplayDriver {
    ILI9341,
    ST7735,
    ST7789,
    ILI9486,
    ILI9488,
    HX8357D,
    RM68140,
    ST7796,
    SSD1351,
    Unknown
};

// Interface modes
enum class InterfaceMode {
    SPI,
    PARALLEL_8BIT
};

// Color order options
enum class ColorOrder {
    RGB,
    BGR
};

// SPI configuration
struct SPIConfig {
    int8_t sck_pin = -1;
    int8_t miso_pin = -1;
    int8_t mosi_pin = -1;
    int8_t cs_pin = -1;
    int8_t dc_pin = -1;
    uint32_t frequency = 40000000;  // Default to 40MHz
    uint8_t spi_mode = 0;
    bool three_wire = false;
};

// Parallel bus configuration
struct ParallelConfig {
    int8_t wr_pin = -1;
    int8_t rd_pin = -1;
    int8_t cs_pin = -1;
    int8_t dc_pin = -1;
    int8_t d0_pin = -1;
    int8_t d1_pin = -1;
    int8_t d2_pin = -1;
    int8_t d3_pin = -1;
    int8_t d4_pin = -1;
    int8_t d5_pin = -1;
    int8_t d6_pin = -1;
    int8_t d7_pin = -1;
};

// Main runtime configuration structure
struct Config {
    DisplayDriver driver = DisplayDriver::ILI9341;
    InterfaceMode interface = InterfaceMode::SPI;
    ColorOrder color_order = ColorOrder::RGB;
    
    // Display properties
    uint16_t width = 240;
    uint16_t height = 320;
    int8_t rst_pin = -1;
    bool invert_colors = false;
    uint8_t rotation = 0;
    
    // Interface specific configuration
    SPIConfig spi;
    ParallelConfig parallel;
    
    // Constructor with defaults
    Config() = default;
};

} // namespace TFT_Runtime

#endif // _TFT_RUNTIME_H_
