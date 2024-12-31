#include <SPI.h>
#include "TFT_Driver_ILI9488.h"

namespace TFT_Runtime {

TFT_Driver_ILI9488::TFT_Driver_ILI9488(Config& config) : TFT_Driver_Base(config) {
    _width = ILI9488_TFTWIDTH;
    _height = ILI9488_TFTHEIGHT;
}

void TFT_Driver_ILI9488::init() {
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

    writeCommand(ILI9488_SWRESET);    // Software reset
    delay(120);                       // Wait for reset to complete

    writeCommand(ILI9488_SLPOUT);     // Exit Sleep
    delay(120);

    writeCommand(ILI9488_MADCTL);     // Memory Access Control
    writeData(0x48);                  // Default rotation (0)

    writeCommand(ILI9488_PIXFMT);     // Set pixel format
    writeData(0x55);                  // 16-bit RGB565 color

    // Power Control 1
    writeCommand(ILI9488_PWCTR1);
    writeData(0x17);                  // Vreg1out
    writeData(0x15);                  // Verg2out

    // Power Control 2
    writeCommand(ILI9488_PWCTR2);
    writeData(0x41);                  // VGH,VGL

    // Power Control 3
    writeCommand(ILI9488_PWCTR3);
    writeData(0x00);
    writeData(0x12);
    writeData(0x80);

    // Interface Mode Control
    writeCommand(ILI9488_ENTRYMODE);
    writeData(0x40);                  // RGB interface

    // Frame Rate Control
    writeCommand(ILI9488_FRMCTR1);
    writeData(0xA0);                  // 60Hz

    // Display Inversion Control
    writeCommand(ILI9488_INVCTR);
    writeData(0x02);                  // 2-dot

    // Display Function Control
    writeCommand(ILI9488_DFUNCTR);
    writeData(0x02);                  // MCU interface
    writeData(0x02);                  // Source,Gate scan direction

    // Adjust Control 3
    writeCommand(0xF7);
    writeData(0xA9);
    writeData(0x51);
    writeData(0x2C);
    writeData(0x82);

    // Positive Gamma Control
    writeCommand(ILI9488_PGAMMAC);
    writeData(0x00);
    writeData(0x03);
    writeData(0x09);
    writeData(0x08);
    writeData(0x16);
    writeData(0x0A);
    writeData(0x3F);
    writeData(0x78);
    writeData(0x4C);
    writeData(0x09);
    writeData(0x0A);
    writeData(0x08);
    writeData(0x16);
    writeData(0x1A);
    writeData(0x0F);

    // Negative Gamma Control
    writeCommand(ILI9488_NGAMMAC);
    writeData(0x00);
    writeData(0x16);
    writeData(0x19);
    writeData(0x03);
    writeData(0x0F);
    writeData(0x05);
    writeData(0x32);
    writeData(0x45);
    writeData(0x46);
    writeData(0x04);
    writeData(0x0E);
    writeData(0x0D);
    writeData(0x35);
    writeData(0x37);
    writeData(0x0F);

    writeCommand(ILI9488_DISPON);     // Display ON
    delay(120);
}

void TFT_Driver_ILI9488::writeCommand(uint8_t cmd) {
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

void TFT_Driver_ILI9488::writeData(uint8_t data) {
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

void TFT_Driver_ILI9488::writeBlock(uint16_t* data, uint32_t len) {
    if (config.interface == InterfaceMode::SPI) {
        digitalWrite(config.spi.dc_pin, HIGH);
        if (config.spi.cs_pin >= 0) digitalWrite(config.spi.cs_pin, LOW);
        
        // Use hardware SPI for better performance
        for (uint32_t i = 0; i < len; i++) {
            // ILI9488 uses 18-bit color in SPI mode, so we need to convert RGB565 to RGB666
            uint16_t color = data[i];
            uint8_t r = (color & 0xF800) >> 8;  // 5 bits red
            uint8_t g = (color & 0x07E0) >> 3;  // 6 bits green
            uint8_t b = (color & 0x001F) << 3;  // 5 bits blue
            
            // Extend to 6 bits per color
            r |= r >> 5;
            b |= b >> 5;
            
            SPI.transfer(r);
            SPI.transfer(g);
            SPI.transfer(b);
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

void TFT_Driver_ILI9488::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(ILI9488_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x48 : 0x40);
            _width = ILI9488_TFTWIDTH;
            _height = ILI9488_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x28 : 0x20);
            _width = ILI9488_TFTHEIGHT;
            _height = ILI9488_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0x88 : 0x80);
            _width = ILI9488_TFTWIDTH;
            _height = ILI9488_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xE8 : 0xE0);
            _width = ILI9488_TFTHEIGHT;
            _height = ILI9488_TFTWIDTH;
            break;
    }
}

void TFT_Driver_ILI9488::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? ILI9488_INVON : ILI9488_INVOFF);
}

} // namespace TFT_Runtime
