#include <SPI.h>
#include "TFT_Driver_ST7701.h"

namespace TFT_Runtime {

TFT_Driver_ST7701::TFT_Driver_ST7701(Config& config) : TFT_Driver_Base(config) {
    _width = ST7701_TFTWIDTH;
    _height = ST7701_TFTHEIGHT;
}

void TFT_Driver_ST7701::init() {
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

    writeCommand(ST7701_SWRESET);    // Software reset
    delay(120);                      // Wait for reset to complete

    writeCommand(ST7701_SLPOUT);     // Exit Sleep
    delay(120);

    // Command page 1
    initCommandPage1();
    delay(10);

    // Command page 2
    initCommandPage2();
    delay(10);

    // Command page 3
    initCommandPage3();
    delay(10);

    // Return to page 0 (normal commands)
    setCommandPage(0);

    // Set Color Format
    writeCommand(ST7701_COLMOD);
    writeData(0x55);                 // 16-bit color

    // Memory Access Control
    writeCommand(ST7701_MADCTL);
    writeData(0x48);                 // Default rotation (0)

    // Display Inversion Control
    writeCommand(ST7701_INVON);      // Enable display inversion

    // Set Gamma
    uint8_t pgamma[] = {0xD0, 0x00, 0x02, 0x07, 0x0B, 0x1A, 0x31, 0x54, 0x40, 0x29, 0x12, 0x12, 0x12, 0x17};
    uint8_t ngamma[] = {0xD0, 0x00, 0x02, 0x07, 0x05, 0x25, 0x2D, 0x44, 0x44, 0x1B, 0x1B, 0x1B, 0x1B, 0x17};
    setGamma(pgamma, ngamma);

    writeCommand(ST7701_TEON);       // Enable frame sync signal
    writeData(0x00);

    writeCommand(ST7701_DISPON);     // Display on
    delay(20);
}

void TFT_Driver_ST7701::initCommandPage1() {
    setCommandPage(1);

    // VCOM Control
    writeCommand(ST7701_VCOMS);
    writeData(0x75);                // VCOM = -1.0V

    // VGH Voltage Setting
    writeCommand(ST7701_VGHSS);
    writeData(0x87);                // VGH = 14.06V

    // Test Command Setting
    writeCommand(ST7701_TESTCMD);
    writeData(0x20);                // Test command

    // VGL Voltage Setting
    writeCommand(ST7701_VGLS);
    writeData(0x44);                // VGL = -8.1V

    // Power Control 1
    writeCommand(ST7701_PWCTLR1);
    writeData(0x85);                // Power control

    // Power Control 2
    writeCommand(ST7701_PWCTLR2);
    writeData(0x20);                // Power control

    // Power Control 3
    writeCommand(ST7701_PWCTLR3);
    writeData(0x10);                // Power control
}

void TFT_Driver_ST7701::initCommandPage2() {
    setCommandPage(2);

    // Power Control 4
    writeCommand(ST7701_PWCTLR4);
    writeData(0x00);
    writeData(0x11);
    writeData(0x11);
    writeData(0x11);

    // Power Control 5
    writeCommand(ST7701_PWCTLR5);
    writeData(0x02);
    writeData(0x01);
    writeData(0x02);
    writeData(0x01);

    // Power Control 6
    writeCommand(ST7701_PWCTLR6);
    writeData(0x25);
    writeData(0x25);
    writeData(0x00);
    writeData(0x00);
}

void TFT_Driver_ST7701::initCommandPage3() {
    setCommandPage(3);

    // Panel Timing Control
    writeCommand(0xC0);
    writeData(0x12);
    writeData(0x27);
    writeData(0x00);
    writeData(0x08);
    writeData(0x00);
    writeData(0x0F);

    // Panel Pad Control
    writeCommand(0xC1);
    writeData(0x01);
    writeData(0x02);
    writeData(0x08);
    writeData(0x12);
    writeData(0x18);

    // Panel Timing Setting
    writeCommand(0xC2);
    writeData(0x00);
    writeData(0x00);
    writeData(0x07);
    writeData(0x00);
    writeData(0x0F);
}

void TFT_Driver_ST7701::setCommandPage(uint8_t page) {
    writeCommand(ST7701_CMD_ACCESS);
    if (page == 0) {
        writeData(0x00);
    } else {
        writeData(0x77);
        writeData(0x01);
        writeData(0x00);
        writeData(0x00);
        writeData(page);
    }
}

void TFT_Driver_ST7701::writeCommand(uint8_t cmd) {
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

void TFT_Driver_ST7701::writeData(uint8_t data) {
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

void TFT_Driver_ST7701::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_ST7701::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(ST7701_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x48 : 0x40);
            _width = ST7701_TFTWIDTH;
            _height = ST7701_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x28 : 0x20);
            _width = ST7701_TFTHEIGHT;
            _height = ST7701_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0x88 : 0x80);
            _width = ST7701_TFTWIDTH;
            _height = ST7701_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xE8 : 0xE0);
            _width = ST7701_TFTHEIGHT;
            _height = ST7701_TFTWIDTH;
            break;
    }
}

void TFT_Driver_ST7701::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? ST7701_INVON : ST7701_INVOFF);
}

void TFT_Driver_ST7701::setVCOM(uint8_t value) {
    setCommandPage(1);
    writeCommand(ST7701_VCOMS);
    writeData(value);
    setCommandPage(0);
}

void TFT_Driver_ST7701::setGamma(uint8_t* pgamma, uint8_t* ngamma) {
    writeCommand(ST7701_PGAMMA);
    for (int i = 0; i < 14; i++) {
        writeData(pgamma[i]);
    }

    writeCommand(ST7701_NGAMMA);
    for (int i = 0; i < 14; i++) {
        writeData(ngamma[i]);
    }
}

} // namespace TFT_Runtime
