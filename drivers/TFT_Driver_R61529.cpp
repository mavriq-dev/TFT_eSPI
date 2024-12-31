#include <SPI.h>
#include "TFT_Driver_R61529.h"

namespace TFT_Runtime {

TFT_Driver_R61529::TFT_Driver_R61529(Config& config) : TFT_Driver_Base(config) {
    _width = R61529_TFTWIDTH;
    _height = R61529_TFTHEIGHT;
}

void TFT_Driver_R61529::init() {
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

    writeCommand(R61529_SWRESET);    // Software reset
    delay(120);                      // Wait for reset to complete

    // Power Setting
    writeCommand(R61529_PWRCTRL1);
    writeData(0x0B);
    writeData(0x0B);
    writeData(0x28);
    writeData(0x28);

    writeCommand(R61529_PWRCTRL2);
    writeData(0x01);

    // VCOM Control
    writeCommand(R61529_VMCTRL1);
    writeData(0x45);
    writeData(0x45);

    writeCommand(R61529_VMCTRL2);
    writeData(0xA2);

    // Memory Access Control
    writeCommand(R61529_MADCTL);
    writeData(0x48);

    // Pixel Format Set
    writeCommand(R61529_PIXFMT);
    writeData(0x55);                 // 16-bit/pixel

    // Frame Rate Control
    writeCommand(R61529_FRAMERATE);
    writeData(0xA0);                 // 60Hz

    // Display Timing Setting for Normal Mode
    writeCommand(R61529_EQTIMING);
    writeData(0x00);
    writeData(0x00);
    writeData(0x00);
    writeData(0x00);

    // Interface Control
    writeCommand(R61529_INTERFACE);
    writeData(0x00);                 // System interface
    writeData(0x00);                 // RGB interface control
    writeData(0x00);                 // Control signal polarity

    // DPI PLL Setting
    writeCommand(R61529_DPIPLL);
    writeData(0x00);
    writeData(0x30);                 // PLL = 120MHz
    writeData(0x03);

    // Gamma Setting
    writeCommand(R61529_GAMMA1);
    writeData(0x00);
    writeData(0x13);
    writeData(0x18);
    writeData(0x04);
    writeData(0x0F);
    writeData(0x06);
    writeData(0x3A);
    writeData(0x56);
    writeData(0x4D);
    writeData(0x03);
    writeData(0x0A);
    writeData(0x06);
    writeData(0x30);
    writeData(0x3E);
    writeData(0x0F);

    writeCommand(R61529_GAMMA2);
    writeData(0x00);
    writeData(0x13);
    writeData(0x18);
    writeData(0x01);
    writeData(0x11);
    writeData(0x06);
    writeData(0x38);
    writeData(0x34);
    writeData(0x4D);
    writeData(0x06);
    writeData(0x0D);
    writeData(0x0B);
    writeData(0x31);
    writeData(0x37);
    writeData(0x0F);

    // Set Brightness Control
    writeCommand(R61529_WRDISBV);
    writeData(0xFF);                 // Maximum Brightness

    // Set Display Control
    writeCommand(R61529_WRCTRLD);
    writeData(0x2C);                 // Enable Brightness Control

    writeCommand(R61529_SLPOUT);     // Exit Sleep
    delay(120);

    writeCommand(R61529_DISPON);     // Display on
    delay(20);
}

void TFT_Driver_R61529::writeCommand(uint8_t cmd) {
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

void TFT_Driver_R61529::writeData(uint8_t data) {
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

void TFT_Driver_R61529::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_R61529::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(R61529_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x48 : 0x40);
            _width = R61529_TFTWIDTH;
            _height = R61529_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x28 : 0x20);
            _width = R61529_TFTHEIGHT;
            _height = R61529_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0x88 : 0x80);
            _width = R61529_TFTWIDTH;
            _height = R61529_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xE8 : 0xE0);
            _width = R61529_TFTHEIGHT;
            _height = R61529_TFTWIDTH;
            break;
    }
}

void TFT_Driver_R61529::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? R61529_INVON : R61529_INVOFF);
}

} // namespace TFT_Runtime
