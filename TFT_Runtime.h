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
    ILI9481,
    HX8357D,
    RM68140,
    ST7796,
    SSD1351,
    GC9A01,
    R61581,
    GC9107,
    HX8357B,
    HX8357C,
    HX8369A,
    ILI9163,
    ILI9225,
    ILI9327,
    ILI9342,
    ILI9806,
    ILI9881C,
    NT35310,
    NT35510,
    NT35516,
    OTM8009A,
    R61529,
    RM68120,
    S6D02A1,
    SSD1963,
    ST7262,
    ST7701,
    ST7735R,
    ST7781,
    ST7789V2,
    ST7789V3,
    ST7796S,
    Unknown
};

// Interface modes
enum class InterfaceMode {
    SPI,
    PARALLEL_8BIT,
    PARALLEL_16BIT
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
    int8_t data_pins[16] = {-1};  // Support for up to 16 data pins
    bool is_16bit = false;        // Flag for 16-bit mode
    uint32_t write_delay = 0;     // Optional delay between writes (in nanoseconds)
    bool use_latch = false;       // Use latch for 16-bit mode
    int8_t latch_pin = -1;        // Latch pin for 16-bit mode
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
