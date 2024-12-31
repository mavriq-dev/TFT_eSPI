#include <SPI.h>
#include "TFT_Driver_ST7796.h"

namespace TFT_Runtime {

TFT_Driver_ST7796::TFT_Driver_ST7796(Config& config) : TFT_Driver_Base(config) {
    _width = ST7796_TFTWIDTH;
    _height = ST7796_TFTHEIGHT;
}

void TFT_Driver_ST7796::init() {
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

    writeCommand(ST7796_SWRESET);    // Software reset
    delay(120);                      // Wait for reset to complete

    writeCommand(ST7796_SLPOUT);     // Exit Sleep
    delay(120);

    // Command Set Control
    writeCommand(0xF0);
    writeData(0xC3);
    writeCommand(0xF0);
    writeData(0x96);

    // Interface Pixel Format
    writeCommand(ST7796_PIXFMT);
    writeData(0x55);                 // 16-bit color

    // Memory Data Access Control
    writeCommand(ST7796_MADCTL);
    writeData(0x48);                 // Default rotation (0)

    // Display Function Control
    writeCommand(ST7796_DFUNCTR);
    writeData(0x80);                 // Display Function Control
    writeData(0x02);                 // Source Output Scan from S1 to S960, Gate Scan from G1 to G480
    writeData(0x3B);                 // LCD Drive Line = 8*(59+1)

    // Display Inversion Control
    writeCommand(ST7796_INVCTR);
    writeData(0x00);                 // Line inversion

    // Frame Rate Control
    writeCommand(ST7796_FRMCTR1);
    writeData(0x80);                 // Frame rate = 60Hz
    writeData(0x10);

    // Power Control 1
    writeCommand(ST7796_PWCTR1);
    writeData(0x0E);
    writeData(0x0E);

    // Power Control 2
    writeCommand(ST7796_PWCTR2);
    writeData(0x41);
    writeData(0x00);

    // Power Control 3
    writeCommand(ST7796_PWCTR3);
    writeData(0x33);

    // VCOM Control
    writeCommand(ST7796_VMCTR);
    writeData(0x00);
    writeData(0x75);

    // Positive Gamma Control
    writeCommand(ST7796_PGAMMAC);
    writeData(0xF0);
    writeData(0x09);
    writeData(0x0B);
    writeData(0x06);
    writeData(0x04);
    writeData(0x15);
    writeData(0x2F);
    writeData(0x54);
    writeData(0x42);
    writeData(0x3C);
    writeData(0x17);
    writeData(0x14);
    writeData(0x18);
    writeData(0x1B);

    // Negative Gamma Control
    writeCommand(ST7796_NGAMMAC);
    writeData(0xE0);
    writeData(0x09);
    writeData(0x0B);
    writeData(0x06);
    writeData(0x04);
    writeData(0x03);
    writeData(0x2B);
    writeData(0x43);
    writeData(0x42);
    writeData(0x3B);
    writeData(0x16);
    writeData(0x14);
    writeData(0x17);
    writeData(0x1B);

    // Enable Tear Effect Line
    writeCommand(0x35);
    writeData(0x00);

    // Display Brightness Value
    writeCommand(ST7796_WRDISBV);
    writeData(0xFF);                 // Maximum Brightness

    // Display Control
    writeCommand(ST7796_WRCTRLD);
    writeData(0x2C);                 // Enable Brightness Control

    writeCommand(ST7796_DISPON);     // Display on
    delay(120);
}

void TFT_Driver_ST7796::writeCommand(uint8_t cmd) {
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

void TFT_Driver_ST7796::writeData(uint8_t data) {
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

void TFT_Driver_ST7796::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_ST7796::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(ST7796_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x48 : 0x40);
            _width = ST7796_TFTWIDTH;
            _height = ST7796_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x28 : 0x20);
            _width = ST7796_TFTHEIGHT;
            _height = ST7796_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0x88 : 0x80);
            _width = ST7796_TFTWIDTH;
            _height = ST7796_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xE8 : 0xE0);
            _width = ST7796_TFTHEIGHT;
            _height = ST7796_TFTWIDTH;
            break;
    }
}

void TFT_Driver_ST7796::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? ST7796_INVON : ST7796_INVOFF);
}

} // namespace TFT_Runtime
