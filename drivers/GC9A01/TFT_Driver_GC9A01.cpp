#include <SPI.h>
#include "TFT_Driver_GC9A01.h"

namespace TFT_Runtime {

TFT_Driver_GC9A01::TFT_Driver_GC9A01(Config& config) : TFT_Driver_Base(config) {
    _width = GC9A01_TFTWIDTH;
    _height = GC9A01_TFTHEIGHT;
}

void TFT_Driver_GC9A01::init() {
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

    writeCommand(GC9A01_SWRESET);    // Software reset
    delay(120);                      // Wait for reset to complete

    writeCommand(0xEF);
    writeCommand(0xEB);
    writeData(0x14);

    writeCommand(0xFE);
    writeCommand(0xEF);

    writeCommand(0xEB);
    writeData(0x14);

    writeCommand(0x84);
    writeData(0x40);

    writeCommand(0x85);
    writeData(0xFF);

    writeCommand(0x86);
    writeData(0xFF);

    writeCommand(0x87);
    writeData(0xFF);

    writeCommand(0x88);
    writeData(0x0A);

    writeCommand(0x89);
    writeData(0x21);

    writeCommand(0x8A);
    writeData(0x00);

    writeCommand(0x8B);
    writeData(0x80);

    writeCommand(0x8C);
    writeData(0x01);

    writeCommand(0x8D);
    writeData(0x01);

    writeCommand(0x8E);
    writeData(0xFF);

    writeCommand(0x8F);
    writeData(0xFF);

    writeCommand(GC9A01_MADCTL);
    writeData(0x48);

    writeCommand(0x3A);
    writeData(0x05);

    writeCommand(0x90);
    writeData(0x08);
    writeData(0x08);
    writeData(0x08);
    writeData(0x08);

    writeCommand(0xBD);
    writeData(0x06);

    writeCommand(0xBC);
    writeData(0x00);

    writeCommand(0xFF);
    writeData(0x60);
    writeData(0x01);
    writeData(0x04);

    writeCommand(GC9A01_PWCTR2);
    writeData(0x13);

    writeCommand(GC9A01_PWCTR3);
    writeData(0x13);

    writeCommand(GC9A01_PWCTR4);
    writeData(0x22);

    writeCommand(0xBE);
    writeData(0x11);

    writeCommand(0xE1);
    writeData(0x10);
    writeData(0x0E);

    writeCommand(0xDF);
    writeData(0x21);
    writeData(0x0c);
    writeData(0x02);

    writeCommand(0xF0);
    writeData(0x45);
    writeData(0x09);
    writeData(0x08);
    writeData(0x08);
    writeData(0x26);
    writeData(0x2A);

    writeCommand(0xF1);
    writeData(0x43);
    writeData(0x70);
    writeData(0x72);
    writeData(0x36);
    writeData(0x37);
    writeData(0x6F);

    writeCommand(0xF2);
    writeData(0x45);
    writeData(0x09);
    writeData(0x08);
    writeData(0x08);
    writeData(0x26);
    writeData(0x2A);

    writeCommand(0xF3);
    writeData(0x43);
    writeData(0x70);
    writeData(0x72);
    writeData(0x36);
    writeData(0x37);
    writeData(0x6F);

    writeCommand(0xED);
    writeData(0x1B);
    writeData(0x0B);

    writeCommand(0xAE);
    writeData(0x77);

    writeCommand(0xCD);
    writeData(0x63);

    writeCommand(0x70);
    writeData(0x07);
    writeData(0x07);
    writeData(0x04);
    writeData(0x0E);
    writeData(0x0F);
    writeData(0x09);
    writeData(0x07);
    writeData(0x08);
    writeData(0x03);

    writeCommand(GC9A01_PVGAMCTRL);
    writeData(0x00);
    writeData(0x03);
    writeData(0x09);
    writeData(0x08);
    writeData(0x16);
    writeData(0x0A);
    writeData(0x3F);
    writeData(0x78);
    writeData(0x4C);
    writeData(0x09);
    writeData(0x0A);
    writeData(0x08);
    writeData(0x16);
    writeData(0x1A);
    writeData(0x0F);

    writeCommand(GC9A01_NVGAMCTRL);
    writeData(0x00);
    writeData(0x16);
    writeData(0x19);
    writeData(0x03);
    writeData(0x0F);
    writeData(0x05);
    writeData(0x32);
    writeData(0x45);
    writeData(0x46);
    writeData(0x04);
    writeData(0x0E);
    writeData(0x0D);
    writeData(0x35);
    writeData(0x37);
    writeData(0x0F);

    writeCommand(GC9A01_TEON);      // Enable frame sync signal if needed
    writeData(0x00);

    writeCommand(GC9A01_SLPOUT);    // Exit sleep
    delay(120);

    writeCommand(GC9A01_DISPON);    // Display on
    delay(20);
}

void TFT_Driver_GC9A01::writeCommand(uint8_t cmd) {
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

void TFT_Driver_GC9A01::writeData(uint8_t data) {
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

void TFT_Driver_GC9A01::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_GC9A01::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(GC9A01_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x48 : 0x40);
            _width = GC9A01_TFTWIDTH;
            _height = GC9A01_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x28 : 0x20);
            _width = GC9A01_TFTHEIGHT;
            _height = GC9A01_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0x88 : 0x80);
            _width = GC9A01_TFTWIDTH;
            _height = GC9A01_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xE8 : 0xE0);
            _width = GC9A01_TFTHEIGHT;
            _height = GC9A01_TFTWIDTH;
            break;
    }
}

void TFT_Driver_GC9A01::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? GC9A01_INVON : GC9A01_INVOFF);
}

} // namespace TFT_Runtime
