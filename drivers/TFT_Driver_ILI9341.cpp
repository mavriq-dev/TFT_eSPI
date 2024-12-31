#include <SPI.h>
#include "drivers/TFT_Driver_ILI9341.h"

namespace TFT_Runtime {

TFT_Driver_ILI9341::TFT_Driver_ILI9341(Config& config) : TFT_Driver_Base(config) {
    _width = ILI9341_TFTWIDTH;
    _height = ILI9341_TFTHEIGHT;
}

void TFT_Driver_ILI9341::init() {
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

    writeCommand(ILI9341_SWRESET);    // Software reset
    delay(150);                        // Wait for reset to complete

    writeCommand(ILI9341_DISPOFF);     // Display off

    writeCommand(ILI9341_PWCTR1);      // Power control
    writeData(0x23);                   // VRH[5:0]

    writeCommand(ILI9341_PWCTR2);      // Power control
    writeData(0x10);                   // SAP[2:0];BT[3:0]

    writeCommand(ILI9341_VMCTR1);      // VCM control
    writeData(0x3e);
    writeData(0x28);

    writeCommand(ILI9341_VMCTR2);      // VCM control2
    writeData(0x86);                   // --

    writeCommand(ILI9341_MADCTL);      // Memory Access Control
    writeData(0x48);                   // Default rotation (0)

    writeCommand(ILI9341_PIXFMT);
    writeData(0x55);                   // 16-bit color

    writeCommand(ILI9341_FRMCTR1);
    writeData(0x00);
    writeData(0x18);

    writeCommand(ILI9341_DFUNCTR);     // Display Function Control
    writeData(0x08);
    writeData(0x82);
    writeData(0x27);

    writeCommand(ILI9341_SLPOUT);      // Exit Sleep
    delay(120);
    writeCommand(ILI9341_DISPON);      // Display on
    delay(120);
}

void TFT_Driver_ILI9341::writeCommand(uint8_t cmd) {
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

void TFT_Driver_ILI9341::writeData(uint8_t data) {
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

void TFT_Driver_ILI9341::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_ILI9341::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(ILI9341_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x48 : 0x40);
            _width = ILI9341_TFTWIDTH;
            _height = ILI9341_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x28 : 0x20);
            _width = ILI9341_TFTHEIGHT;
            _height = ILI9341_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0x88 : 0x80);
            _width = ILI9341_TFTWIDTH;
            _height = ILI9341_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xE8 : 0xE0);
            _width = ILI9341_TFTHEIGHT;
            _height = ILI9341_TFTWIDTH;
            break;
    }
}

void TFT_Driver_ILI9341::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? ILI9341_INVON : ILI9341_INVOFF);
}

} // namespace TFT_Runtime
