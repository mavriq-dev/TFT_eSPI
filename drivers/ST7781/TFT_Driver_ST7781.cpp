#include <SPI.h>
#include "TFT_Driver_ST7781.h"

namespace TFT_Runtime {

TFT_Driver_ST7781::TFT_Driver_ST7781(Config& config) : TFT_Driver_Base(config) {
    _width = ST7781_TFTWIDTH;
    _height = ST7781_TFTHEIGHT;
}

void TFT_Driver_ST7781::init() {
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

    writeCommand(ST7781_SWRESET);    // Software reset
    delay(120);                      // Wait for reset to complete

    writeCommand(ST7781_SLPOUT);     // Exit Sleep
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
    writeCommand(ST7781_COLMOD);
    writeData(0x55);                 // 16-bit color

    // Memory Access Control
    writeCommand(ST7781_MADCTL);
    writeData(0x00);                 // Row/Column addressing

    // Display on
    writeCommand(ST7781_DISPON);
    delay(120);
}

void TFT_Driver_ST7781::initPowerSettings() {
    // Power Control 1
    writeCommand(ST7781_PWCTR1);
    writeData(0x23);                 // VRH[5:0]
    writeData(0x00);                 // BT[2:0]

    // Power Control 2
    writeCommand(ST7781_PWCTR2);
    writeData(0x10);                 // VCIRE, PON, AON

    // Power Control 3
    writeCommand(ST7781_PWCTR3);
    writeData(0x3E);                 // VRH
    writeData(0x28);                 // VCI1

    // VCOM Control
    writeCommand(ST7781_VMCTR1);
    writeData(0x86);                 // VMH[6:0]
    writeData(0x69);                 // VML[6:0]
}

void TFT_Driver_ST7781::initDisplaySettings() {
    // Frame Rate Control
    writeCommand(ST7781_FRMCTR1);
    writeData(0x00);                 // Division ratio
    writeData(0x1B);                 // Frame rate

    // Display Function Control
    writeCommand(ST7781_DISSET5);
    writeData(0x0A);                 // Non-overlap period
    writeData(0x82);                 // Number of lines
    writeData(0x27);                 // Clock frequency
    writeData(0x00);                 // Gate start position
}

void TFT_Driver_ST7781::initGamma() {
    // Positive Gamma Control
    writeCommand(ST7781_GMCTRP1);
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

    // Negative Gamma Control
    writeCommand(ST7781_GMCTRN1);
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
}

void TFT_Driver_ST7781::writeCommand(uint8_t cmd) {
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

void TFT_Driver_ST7781::writeData(uint8_t data) {
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

void TFT_Driver_ST7781::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_ST7781::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(ST7781_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x00 : 0x08);
            _width = ST7781_TFTWIDTH;
            _height = ST7781_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x60 : 0x68);
            _width = ST7781_TFTHEIGHT;
            _height = ST7781_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0xC0 : 0xC8);
            _width = ST7781_TFTWIDTH;
            _height = ST7781_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xA0 : 0xA8);
            _width = ST7781_TFTHEIGHT;
            _height = ST7781_TFTWIDTH;
            break;
    }
}

void TFT_Driver_ST7781::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? ST7781_INVON : ST7781_INVOFF);
}

void TFT_Driver_ST7781::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(ST7781_CASET);
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);

    writeCommand(ST7781_PASET);
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);

    writeCommand(ST7781_RAMWR);
}

void TFT_Driver_ST7781::setFrameRate(uint8_t divider, uint8_t rtna) {
    writeCommand(ST7781_FRMCTR1);
    writeData(divider);
    writeData(rtna);
}

void TFT_Driver_ST7781::setDisplayTiming(uint8_t frs, uint8_t btt) {
    writeCommand(ST7781_DISSET5);
    writeData(frs);
    writeData(btt);
}

void TFT_Driver_ST7781::setPowerControl(uint8_t vc, uint8_t bt, uint8_t dc) {
    writeCommand(ST7781_PWCTR1);
    writeData(vc);
    writeData(bt);
    writeData(dc);
}

void TFT_Driver_ST7781::setVCOMVoltage(uint8_t vcm) {
    writeCommand(ST7781_VMCTR1);
    writeData(vcm);
}

void TFT_Driver_ST7781::setGamma(uint8_t gamma) {
    writeCommand(ST7781_GAMSET);
    writeData(gamma);
}

void TFT_Driver_ST7781::setIdleMode(bool enable) {
    writeCommand(enable ? ST7781_IDMON : ST7781_IDMOFF);
}

void TFT_Driver_ST7781::setInterface(uint8_t mode) {
    writeCommand(ST7781_DISSET5);
    writeData(mode);
}

void TFT_Driver_ST7781::setDisplayMode(uint8_t mode) {
    writeCommand(ST7781_DISSET5);
    writeData(mode);
}

void TFT_Driver_ST7781::setTearingEffect(bool enable, bool mode) {
    writeCommand(enable ? ST7781_TEON : ST7781_TEOFF);
    if (enable) {
        writeData(mode ? 0x01 : 0x00);
    }
}

void TFT_Driver_ST7781::setPartialArea(uint16_t start, uint16_t end) {
    writeCommand(ST7781_PTLAR);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_ST7781::setScrollArea(uint16_t topFixed, uint16_t scrollArea, uint16_t bottomFixed) {
    writeCommand(ST7781_VSCRDEF);
    writeData(topFixed >> 8);
    writeData(topFixed & 0xFF);
    writeData(scrollArea >> 8);
    writeData(scrollArea & 0xFF);
    writeData(bottomFixed >> 8);
    writeData(bottomFixed & 0xFF);
}

void TFT_Driver_ST7781::setScrollStart(uint16_t start) {
    writeCommand(ST7781_VSCRSADD);
    writeData(start >> 8);
    writeData(start & 0xFF);
}

void TFT_Driver_ST7781::setPixelFormat(uint8_t format) {
    writeCommand(ST7781_COLMOD);
    writeData(format);
}

void TFT_Driver_ST7781::setMemoryAccessControl(uint8_t mode) {
    writeCommand(ST7781_MADCTL);
    writeData(mode);
}

void TFT_Driver_ST7781::setColumnAddress(uint16_t start, uint16_t end) {
    writeCommand(ST7781_CASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_ST7781::setPageAddress(uint16_t start, uint16_t end) {
    writeCommand(ST7781_PASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

} // namespace TFT_Runtime
