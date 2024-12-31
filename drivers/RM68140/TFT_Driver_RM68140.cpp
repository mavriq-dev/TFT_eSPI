#include <SPI.h>
#include "TFT_Driver_RM68140.h"

namespace TFT_Runtime {

TFT_Driver_RM68140::TFT_Driver_RM68140(Config& config) : TFT_Driver_Base(config) {
    _width = RM68140_TFTWIDTH;
    _height = RM68140_TFTHEIGHT;
}

void TFT_Driver_RM68140::init() {
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

    writeCommand(RM68140_SWRESET);    // Software reset
    delay(150);                       // Wait for reset to complete

    writeCommand(RM68140_SLPOUT);     // Exit Sleep
    delay(120);

    // Power Control A
    writeCommand(0xCB);
    writeData(0x39);
    writeData(0x2C);
    writeData(0x00);
    writeData(0x34);
    writeData(0x02);

    // Power Control B
    writeCommand(0xCF);
    writeData(0x00);
    writeData(0xC1);
    writeData(0x30);

    // Driver timing control A
    writeCommand(0xE8);
    writeData(0x85);
    writeData(0x00);
    writeData(0x78);

    // Driver timing control B
    writeCommand(0xEA);
    writeData(0x00);
    writeData(0x00);

    // Power on sequence control
    writeCommand(0xED);
    writeData(0x64);
    writeData(0x03);
    writeData(0x12);
    writeData(0x81);

    // Pump ratio control
    writeCommand(0xF7);
    writeData(0x20);

    // Power Control 1
    writeCommand(RM68140_PWCTR1);
    writeData(0x23);

    // Power Control 2
    writeCommand(RM68140_PWCTR2);
    writeData(0x10);

    // VCOM Control 1
    writeCommand(RM68140_VMCTR1);
    writeData(0x3E);
    writeData(0x28);

    // VCOM Control 2
    writeCommand(RM68140_VMCTR2);
    writeData(0x86);

    // Memory Access Control
    writeCommand(RM68140_MADCTL);
    writeData(0x48);                  // Default rotation (0)

    // Pixel Format Set
    writeCommand(RM68140_PIXFMT);
    writeData(0x55);                  // 16-bit color

    // Frame Rate Control
    writeCommand(RM68140_FRMCTR1);
    writeData(0x00);
    writeData(0x18);

    // Display Function Control
    writeCommand(RM68140_DFUNCTR);
    writeData(0x08);
    writeData(0x82);
    writeData(0x27);

    // Gamma Function Disable
    writeCommand(RM68140_GAMSET);
    writeData(0x01);

    // Positive Gamma Correction
    writeCommand(RM68140_GMCTRP1);
    writeData(0x0F);
    writeData(0x31);
    writeData(0x2B);
    writeData(0x0C);
    writeData(0x0E);
    writeData(0x08);
    writeData(0x4E);
    writeData(0xF1);
    writeData(0x37);
    writeData(0x07);
    writeData(0x10);
    writeData(0x03);
    writeData(0x0E);
    writeData(0x09);
    writeData(0x00);

    // Negative Gamma Correction
    writeCommand(RM68140_GMCTRN1);
    writeData(0x00);
    writeData(0x0E);
    writeData(0x14);
    writeData(0x03);
    writeData(0x11);
    writeData(0x07);
    writeData(0x31);
    writeData(0xC1);
    writeData(0x48);
    writeData(0x08);
    writeData(0x0F);
    writeData(0x0C);
    writeData(0x31);
    writeData(0x36);
    writeData(0x0F);

    writeCommand(RM68140_DISPON);     // Display on
    delay(120);
}

void TFT_Driver_RM68140::writeCommand(uint8_t cmd) {
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

void TFT_Driver_RM68140::writeData(uint8_t data) {
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

void TFT_Driver_RM68140::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_RM68140::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(RM68140_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x48 : 0x40);
            _width = RM68140_TFTWIDTH;
            _height = RM68140_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x28 : 0x20);
            _width = RM68140_TFTHEIGHT;
            _height = RM68140_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0x88 : 0x80);
            _width = RM68140_TFTWIDTH;
            _height = RM68140_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xE8 : 0xE0);
            _width = RM68140_TFTHEIGHT;
            _height = RM68140_TFTWIDTH;
            break;
    }
}

void TFT_Driver_RM68140::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? RM68140_INVON : RM68140_INVOFF);
}

} // namespace TFT_Runtime
