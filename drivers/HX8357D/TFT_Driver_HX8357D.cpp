#include <SPI.h>
#include "TFT_Driver_HX8357D.h"

namespace TFT_Runtime {

TFT_Driver_HX8357D::TFT_Driver_HX8357D(Config& config) : TFT_Driver_Base(config) {
    _width = HX8357D_TFTWIDTH;
    _height = HX8357D_TFTHEIGHT;
}

void TFT_Driver_HX8357D::init() {
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

    writeCommand(HX8357D_SWRESET);    // Software reset
    delay(150);                       // Wait for reset to complete

    // Set extension command
    writeCommand(HX8357D_SETC);
    writeData(0xFF);
    writeData(0x83);
    writeData(0x57);
    delay(50);

    // Set OPON
    writeCommand(HX8357D_SETOSC);
    writeData(0x68);
    delay(50);

    // Set power control
    writeCommand(HX8357D_SETPWR1);
    writeData(0x00);
    writeData(0x15);
    writeData(0x1C);
    writeData(0x1C);
    writeData(0x83);
    writeData(0xAA);
    delay(50);

    // Set RGBIF
    writeCommand(HX8357D_SETRGB);
    writeData(0x00);  // RGB interface control
    writeData(0x00);  // DE pol, PCLK pol, HSYNC pol, VSYNC pol
    writeData(0x06);  // RGB seq: RGB
    writeData(0x06);  // RGB interface timing control

    // Set CYC
    writeCommand(HX8357D_SETCYC);
    writeData(0x7F);  // NW control
    writeData(0x00);  // Source EQ control
    writeData(0x00);  // Timing control 4H w/ 4-delay
    writeData(0x05);  // Timing control

    // Set STBA (Standby)
    writeCommand(HX8357D_SETSTBA);
    writeData(0x50);  // OPON: Normal
    writeData(0x50);  // OPON: Idle
    writeData(0x01);  // STBA
    writeData(0x3C);  // STBA
    writeData(0x1E);  // STBA
    writeData(0x08);  // GEN

    // Set Panel Driving
    writeCommand(HX8357D_SETPANEL);
    writeData(0x02);  // REV, SM, GS

    // Set Display Cycle
    writeCommand(0xD0);
    writeData(0x0D);

    // Set Gamma
    writeCommand(HX8357D_SETGAMMA);
    writeData(0x00);  writeData(0x15);  writeData(0x1D);  writeData(0x0F);
    writeData(0x12);  writeData(0x08);  writeData(0x47);  writeData(0xF7);
    writeData(0x37);  writeData(0x07);  writeData(0x10);  writeData(0x03);
    writeData(0x0E);  writeData(0x09);  writeData(0x00);

    // Set COLMOD
    writeCommand(HX8357D_COLMOD);
    writeData(0x55);  // 16-bit/pixel

    // Set MADCTL
    writeCommand(HX8357D_MADCTL);
    writeData(0x48);  // Default rotation (0)

    writeCommand(HX8357D_TEON);  // Tear effect line on
    writeData(0x00);

    writeCommand(HX8357D_SLPOUT);  // Exit sleep
    delay(150);

    writeCommand(HX8357D_DISPON);  // Display on
    delay(50);
}

void TFT_Driver_HX8357D::writeCommand(uint8_t cmd) {
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

void TFT_Driver_HX8357D::writeData(uint8_t data) {
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

void TFT_Driver_HX8357D::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_HX8357D::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(HX8357D_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x48 : 0x40);
            _width = HX8357D_TFTWIDTH;
            _height = HX8357D_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x28 : 0x20);
            _width = HX8357D_TFTHEIGHT;
            _height = HX8357D_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0x88 : 0x80);
            _width = HX8357D_TFTWIDTH;
            _height = HX8357D_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xE8 : 0xE0);
            _width = HX8357D_TFTHEIGHT;
            _height = HX8357D_TFTWIDTH;
            break;
    }
}

void TFT_Driver_HX8357D::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? HX8357D_INVON : HX8357D_INVOFF);
}

} // namespace TFT_Runtime
