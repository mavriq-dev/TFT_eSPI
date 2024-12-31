#include <SPI.h>
#include "TFT_Driver_ILI9486.h"

namespace TFT_Runtime {

TFT_Driver_ILI9486::TFT_Driver_ILI9486(Config& config) : TFT_Driver_Base(config) {
    _width = ILI9486_TFTWIDTH;
    _height = ILI9486_TFTHEIGHT;
}

void TFT_Driver_ILI9486::init() {
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

    writeCommand(ILI9486_SWRESET);    // Software reset
    delay(150);                       // Wait for reset to complete

    writeCommand(ILI9486_SLPOUT);     // Exit Sleep
    delay(50);

    writeCommand(ILI9486_PIXFMT);     // Set pixel format
    writeData(0x55);                  // 16-bit color

    // Power Control 1
    writeCommand(ILI9486_PWCTR1);
    writeData(0x0E);
    writeData(0x0E);

    // Power Control 2
    writeCommand(ILI9486_PWCTR2);
    writeData(0x41);
    writeData(0x00);

    // Power Control 3
    writeCommand(ILI9486_PWCTR3);
    writeData(0x00);
    writeData(0x22);
    writeData(0x22);

    // VCOM Control
    writeCommand(ILI9486_VMCTR1);
    writeData(0x00);
    writeData(0x35);
    writeData(0x80);

    writeCommand(ILI9486_MADCTL);    // Memory Access Control
    writeData(0x48);                 // Default rotation (0)

    // Interface Control
    writeCommand(0xF6);
    writeData(0x01);
    writeData(0x00);
    writeData(0x06);

    // Positive Gamma Control
    writeCommand(ILI9486_PGAMMAC);
    writeData(0x0F);
    writeData(0x1F);
    writeData(0x1C);
    writeData(0x0C);
    writeData(0x0F);
    writeData(0x08);
    writeData(0x48);
    writeData(0x98);
    writeData(0x37);
    writeData(0x0A);
    writeData(0x13);
    writeData(0x04);
    writeData(0x11);
    writeData(0x0D);
    writeData(0x00);

    // Negative Gamma Control
    writeCommand(ILI9486_NGAMMAC);
    writeData(0x0F);
    writeData(0x32);
    writeData(0x2E);
    writeData(0x0B);
    writeData(0x0D);
    writeData(0x05);
    writeData(0x47);
    writeData(0x75);
    writeData(0x37);
    writeData(0x06);
    writeData(0x10);
    writeData(0x03);
    writeData(0x24);
    writeData(0x20);
    writeData(0x00);

    writeCommand(ILI9486_DISPON);     // Display ON
    delay(120);
}

void TFT_Driver_ILI9486::writeCommand(uint8_t cmd) {
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

void TFT_Driver_ILI9486::writeData(uint8_t data) {
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

void TFT_Driver_ILI9486::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_ILI9486::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(ILI9486_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x48 : 0x40);
            _width = ILI9486_TFTWIDTH;
            _height = ILI9486_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x28 : 0x20);
            _width = ILI9486_TFTHEIGHT;
            _height = ILI9486_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0x88 : 0x80);
            _width = ILI9486_TFTWIDTH;
            _height = ILI9486_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xE8 : 0xE0);
            _width = ILI9486_TFTHEIGHT;
            _height = ILI9486_TFTWIDTH;
            break;
    }
}

void TFT_Driver_ILI9486::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? ILI9486_INVON : ILI9486_INVOFF);
}

} // namespace TFT_Runtime
