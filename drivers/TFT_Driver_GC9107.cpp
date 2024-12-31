#include <SPI.h>
#include "TFT_Driver_GC9107.h"

namespace TFT_Runtime {

TFT_Driver_GC9107::TFT_Driver_GC9107(Config& config) : TFT_Driver_Base(config) {
    _width = GC9107_TFTWIDTH;
    _height = GC9107_TFTHEIGHT;
}

void TFT_Driver_GC9107::init() {
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

    writeCommand(GC9107_SWRESET);    // Software reset
    delay(120);                      // Wait for reset to complete

    writeCommand(GC9107_SLPOUT);     // Exit Sleep
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
    writeCommand(GC9107_COLMOD);
    writeData(0x55);                 // 16-bit color

    // Memory Access Control
    writeCommand(GC9107_MADCTL);
    writeData(0x00);                 // Normal orientation

    // Display on
    writeCommand(GC9107_NORON);      // Normal display mode on
    delay(10);
    writeCommand(GC9107_DISPON);     // Display on
    delay(120);
}

void TFT_Driver_GC9107::initPowerSettings() {
    // Power Control 1
    writeCommand(GC9107_PWCTRL1);
    writeData(0xA4);
    writeData(0xA1);

    // Power Control 2
    writeCommand(GC9107_PWCTRL2);
    writeData(0x69);

    // VCOM Control
    writeCommand(GC9107_VCOMS);
    writeData(0x24);

    // VDVVRHEN
    writeCommand(GC9107_VDVVRHEN);
    writeData(0x01);
    writeData(0xFF);

    // VRH Set
    writeCommand(GC9107_VRHS);
    writeData(0x0B);

    // VDV Set
    writeCommand(GC9107_VDVS);
    writeData(0x20);

    // Frame Rate Control
    writeCommand(GC9107_FRCTRL2);
    writeData(0x0F);                 // 60Hz

    // Power Control 3
    writeCommand(GC9107_PWCTRL1);
    writeData(0xA4);
    writeData(0xA1);
}

void TFT_Driver_GC9107::initDisplaySettings() {
    // Porch Control
    writeCommand(GC9107_PORCTRL);
    writeData(0x0C);
    writeData(0x0C);
    writeData(0x00);
    writeData(0x33);
    writeData(0x33);

    // Gate Control
    writeCommand(GC9107_GCTRL);
    writeData(0x35);

    // VCOM Offset Control
    writeCommand(GC9107_VCMOFSET);
    writeData(0x19);

    // Display Function Control
    writeCommand(GC9107_LCMCTRL);
    writeData(0x2C);

    // VDV and VRH Command Enable
    writeCommand(GC9107_VDVVRHEN);
    writeData(0x01);
    writeData(0xFF);

    // VRH Set
    writeCommand(GC9107_VRHS);
    writeData(0x11);

    // VDV Set
    writeCommand(GC9107_VDVS);
    writeData(0x20);

    // Frame Rate Control
    writeCommand(GC9107_FRCTRL1);
    writeData(0x0F);                 // 60Hz
}

void TFT_Driver_GC9107::initGamma() {
    // Positive Voltage Gamma Control
    writeCommand(GC9107_PVGAMCTRL);
    writeData(0xD0);
    writeData(0x00);
    writeData(0x02);
    writeData(0x07);
    writeData(0x0B);
    writeData(0x1A);
    writeData(0x31);
    writeData(0x54);
    writeData(0x40);
    writeData(0x29);
    writeData(0x12);
    writeData(0x12);
    writeData(0x12);
    writeData(0x17);

    // Negative Voltage Gamma Control
    writeCommand(GC9107_NVGAMCTRL);
    writeData(0xD0);
    writeData(0x00);
    writeData(0x02);
    writeData(0x07);
    writeData(0x05);
    writeData(0x25);
    writeData(0x2D);
    writeData(0x44);
    writeData(0x45);
    writeData(0x1C);
    writeData(0x18);
    writeData(0x16);
    writeData(0x1C);
    writeData(0x1D);
}

void TFT_Driver_GC9107::writeCommand(uint8_t cmd) {
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

void TFT_Driver_GC9107::writeData(uint8_t data) {
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

void TFT_Driver_GC9107::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_GC9107::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(GC9107_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x00 : 0x08);
            _width = GC9107_TFTWIDTH;
            _height = GC9107_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x60 : 0x68);
            _width = GC9107_TFTHEIGHT;
            _height = GC9107_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0xC0 : 0xC8);
            _width = GC9107_TFTWIDTH;
            _height = GC9107_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xA0 : 0xA8);
            _width = GC9107_TFTHEIGHT;
            _height = GC9107_TFTWIDTH;
            break;
    }
}

void TFT_Driver_GC9107::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? GC9107_INVON : GC9107_INVOFF);
}

void TFT_Driver_GC9107::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(GC9107_CASET);
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);

    writeCommand(GC9107_RASET);
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);

    writeCommand(GC9107_RAMWR);
}

void TFT_Driver_GC9107::setScrollArea(uint16_t topFixed, uint16_t scrollArea, uint16_t bottomFixed) {
    writeCommand(GC9107_VSCRDEF);
    writeData(topFixed >> 8);
    writeData(topFixed & 0xFF);
    writeData(scrollArea >> 8);
    writeData(scrollArea & 0xFF);
    writeData(bottomFixed >> 8);
    writeData(bottomFixed & 0xFF);
}

void TFT_Driver_GC9107::setScrollStart(uint16_t start) {
    writeCommand(GC9107_VSCRSADD);
    writeData(start >> 8);
    writeData(start & 0xFF);
}

void TFT_Driver_GC9107::setIdleMode(bool enable) {
    writeCommand(enable ? GC9107_IDMON : GC9107_IDMOFF);
}

void TFT_Driver_GC9107::setPowerControl(uint8_t mode) {
    writeCommand(GC9107_PWCTRL1);
    writeData(mode);
}

void TFT_Driver_GC9107::setVCOM(uint8_t value) {
    writeCommand(GC9107_VCOMS);
    writeData(value);
}

void TFT_Driver_GC9107::setMemoryAccessControl(uint8_t mode) {
    writeCommand(GC9107_MADCTL);
    writeData(mode);
}

void TFT_Driver_GC9107::setPixelFormat(uint8_t format) {
    writeCommand(GC9107_COLMOD);
    writeData(format);
}

void TFT_Driver_GC9107::setGamma(uint8_t gamma) {
    writeCommand(GC9107_GAMSET);
    writeData(gamma);
}

void TFT_Driver_GC9107::setPositiveGamma(const uint8_t* gamma) {
    writeCommand(GC9107_PVGAMCTRL);
    for(uint8_t i = 0; i < 14; i++) {
        writeData(gamma[i]);
    }
}

void TFT_Driver_GC9107::setNegativeGamma(const uint8_t* gamma) {
    writeCommand(GC9107_NVGAMCTRL);
    for(uint8_t i = 0; i < 14; i++) {
        writeData(gamma[i]);
    }
}

void TFT_Driver_GC9107::setFrameRate(uint8_t divider, uint8_t rtna) {
    writeCommand(GC9107_FRCTRL2);
    writeData(divider);
    writeData(rtna);
}

void TFT_Driver_GC9107::setPartialArea(uint16_t start, uint16_t end) {
    writeCommand(GC9107_PTLAR);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_GC9107::setTearingEffect(bool enable, bool mode) {
    writeCommand(enable ? GC9107_TEON : GC9107_TEOFF);
    if (enable) {
        writeData(mode ? 0x01 : 0x00);
    }
}

void TFT_Driver_GC9107::enablePowerSave(bool enable) {
    writeCommand(GC9107_IDMON + !enable);  // IDMON/IDMOFF
}

uint32_t TFT_Driver_GC9107::readID() {
    writeCommand(GC9107_RDDID);
    uint32_t id = 0;
    id = readData() << 16;
    id |= readData() << 8;
    id |= readData();
    return id;
}

} // namespace TFT_Runtime
