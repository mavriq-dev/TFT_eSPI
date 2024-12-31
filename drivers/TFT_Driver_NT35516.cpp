#include <SPI.h>
#include "TFT_Driver_NT35516.h"

namespace TFT_Runtime {

TFT_Driver_NT35516::TFT_Driver_NT35516(Config& config) : TFT_Driver_Base(config) {
    _width = NT35516_TFTWIDTH;
    _height = NT35516_TFTHEIGHT;
}

void TFT_Driver_NT35516::init() {
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

    writeCommand(NT35516_SWRESET);   // Software reset
    delay(120);                      // Wait for reset to complete

    writeCommand(NT35516_SLPOUT);    // Exit Sleep
    delay(120);

    // Extended command set
    writeCommand(NT35516_SETEXTC);
    writeData(0xFF);
    writeData(0x83);
    writeData(0x57);

    // Power settings
    initPowerSettings();
    delay(10);

    // Display settings
    initDisplaySettings();
    delay(10);

    // Gamma settings
    initGamma();
    delay(10);

    // Set pixel format
    writeCommand(NT35516_COLMOD);
    writeData(0x55);                 // 16-bit color

    // Memory Access Control
    writeCommand(NT35516_MADCTL);
    writeData(0x00);                 // Row/Column addressing

    // Set MIPI control for high speed
    writeCommand(NT35516_SETMIPI);
    writeData(0x03);                 // Enable high-speed mode

    // VDD settings for power optimization
    writeCommand(NT35516_SETVDD);
    writeData(0x24);                 // Optimized VDD settings

    // Display on
    writeCommand(NT35516_DISPON);
    delay(120);
}

void TFT_Driver_NT35516::initPowerSettings() {
    // Power Control 1
    writeCommand(NT35516_SETPOWER);
    writeData(0x00);                 // AVDD: 6.0V
    writeData(0x15);                 // AVDD: 5.4V
    writeData(0x1C);                 // VGH: 14.7V
    writeData(0x1C);                 // VGL: -7.35V
    writeData(0x83);                 // DDVDH: 5.94V
    writeData(0xAA);                 // Set pump clock frequency

    // VCOM setting
    writeCommand(NT35516_SETVCOM);
    writeData(0x3C);                 // VCOMH
    writeData(0x3C);                 // VCOML

    // Oscillator setting
    writeCommand(NT35516_SETOSC);
    writeData(0x01);                 // 65Hz frame rate
    writeData(0x11);                 // Internal oscillator
}

void TFT_Driver_NT35516::initDisplaySettings() {
    // Display Timing Setting
    writeCommand(NT35516_SETDISP);
    writeData(0x0A);                 // Non-overlap period
    writeData(0xA2);                 // Source bias current
    writeData(0x27);                 // Gate EQ/timing
    writeData(0x00);                 // Source/Gate start position
    writeData(0x11);                 // Source timing adjustment
    writeData(0x3C);                 // Gate timing adjustment

    // RGB Interface Setting
    writeCommand(NT35516_SETRGB);
    writeData(0x80);                 // RGB interface control
    writeData(0x00);                 // RGB interface timing
    writeData(0x00);                 // RGB interface polarity

    // Panel Characteristics
    writeCommand(NT35516_SETPANEL);
    writeData(0x09);                 // Panel characteristics setting
}

void TFT_Driver_NT35516::initGamma() {
    // Gamma Setting
    writeCommand(NT35516_SETGAMMA);
    // Positive Gamma
    writeData(0x00);
    writeData(0x09);
    writeData(0x0F);
    writeData(0x0E);
    writeData(0x07);
    writeData(0x10);
    writeData(0x0B);
    writeData(0x3A);
    writeData(0x33);
    writeData(0x07);
    writeData(0x0E);
    writeData(0x0C);
    writeData(0x0C);
    writeData(0x12);
    writeData(0x0F);
    writeData(0x08);
    // Negative Gamma
    writeData(0x00);
    writeData(0x0C);
    writeData(0x0F);
    writeData(0x0C);
    writeData(0x07);
    writeData(0x0F);
    writeData(0x0B);
    writeData(0x3A);
    writeData(0x33);
    writeData(0x07);
    writeData(0x0E);
    writeData(0x0C);
    writeData(0x0C);
    writeData(0x12);
    writeData(0x0F);
    writeData(0x08);
}

void TFT_Driver_NT35516::writeCommand(uint8_t cmd) {
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

void TFT_Driver_NT35516::writeData(uint8_t data) {
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

void TFT_Driver_NT35516::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_NT35516::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(NT35516_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x00 : 0x08);
            _width = NT35516_TFTWIDTH;
            _height = NT35516_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x60 : 0x68);
            _width = NT35516_TFTHEIGHT;
            _height = NT35516_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0xC0 : 0xC8);
            _width = NT35516_TFTWIDTH;
            _height = NT35516_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xA0 : 0xA8);
            _width = NT35516_TFTHEIGHT;
            _height = NT35516_TFTWIDTH;
            break;
    }
}

void TFT_Driver_NT35516::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? NT35516_INVON : NT35516_INVOFF);
}

void TFT_Driver_NT35516::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(NT35516_CASET);
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);

    writeCommand(NT35516_PASET);
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);

    writeCommand(NT35516_RAMWR);
}

void TFT_Driver_NT35516::setOscillator(uint8_t osc) {
    writeCommand(NT35516_SETOSC);
    writeData(osc);
}

void TFT_Driver_NT35516::setPowerControl(uint8_t vc, uint8_t bt, uint8_t dc) {
    writeCommand(NT35516_SETPOWER);
    writeData(vc);
    writeData(bt);
    writeData(dc);
}

void TFT_Driver_NT35516::setDisplayTiming(uint8_t rtna, uint8_t fpa, uint8_t bpa) {
    writeCommand(NT35516_SETDISP);
    writeData(rtna);
    writeData(fpa);
    writeData(bpa);
}

void TFT_Driver_NT35516::setRGBInterface(uint8_t rgb_if) {
    writeCommand(NT35516_SETRGB);
    writeData(rgb_if);
}

void TFT_Driver_NT35516::setCycle(uint8_t cycle) {
    writeCommand(NT35516_SETCYC);
    writeData(cycle);
}

void TFT_Driver_NT35516::setBacklightControl(uint8_t bl) {
    writeCommand(NT35516_SETBGP);
    writeData(bl);
}

void TFT_Driver_NT35516::setVCOMVoltage(uint8_t vcm) {
    writeCommand(NT35516_SETVCOM);
    writeData(vcm);
}

void TFT_Driver_NT35516::setExtendedCommands(bool enable) {
    writeCommand(NT35516_SETEXTC);
    if (enable) {
        writeData(0xFF);
        writeData(0x83);
        writeData(0x57);
    }
}

void TFT_Driver_NT35516::setMIPIControl(uint8_t mipi) {
    writeCommand(NT35516_SETMIPI);
    writeData(mipi);
}

void TFT_Driver_NT35516::setVDDSetting(uint8_t vdd) {
    writeCommand(NT35516_SETVDD);
    writeData(vdd);
}

void TFT_Driver_NT35516::setGamma(uint8_t gamma) {
    writeCommand(NT35516_SETGAMMA);
    writeData(gamma);
}

void TFT_Driver_NT35516::setPanelCharacteristics(uint8_t panel) {
    writeCommand(NT35516_SETPANEL);
    writeData(panel);
}

void TFT_Driver_NT35516::setDisplayOffset(uint16_t x, uint16_t y) {
    writeCommand(NT35516_SETOFFSET);
    writeData(x >> 8);
    writeData(x & 0xFF);
    writeData(y >> 8);
    writeData(y & 0xFF);
}

void TFT_Driver_NT35516::setCOMControl(uint8_t com) {
    writeCommand(NT35516_SETCOM);
    writeData(com);
}

void TFT_Driver_NT35516::setIdleMode(bool enable) {
    writeCommand(enable ? NT35516_IDMON : NT35516_IDMOFF);
}

void TFT_Driver_NT35516::setInterface(uint8_t mode) {
    writeCommand(NT35516_SETCOM);
    writeData(mode);
}

void TFT_Driver_NT35516::setDisplayMode(uint8_t mode) {
    writeCommand(NT35516_SETCOM);
    writeData(mode);
}

void TFT_Driver_NT35516::setTearingEffect(bool enable, bool mode) {
    writeCommand(enable ? NT35516_TEON : NT35516_TEOFF);
    if (enable) {
        writeData(mode ? 0x01 : 0x00);
    }
}

void TFT_Driver_NT35516::setPartialArea(uint16_t start, uint16_t end) {
    writeCommand(NT35516_PTLAR);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_NT35516::setPixelFormat(uint8_t format) {
    writeCommand(NT35516_COLMOD);
    writeData(format);
}

void TFT_Driver_NT35516::setMemoryAccessControl(uint8_t mode) {
    writeCommand(NT35516_MADCTL);
    writeData(mode);
}

void TFT_Driver_NT35516::setColumnAddress(uint16_t start, uint16_t end) {
    writeCommand(NT35516_CASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_NT35516::setPageAddress(uint16_t start, uint16_t end) {
    writeCommand(NT35516_PASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

} // namespace TFT_Runtime
