#include <SPI.h>
#include "TFT_Driver_ST7735.h"

namespace TFT_Runtime {

TFT_Driver_ST7735::TFT_Driver_ST7735(Config& config) : TFT_Driver_Base(config) {
    _width = ST7735_TFTWIDTH;
    _height = ST7735_TFTHEIGHT;
}

void TFT_Driver_ST7735::init() {
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

    writeCommand(ST7735_SWRESET);    // Software reset
    delay(150);                      // Wait for reset to complete

    writeCommand(ST7735_SLPOUT);     // Out of sleep mode
    delay(120);

    writeCommand(ST7735_FRMCTR1);    // Frame rate control
    writeData(0x01);                 // Normal mode
    writeData(0x2C);
    writeData(0x2D);

    writeCommand(ST7735_FRMCTR2);    // Frame rate control (idle mode)
    writeData(0x01);
    writeData(0x2C);
    writeData(0x2D);

    writeCommand(ST7735_FRMCTR3);    // Frame rate control (partial mode)
    writeData(0x01);
    writeData(0x2C);
    writeData(0x2D);
    writeData(0x01);
    writeData(0x2C);
    writeData(0x2D);

    writeCommand(ST7735_INVCTR);     // Display inversion control
    writeData(0x07);

    writeCommand(ST7735_PWCTR1);     // Power control
    writeData(0xA2);
    writeData(0x02);
    writeData(0x84);

    writeCommand(ST7735_PWCTR2);     // Power control
    writeData(0xC5);

    writeCommand(ST7735_PWCTR3);     // Power control
    writeData(0x0A);
    writeData(0x00);

    writeCommand(ST7735_PWCTR4);     // Power control
    writeData(0x8A);
    writeData(0x2A);

    writeCommand(ST7735_PWCTR5);     // Power control
    writeData(0x8A);
    writeData(0xEE);

    writeCommand(ST7735_VMCTR1);     // Power control
    writeData(0x0E);

    writeCommand(ST7735_MADCTL);     // Memory Access Control
    writeData(0x48);                 // Default rotation (0)

    writeCommand(ST7735_COLMOD);     // Interface pixel format
    writeData(0x05);                 // 16-bit color

    // Gamma sequence
    writeCommand(ST7735_GMCTRP1);
    writeData(0x0F);
    writeData(0x1A);
    writeData(0x0F);
    writeData(0x18);
    writeData(0x2F);
    writeData(0x28);
    writeData(0x20);
    writeData(0x22);
    writeData(0x1F);
    writeData(0x1B);
    writeData(0x23);
    writeData(0x37);
    writeData(0x00);
    writeData(0x07);
    writeData(0x02);
    writeData(0x10);

    writeCommand(ST7735_GMCTRN1);
    writeData(0x0F);
    writeData(0x1B);
    writeData(0x0F);
    writeData(0x17);
    writeData(0x33);
    writeData(0x2C);
    writeData(0x29);
    writeData(0x2E);
    writeData(0x30);
    writeData(0x30);
    writeData(0x39);
    writeData(0x3F);
    writeData(0x00);
    writeData(0x07);
    writeData(0x03);
    writeData(0x10);

    writeCommand(ST7735_DISPON);     // Display on
    delay(100);
}

void TFT_Driver_ST7735::writeCommand(uint8_t cmd) {
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

void TFT_Driver_ST7735::writeData(uint8_t data) {
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

void TFT_Driver_ST7735::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_ST7735::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(ST7735_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x48 : 0x40);
            _width = ST7735_TFTWIDTH;
            _height = ST7735_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x28 : 0x20);
            _width = ST7735_TFTHEIGHT;
            _height = ST7735_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0x88 : 0x80);
            _width = ST7735_TFTWIDTH;
            _height = ST7735_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xE8 : 0xE0);
            _width = ST7735_TFTHEIGHT;
            _height = ST7735_TFTWIDTH;
            break;
    }
}

void TFT_Driver_ST7735::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? ST7735_INVON : ST7735_INVOFF);
}

} // namespace TFT_Runtime
