#include <SPI.h>
#include "TFT_Driver_ST7796S.h"

namespace TFT_Runtime {

TFT_Driver_ST7796S::TFT_Driver_ST7796S(Config& config) : TFT_Driver_Base(config) {
    _width = ST7796S_TFTWIDTH;
    _height = ST7796S_TFTHEIGHT;
}

void TFT_Driver_ST7796S::init() {
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

    writeCommand(ST7796S_SWRESET);    // Software reset
    delay(120);                       // Wait for reset to complete

    writeCommand(ST7796S_SLPOUT);     // Exit Sleep
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

    // Set pixel format - 16bit/pixel
    writeCommand(ST7796S_COLMOD);
    writeData(0x55);                  // 16-bit color

    // Memory Access Control
    writeCommand(ST7796S_MADCTL);
    writeData(0x00);                  // Normal orientation

    // Frame Rate Control
    writeCommand(ST7796S_FRMCTR1);
    writeData(0xA0);                  // 60Hz

    // Display Function Control
    writeCommand(ST7796S_DFUNCTR);
    writeData(0x02);                  // Normal mode
    writeData(0x02);                  // 480 lines
    writeData(0x3B);                  // Source output control

    // Set image format
    writeCommand(ST7796S_SETIMAGE);
    writeData(0x00);                  // RGB interface control

    // Set VCOM voltage
    writeCommand(ST7796S_VMCTR);
    writeData(0x20);                  // VCOM = -1.1V

    // Display on
    writeCommand(ST7796S_DISPON);
    delay(120);
}

void TFT_Driver_ST7796S::initPowerSettings() {
    // Power Control 1
    writeCommand(ST7796S_PWCTR1);
    writeData(0x80);                  // AVDD = 6.8V
    writeData(0x15);                  // AVCL = -4.8V
    writeData(0x15);                  // VDS = 2.4V

    // Power Control 2
    writeCommand(ST7796S_PWCTR2);
    writeData(0x13);                  // VGH = 14.7V, VGL = -7.35V

    // Power Control 3
    writeCommand(ST7796S_PWCTR3);
    writeData(0xA7);                  // Operational amplifier current

    // Power Control 4
    writeCommand(ST7796S_PWCTR4);
    writeData(0xA7);                  // Source amplifier current

    // Power Control 5
    writeCommand(ST7796S_PWCTR5);
    writeData(0x22);                  // VCOM control
}

void TFT_Driver_ST7796S::initDisplaySettings() {
    // Frame Rate Control
    writeCommand(ST7796S_FRMCTR1);
    writeData(0xA0);                  // 60Hz
    writeData(0x11);                  // Column inversion

    // Display Function Control
    writeCommand(ST7796S_DFUNCTR);
    writeData(0x02);                  // Normal mode
    writeData(0x02);                  // 480 lines
    writeData(0x3B);                  // Source output control
}

void TFT_Driver_ST7796S::initGamma() {
    // Positive Gamma Control
    writeCommand(ST7796S_GMCTRP1);
    writeData(0xF0);
    writeData(0x09);
    writeData(0x13);
    writeData(0x12);
    writeData(0x12);
    writeData(0x2B);
    writeData(0x3C);
    writeData(0x44);
    writeData(0x4B);
    writeData(0x1B);
    writeData(0x18);
    writeData(0x17);
    writeData(0x1D);
    writeData(0x21);

    // Negative Gamma Control
    writeCommand(ST7796S_GMCTRN1);
    writeData(0xF0);
    writeData(0x09);
    writeData(0x13);
    writeData(0x0C);
    writeData(0x0D);
    writeData(0x27);
    writeData(0x3B);
    writeData(0x44);
    writeData(0x4D);
    writeData(0x0B);
    writeData(0x17);
    writeData(0x17);
    writeData(0x1D);
    writeData(0x21);
}

void TFT_Driver_ST7796S::writeCommand(uint8_t cmd) {
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

void TFT_Driver_ST7796S::writeData(uint8_t data) {
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

void TFT_Driver_ST7796S::writeBlock(uint16_t* data, uint32_t len) {
    if (config.interface == InterfaceMode::SPI) {
        digitalWrite(config.spi.dc_pin, HIGH);
        if (config.spi.cs_pin >= 0) digitalWrite(config.spi.cs_pin, LOW);
        
        for (uint32_t i = 0; i < len; i++) {
            SPI.transfer16(data[i]);
        }
        
        if (config.spi.cs_pin >= 0) digitalWrite(config.spi.cs_pin, HIGH);
    } else {
        // Parallel 8-bit implementation
        digitalWrite(config.parallel.dc_pin, HIGH);
        if (config.parallel.cs_pin >= 0) digitalWrite(config.parallel.cs_pin, LOW);
        
        for (uint32_t i = 0; i < len; i++) {
            // Write high byte then low byte
            // Implementation depends on hardware setup
        }
        
        if (config.parallel.cs_pin >= 0) digitalWrite(config.parallel.cs_pin, HIGH);
    }
}

void TFT_Driver_ST7796S::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(ST7796S_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x00 : 0x08);
            _width = ST7796S_TFTWIDTH;
            _height = ST7796S_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x60 : 0x68);
            _width = ST7796S_TFTHEIGHT;
            _height = ST7796S_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0xC0 : 0xC8);
            _width = ST7796S_TFTWIDTH;
            _height = ST7796S_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xA0 : 0xA8);
            _width = ST7796S_TFTHEIGHT;
            _height = ST7796S_TFTWIDTH;
            break;
    }
}

void TFT_Driver_ST7796S::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? ST7796S_INVON : ST7796S_INVOFF);
}

void TFT_Driver_ST7796S::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(ST7796S_CASET);
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);

    writeCommand(ST7796S_PASET);
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);

    writeCommand(ST7796S_RAMWR);
}

void TFT_Driver_ST7796S::setFrameRate(uint8_t divider, uint8_t rtna) {
    writeCommand(ST7796S_FRMCTR1);
    writeData(divider);
    writeData(rtna);
}

void TFT_Driver_ST7796S::setImageFormat(uint8_t format) {
    writeCommand(ST7796S_SETIMAGE);
    writeData(format);
}

void TFT_Driver_ST7796S::setInversion(uint8_t mode) {
    writeCommand(ST7796S_INVCTR);
    writeData(mode);
}

void TFT_Driver_ST7796S::setDisplayFunction(uint8_t mode) {
    writeCommand(ST7796S_DFUNCTR);
    writeData(mode);
}

void TFT_Driver_ST7796S::setPowerControl1(uint8_t vrh) {
    writeCommand(ST7796S_PWCTR1);
    writeData(vrh);
}

void TFT_Driver_ST7796S::setPowerControl2(uint8_t bt) {
    writeCommand(ST7796S_PWCTR2);
    writeData(bt);
}

void TFT_Driver_ST7796S::setPowerControl3(uint8_t mode) {
    writeCommand(ST7796S_PWCTR3);
    writeData(mode);
}

void TFT_Driver_ST7796S::setPowerControl4(uint8_t mode) {
    writeCommand(ST7796S_PWCTR4);
    writeData(mode);
}

void TFT_Driver_ST7796S::setPowerControl5(uint8_t mode) {
    writeCommand(ST7796S_PWCTR5);
    writeData(mode);
}

void TFT_Driver_ST7796S::setVCOMControl(uint8_t vcm) {
    writeCommand(ST7796S_VMCTR);
    writeData(vcm);
}

void TFT_Driver_ST7796S::setMemoryAccessControl(uint8_t mode) {
    writeCommand(ST7796S_MADCTL);
    writeData(mode);
}

void TFT_Driver_ST7796S::setPixelFormat(uint8_t format) {
    writeCommand(ST7796S_COLMOD);
    writeData(format);
}

void TFT_Driver_ST7796S::setColumnAddress(uint16_t start, uint16_t end) {
    writeCommand(ST7796S_CASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_ST7796S::setPageAddress(uint16_t start, uint16_t end) {
    writeCommand(ST7796S_PASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_ST7796S::setScrollArea(uint16_t tfa, uint16_t vsa, uint16_t bfa) {
    writeCommand(ST7796S_VSCRDEF);
    writeData(tfa >> 8);
    writeData(tfa & 0xFF);
    writeData(vsa >> 8);
    writeData(vsa & 0xFF);
    writeData(bfa >> 8);
    writeData(bfa & 0xFF);
}

void TFT_Driver_ST7796S::setScrollStart(uint16_t start) {
    writeCommand(ST7796S_VSCRSADD);
    writeData(start >> 8);
    writeData(start & 0xFF);
}

void TFT_Driver_ST7796S::setTearingEffect(bool enable, bool mode) {
    writeCommand(enable ? ST7796S_TEON : ST7796S_TEOFF);
    if (enable) {
        writeData(mode ? 0x01 : 0x00);
    }
}

void TFT_Driver_ST7796S::setPartialArea(uint16_t start, uint16_t end) {
    writeCommand(ST7796S_PTLAR);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_ST7796S::setIdleMode(bool enable) {
    writeCommand(enable ? ST7796S_IDMON : ST7796S_IDMOFF);
}

void TFT_Driver_ST7796S::setColorEnhancement(uint8_t mode) {
    writeCommand(ST7796S_DOCA);
    writeData(mode);
}

void TFT_Driver_ST7796S::setCommandAccess(uint8_t mode) {
    writeCommand(ST7796S_CSCON);
    writeData(mode);
}

} // namespace TFT_Runtime
