#include <SPI.h>
#include "TFT_Driver_SSD1963.h"

namespace TFT_Runtime {

TFT_Driver_SSD1963::TFT_Driver_SSD1963(Config& config) : TFT_Driver_Base(config) {
    _width = SSD1963_TFTWIDTH;
    _height = SSD1963_TFTHEIGHT;
}

void TFT_Driver_SSD1963::init() {
    // Hardware reset
    if (config.rst_pin >= 0) {
        pinMode(config.rst_pin, OUTPUT);
        digitalWrite(config.rst_pin, HIGH);
        delay(5);
        digitalWrite(config.rst_pin, LOW);
        delay(20);
        digitalWrite(config.rst_pin, HIGH);
        delay(150);
    }

    writeCommand(SSD1963_SWRESET);    // Software reset
    delay(120);                       // Wait for reset to complete

    // Set PLL with OSC = 10MHz (hardware), multiplier N = 35, divider M = 2
    // PLL frequency = OSC * (N + 1) / (M + 1) = 10MHz * 36 / 3 = 120MHz
    writeCommand(SSD1963_SETPLL);
    writeData(0x01);                  // Enable PLL
    delay(2);
    writeCommand(SSD1963_SETPLL);
    writeData(0x03);                  // Use PLL
    delay(2);

    writeCommand(SSD1963_SETPLLMN);
    writeData(35);                    // Multiplier N = 35 (0-63)
    writeData(2);                     // Divider M = 2 (0-63)
    delay(2);

    // Configure LCD panel mode
    writeCommand(SSD1963_SETLCDMODE);
    writeData(0x0C);                  // 24-bit TFT panel, disable dithering
    writeData(0x00);                  // TFT mode
    writeData(0x03);                  // Set horizontal size = 800-1 = 799
    writeData(0x1F);
    writeData(0x01);                  // Set vertical size = 480-1 = 479
    writeData(0xDF);
    writeData(0x00);                  // RGB interface settings

    // Set pixel data interface
    writeCommand(SSD1963_PIXFMT);
    writeData(0x55);                  // 16-bit/pixel

    // Configure display timing
    writeCommand(0xE6);               // Set pixel clock frequency
    writeData(0x01);
    writeData(0x1F);
    writeData(0xFF);

    writeCommand(0xB4);               // Set horizontal period
    writeData(0x02);                  // HSYNC total
    writeData(0x13);
    writeData(0x00);                  // HSYNC display period start
    writeData(0x08);
    writeData(0x00);                  // HSYNC display period end
    writeData(0x02);
    writeData(0x00);                  // HSYNC back porch

    writeCommand(0xB6);               // Set vertical period
    writeData(0x01);                  // VSYNC total
    writeData(0x20);
    writeData(0x00);                  // VSYNC display period start
    writeData(0x04);
    writeData(0x00);                  // VSYNC display period end
    writeData(0x00);
    writeData(0x00);                  // VSYNC back porch

    // Set GPIO configuration
    writeCommand(SSD1963_SETGPIO);
    writeData(0x0F);                  // GPIO3=input, GPIO[2:0]=output
    writeData(0x01);                  // GPIO0 normal

    // Set PWM configuration for backlight control
    writeCommand(SSD1963_SETPWM);
    writeData(0x06);                  // PWM frequency = PLL clock / (256 * (1 + value))
    writeData(0xFF);                  // PWM duty cycle = value / 256
    writeData(0x01);                  // PWM enabled and controlled by host
    writeData(0xFF);
    writeData(0x00);
    writeData(0x00);

    // Set VCOM voltage
    writeCommand(SSD1963_SETVCOM);
    writeData(0x00);
    writeData(0x40);                  // VCOM = -1.6V

    // Set Panel Characteristics
    writeCommand(SSD1963_SETPANEL);
    writeData(0x09);                  // Panel settings

    // Set gamma curve
    writeCommand(SSD1963_SETGAMMA);
    writeData(0x00);
    writeData(0x15);
    writeData(0x00);
    writeData(0x22);
    writeData(0x00);
    writeData(0x0F);
    writeData(0x00);
    writeData(0x1C);
    writeData(0x00);
    writeData(0x18);
    writeData(0x00);
    writeData(0x25);
    writeData(0x00);
    writeData(0x2A);
    writeData(0x00);
    writeData(0x2B);

    writeCommand(SSD1963_SLPOUT);     // Exit Sleep
    delay(120);

    writeCommand(SSD1963_DISPON);     // Display on
    delay(20);

    // Set initial backlight level
    setBacklight(255);                // Full brightness
}

void TFT_Driver_SSD1963::writeCommand(uint8_t cmd) {
    if (config.interface == InterfaceMode::SPI) {
        digitalWrite(config.spi.dc_pin, LOW);
        if (config.spi.cs_pin >= 0) digitalWrite(config.spi.cs_pin, LOW);
        SPI.transfer(cmd);
        if (config.spi.cs_pin >= 0) digitalWrite(config.spi.cs_pin, HIGH);
    } else {
        // Parallel 8-bit implementation
        digitalWrite(config.parallel.dc_pin, LOW);
        if (config.parallel.cs_pin >= 0) digitalWrite(config.parallel.cs_pin, LOW);
        // Write to parallel bus - implementation depends on hardware setup
        if (config.parallel.cs_pin >= 0) digitalWrite(config.parallel.cs_pin, HIGH);
    }
}

void TFT_Driver_SSD1963::writeData(uint8_t data) {
    if (config.interface == InterfaceMode::SPI) {
        digitalWrite(config.spi.dc_pin, HIGH);
        if (config.spi.cs_pin >= 0) digitalWrite(config.spi.cs_pin, LOW);
        SPI.transfer(data);
        if (config.spi.cs_pin >= 0) digitalWrite(config.spi.cs_pin, HIGH);
    } else {
        // Parallel 8-bit implementation
        digitalWrite(config.parallel.dc_pin, HIGH);
        if (config.parallel.cs_pin >= 0) digitalWrite(config.parallel.cs_pin, LOW);
        // Write to parallel bus - implementation depends on hardware setup
        if (config.parallel.cs_pin >= 0) digitalWrite(config.parallel.cs_pin, HIGH);
    }
}

void TFT_Driver_SSD1963::writeBlock(uint16_t* data, uint32_t len) {
    if (config.interface == InterfaceMode::SPI) {
        digitalWrite(config.spi.dc_pin, HIGH);
        if (config.spi.cs_pin >= 0) digitalWrite(config.spi.cs_pin, LOW);
        
        // Use hardware SPI for better performance
        for (uint32_t i = 0; i < len; i++) {
            SPI.transfer16(data[i]);
        }
        
        if (config.spi.cs_pin >= 0) digitalWrite(config.spi.cs_pin, HIGH);
    } else {
        // Parallel 8-bit implementation
        digitalWrite(config.parallel.dc_pin, HIGH);
        if (config.parallel.cs_pin >= 0) digitalWrite(config.parallel.cs_pin, LOW);
        
        // Write to parallel bus - implementation depends on hardware setup
        for (uint32_t i = 0; i < len; i++) {
            // Write high byte then low byte
            // Implementation depends on hardware setup
        }
        
        if (config.parallel.cs_pin >= 0) digitalWrite(config.parallel.cs_pin, HIGH);
    }
}

void TFT_Driver_SSD1963::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(SSD1963_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x48 : 0x40);
            _width = SSD1963_TFTWIDTH;
            _height = SSD1963_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x28 : 0x20);
            _width = SSD1963_TFTHEIGHT;
            _height = SSD1963_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0x88 : 0x80);
            _width = SSD1963_TFTWIDTH;
            _height = SSD1963_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xE8 : 0xE0);
            _width = SSD1963_TFTHEIGHT;
            _height = SSD1963_TFTWIDTH;
            break;
    }
}

void TFT_Driver_SSD1963::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? SSD1963_INVON : SSD1963_INVOFF);
}

void TFT_Driver_SSD1963::setPLL(uint8_t mult, uint8_t div) {
    writeCommand(SSD1963_SETPLL);
    writeData(0x01);                  // Enable PLL
    delay(2);
    writeCommand(SSD1963_SETPLL);
    writeData(0x03);                  // Use PLL
    delay(2);

    writeCommand(SSD1963_SETPLLMN);
    writeData(mult);                  // Multiplier N (0-63)
    writeData(div);                   // Divider M (0-63)
    delay(2);
}

void TFT_Driver_SSD1963::setLCDMode(uint8_t hori, uint8_t vert, uint8_t pol) {
    writeCommand(SSD1963_SETLCDMODE);
    writeData(0x0C);                  // 24-bit TFT panel
    writeData(0x00);                  // TFT mode
    writeData(hori);                  // Horizontal panel size
    writeData(0x00);
    writeData(vert);                  // Vertical panel size
    writeData(0x00);
    writeData(pol);                   // RGB interface polarity
}

void TFT_Driver_SSD1963::setBacklight(uint8_t value) {
    writeCommand(SSD1963_SETPWM);
    writeData(0x06);                  // PWM frequency
    writeData(value);                 // PWM duty cycle
    writeData(0x01);                  // PWM enabled and controlled by host
    writeData(0xFF);
    writeData(0x00);
    writeData(0x00);
}

} // namespace TFT_Runtime
