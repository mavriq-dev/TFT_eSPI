#include <SPI.h>
#include "TFT_Driver_ST7789.h"

namespace TFT_Runtime {

TFT_Driver_ST7789::TFT_Driver_ST7789(Config& config) : TFT_Driver_Base(config) {
    _width = ST7789_TFTWIDTH;
    _height = ST7789_TFTHEIGHT;
}

void TFT_Driver_ST7789::init() {
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

    writeCommand(ST7789_SWRESET);    // Software reset
    delay(150);                      // Wait for reset to complete

    writeCommand(ST7789_SLPOUT);     // Out of sleep mode
    delay(120);

    writeCommand(ST7789_COLMOD);     // Set color mode
    writeData(0x55);                 // 16-bit color

    writeCommand(ST7789_MADCTL);     // Memory Access Control
    writeData(0x00);                 // Default rotation (0)

    writeCommand(ST7789_PORCTRL);    // Porch control
    writeData(0x0C);
    writeData(0x0C);
    writeData(0x00);
    writeData(0x33);
    writeData(0x33);

    writeCommand(ST7789_GCTRL);      // Gate Control
    writeData(0x35);                 // Default VGH=13.26V, VGL=-10.43V

    writeCommand(ST7789_VCOMS);      // VCOM Setting
    writeData(0x19);                 // 0.725v

    writeCommand(ST7789_PWCTR1);     // Power Control 1
    writeData(0xA4);
    writeData(0xA1);

    // Positive Voltage Gamma Control
    writeCommand(ST7789_PVGAMCTRL);
    writeData(0xD0);
    writeData(0x04);
    writeData(0x0D);
    writeData(0x11);
    writeData(0x13);
    writeData(0x2B);
    writeData(0x3F);
    writeData(0x54);
    writeData(0x4C);
    writeData(0x18);
    writeData(0x0D);
    writeData(0x0B);
    writeData(0x1F);
    writeData(0x23);

    // Negative Voltage Gamma Control
    writeCommand(ST7789_NVGAMCTRL);
    writeData(0xD0);
    writeData(0x04);
    writeData(0x0C);
    writeData(0x11);
    writeData(0x13);
    writeData(0x2C);
    writeData(0x3F);
    writeData(0x44);
    writeData(0x51);
    writeData(0x2F);
    writeData(0x1F);
    writeData(0x1F);
    writeData(0x20);
    writeData(0x23);

    writeCommand(ST7789_INVON);      // Display Inversion On
    delay(10);

    writeCommand(ST7789_NORON);      // Normal Display Mode On
    delay(10);

    writeCommand(ST7789_DISPON);     // Display on
    delay(100);
}

void TFT_Driver_ST7789::writeCommand(uint8_t cmd) {
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

void TFT_Driver_ST7789::writeData(uint8_t data) {
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

void TFT_Driver_ST7789::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_ST7789::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(ST7789_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x00 : 0x08);
            _width = ST7789_TFTWIDTH;
            _height = ST7789_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x60 : 0x68);
            _width = ST7789_TFTHEIGHT;
            _height = ST7789_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0xC0 : 0xC8);
            _width = ST7789_TFTWIDTH;
            _height = ST7789_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xA0 : 0xA8);
            _width = ST7789_TFTHEIGHT;
            _height = ST7789_TFTWIDTH;
            break;
    }
}

void TFT_Driver_ST7789::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? ST7789_INVON : ST7789_INVOFF);
}

} // namespace TFT_Runtime
