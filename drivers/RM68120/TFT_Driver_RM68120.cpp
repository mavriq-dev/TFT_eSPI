#include <SPI.h>
#include "TFT_Driver_RM68120.h"

namespace TFT_Runtime {

TFT_Driver_RM68120::TFT_Driver_RM68120(Config& config) : TFT_Driver_Base(config) {
    _width = RM68120_TFTWIDTH;
    _height = RM68120_TFTHEIGHT;
}

void TFT_Driver_RM68120::init() {
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

    writeCommand(RM68120_SWRESET);    // Software reset
    delay(120);                       // Wait for reset to complete

    writeCommand(RM68120_SLPOUT);     // Exit Sleep
    delay(120);

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
    writeCommand(RM68120_COLMOD);
    writeData(0x55);                  // 16-bit color

    // Memory Access Control
    writeCommand(RM68120_MADCTL);
    writeData(0x00);                  // Row/Column addressing

    // Display on
    writeCommand(RM68120_DISPON);
    delay(120);
}

void TFT_Driver_RM68120::initPowerSettings() {
    // Power Control 1
    writeCommand(RM68120_PWCTR1);
    writeData(0x17);                  // GVDD = 4.6V
    writeData(0x15);                  // VCI1 = 2.5V

    // Power Control 2
    writeCommand(RM68120_PWCTR2);
    writeData(0x41);                  // VGH = 14.7V, VGL = -7.35V

    // Power Control 3
    writeCommand(RM68120_PWCTR3);
    writeData(0x00);
    writeData(0x12);
    writeData(0x80);

    // Power Control 4
    writeCommand(RM68120_PWCTR4);
    writeData(0x8D);
    writeData(0xEE);

    // Power Control 5
    writeCommand(RM68120_PWCTR5);
    writeData(0x8D);
    writeData(0xEE);

    // VCOM Control
    writeCommand(RM68120_VMCTR1);
    writeData(0x0E);
}

void TFT_Driver_RM68120::initDisplaySettings() {
    // Frame Rate Control
    writeCommand(RM68120_FRMCTR1);
    writeData(0x00);
    writeData(0x18);

    // Display Function Control
    writeCommand(RM68120_DISSET5);
    writeData(0x0A);
    writeData(0xA2);

    // Interface Control
    writeCommand(0xF6);
    writeData(0x01);
    writeData(0x30);
    writeData(0x00);
}

void TFT_Driver_RM68120::initGamma() {
    // Positive Gamma Control
    writeCommand(RM68120_GMCTRP1);
    writeData(0x0F);
    writeData(0x22);
    writeData(0x1F);
    writeData(0x0B);
    writeData(0x0B);
    writeData(0x06);
    writeData(0x4C);
    writeData(0x66);
    writeData(0x3F);
    writeData(0x0F);
    writeData(0x16);
    writeData(0x03);
    writeData(0x0A);
    writeData(0x06);
    writeData(0x00);

    // Negative Gamma Control
    writeCommand(RM68120_GMCTRN1);
    writeData(0x0F);
    writeData(0x22);
    writeData(0x1F);
    writeData(0x0B);
    writeData(0x0B);
    writeData(0x06);
    writeData(0x4C);
    writeData(0x66);
    writeData(0x3F);
    writeData(0x0F);
    writeData(0x16);
    writeData(0x03);
    writeData(0x0A);
    writeData(0x06);
    writeData(0x00);
}

void TFT_Driver_RM68120::writeCommand(uint8_t cmd) {
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

void TFT_Driver_RM68120::writeData(uint8_t data) {
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

void TFT_Driver_RM68120::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_RM68120::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(RM68120_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x00 : 0x08);
            _width = RM68120_TFTWIDTH;
            _height = RM68120_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x60 : 0x68);
            _width = RM68120_TFTHEIGHT;
            _height = RM68120_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0xC0 : 0xC8);
            _width = RM68120_TFTWIDTH;
            _height = RM68120_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xA0 : 0xA8);
            _width = RM68120_TFTHEIGHT;
            _height = RM68120_TFTWIDTH;
            break;
    }
}

void TFT_Driver_RM68120::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? RM68120_INVON : RM68120_INVOFF);
}

void TFT_Driver_RM68120::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(RM68120_CASET);
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);

    writeCommand(RM68120_RASET);
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);

    writeCommand(RM68120_RAMWR);
}

void TFT_Driver_RM68120::setFrameRate(uint8_t divider, uint8_t rtna) {
    writeCommand(RM68120_FRMCTR1);
    writeData(divider);
    writeData(rtna);
}

void TFT_Driver_RM68120::setDisplayTiming(uint8_t frs, uint8_t btt) {
    writeCommand(RM68120_DISSET5);
    writeData(frs);
    writeData(btt);
}

void TFT_Driver_RM68120::setPowerControl(uint8_t vc, uint8_t bt, uint8_t dc) {
    writeCommand(RM68120_PWCTR1);
    writeData(vc);
    writeData(bt);
    writeData(dc);
}

void TFT_Driver_RM68120::setVCOMVoltage(uint8_t vcm) {
    writeCommand(RM68120_VMCTR1);
    writeData(vcm);
}

void TFT_Driver_RM68120::setGamma(uint8_t gamma) {
    writeCommand(RM68120_GAMSET);
    writeData(gamma);
}

void TFT_Driver_RM68120::setIdleMode(bool enable) {
    writeCommand(enable ? RM68120_IDMON : RM68120_IDMOFF);
}

void TFT_Driver_RM68120::setInterface(uint8_t mode) {
    writeCommand(0xF6);
    writeData(mode);
}

void TFT_Driver_RM68120::setDisplayMode(uint8_t mode) {
    writeCommand(RM68120_DISSET5);
    writeData(mode);
}

void TFT_Driver_RM68120::setTearingEffect(bool enable, bool mode) {
    writeCommand(enable ? RM68120_TEON : RM68120_TEOFF);
    if (enable) {
        writeData(mode ? 0x01 : 0x00);
    }
}

void TFT_Driver_RM68120::setPartialArea(uint16_t start, uint16_t end) {
    writeCommand(RM68120_PTLAR);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_RM68120::setPixelFormat(uint8_t format) {
    writeCommand(RM68120_COLMOD);
    writeData(format);
}

void TFT_Driver_RM68120::setMemoryAccessControl(uint8_t mode) {
    writeCommand(RM68120_MADCTL);
    writeData(mode);
}

void TFT_Driver_RM68120::setColumnAddress(uint16_t start, uint16_t end) {
    writeCommand(RM68120_CASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_RM68120::setRowAddress(uint16_t start, uint16_t end) {
    writeCommand(RM68120_RASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

} // namespace TFT_Runtime
