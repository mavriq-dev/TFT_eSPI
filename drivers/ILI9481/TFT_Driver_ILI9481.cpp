#include <SPI.h>
#include "TFT_Driver_ILI9481.h"

namespace TFT_Runtime {

TFT_Driver_ILI9481::TFT_Driver_ILI9481(Config& config) : TFT_Driver_Base(config) {
    _width = ILI9481_TFTWIDTH;
    _height = ILI9481_TFTHEIGHT;
}

void TFT_Driver_ILI9481::init() {
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

    writeCommand(ILI9481_SWRESET);    // Software reset
    delay(120);                       // Wait for reset to complete

    writeCommand(ILI9481_SLPOUT);     // Exit Sleep
    delay(60);

    // Power Setting
    initPowerSequence();
    delay(10);

    // VCOM Setting
    writeCommand(ILI9481_VMCTR1);     // VCOM Control
    writeData(0x00);
    writeData(0x58);
    writeData(0x80);
    writeData(0x00);

    // Interface Pixel Format
    writeCommand(ILI9481_PIXFMT);     // Set pixel format
    writeData(0x55);                  // 16-bit RGB565

    // Display Setting
    writeCommand(ILI9481_DFUNCTR);
    writeData(0x02);                  // All scan lines are displayed
    writeData(0x02);                  // Normal display mode
    writeData(0x3B);                  // Set GRAM write direction and BGR=1

    // Frame Rate Control
    writeCommand(ILI9481_FRMCTR1);    // Frame rate 70Hz
    writeData(0xA0);
    writeData(0x08);

    writeCommand(ILI9481_INVCTR);     // Display Inversion Control
    writeData(0x02);                  // 2-dot inversion

    // RGB Interface Setting
    writeCommand(ILI9481_RGBCTRL);
    writeData(0x00);                  // RGB interface setting
    writeData(0x02);                  // RGB 16-bit (565)
    writeData(0x3B);                  // Set RGB interface

    // Gamma Setting
    initGamma();
    delay(10);

    writeCommand(ILI9481_MADCTL);     // Memory Access Control
    writeData(0x48);                  // MX, BGR

    writeCommand(ILI9481_CASET);      // Column address set
    writeData(0x00);
    writeData(0x00);                  // Start 0
    writeData(0x01);
    writeData(0x3F);                  // End 319

    writeCommand(ILI9481_PASET);      // Page address set
    writeData(0x00);
    writeData(0x00);                  // Start 0
    writeData(0x01);
    writeData(0xDF);                  // End 479

    writeCommand(ILI9481_NORON);      // Normal Display Mode ON
    delay(10);

    writeCommand(ILI9481_DISPON);     // Display ON
    delay(120);
}

void TFT_Driver_ILI9481::initPowerSequence() {
    writeCommand(ILI9481_PWCTR1);     // Power Control 1
    writeData(0x07);
    writeData(0x42);
    writeData(0x18);

    writeCommand(ILI9481_PWCTR2);     // Power Control 2
    writeData(0x00);
    writeData(0x07);
    writeData(0x10);

    writeCommand(ILI9481_PWCTR3);     // Power Control 3
    writeData(0x01);
    writeData(0x02);

    writeCommand(ILI9481_PWCTR4);     // Power Control 4
    writeData(0x10);
    writeData(0x3B);
    writeData(0x00);
    writeData(0x02);
    writeData(0x01);

    writeCommand(ILI9481_PWCTR5);     // Power Control 5
    writeData(0x03);
    writeData(0x70);
    writeData(0x50);
    writeData(0x3E);
    writeData(0x08);
}

void TFT_Driver_ILI9481::initGamma() {
    writeCommand(ILI9481_GMCTRP1);    // Positive Gamma Control
    writeData(0x00);
    writeData(0x32);
    writeData(0x36);
    writeData(0x45);
    writeData(0x06);
    writeData(0x16);
    writeData(0x37);
    writeData(0x75);
    writeData(0x77);
    writeData(0x54);
    writeData(0x0C);
    writeData(0x00);

    writeCommand(ILI9481_GMCTRN1);    // Negative Gamma Control
    writeData(0x00);
    writeData(0x32);
    writeData(0x36);
    writeData(0x45);
    writeData(0x06);
    writeData(0x16);
    writeData(0x37);
    writeData(0x75);
    writeData(0x77);
    writeData(0x54);
    writeData(0x0C);
    writeData(0x00);
}

void TFT_Driver_ILI9481::writeCommand(uint8_t cmd) {
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

void TFT_Driver_ILI9481::writeData(uint8_t data) {
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

void TFT_Driver_ILI9481::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_ILI9481::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(ILI9481_MADCTL);
    
    switch (_rotation) {
        case 0: // Portrait
            writeData(config.color_order == ColorOrder::RGB ? 0x48 : 0x40);
            _width = ILI9481_TFTWIDTH;
            _height = ILI9481_TFTHEIGHT;
            break;
        case 1: // Landscape
            writeData(config.color_order == ColorOrder::RGB ? 0x28 : 0x20);
            _width = ILI9481_TFTHEIGHT;
            _height = ILI9481_TFTWIDTH;
            break;
        case 2: // Portrait inverted
            writeData(config.color_order == ColorOrder::RGB ? 0x88 : 0x80);
            _width = ILI9481_TFTWIDTH;
            _height = ILI9481_TFTHEIGHT;
            break;
        case 3: // Landscape inverted
            writeData(config.color_order == ColorOrder::RGB ? 0xE8 : 0xE0);
            _width = ILI9481_TFTHEIGHT;
            _height = ILI9481_TFTWIDTH;
            break;
    }
}

void TFT_Driver_ILI9481::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? ILI9481_INVON : ILI9481_INVOFF);
}

void TFT_Driver_ILI9481::setGamma(uint8_t gamma) {
    writeCommand(ILI9481_GAMMASET);
    writeData(gamma);
}

void TFT_Driver_ILI9481::setVCOM(uint8_t value) {
    writeCommand(ILI9481_VMCTR1);
    writeData(0x00);
    writeData(value);
}

void TFT_Driver_ILI9481::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(ILI9481_CASET);      // Column addr set
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);             // Start col
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);             // End col

    writeCommand(ILI9481_PASET);      // Row addr set
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);             // Start row
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);             // End row

    writeCommand(ILI9481_RAMWR);      // Write to RAM
}

} // namespace TFT_Runtime
