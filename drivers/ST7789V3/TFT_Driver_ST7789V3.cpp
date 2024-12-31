#include <SPI.h>
#include "TFT_Driver_ST7789V3.h"

namespace TFT_Runtime {

TFT_Driver_ST7789V3::TFT_Driver_ST7789V3(Config& config) : TFT_Driver_Base(config) {
    _width = ST7789V3_TFTWIDTH;
    _height = ST7789V3_TFTHEIGHT;
    _colstart = 0;
    _rowstart = 0;
}

void TFT_Driver_ST7789V3::init() {
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

    writeCommand(ST7789V3_SWRESET);    // Software reset
    delay(120);                        // Wait for reset to complete

    writeCommand(ST7789V3_SLPOUT);     // Exit Sleep
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

    // Additional V3 features
    // Enable enhanced features
    writeCommand(ST7789V3_SPI2EN);
    writeData(0x01);                   // Enable SPI2 interface

    // Set gate control
    writeCommand(ST7789V3_GATECTRL);
    writeData(0x45);                   // 480 lines
    writeData(0x00);
    writeData(0x00);
    writeData(0x00);

    // Set equalizer
    writeCommand(ST7789V3_EQCTRL);
    writeData(0x00);
    writeData(0x00);
    writeData(0x00);

    // Set power control
    writeCommand(ST7789V3_PWCTRL2);
    writeData(0x00);
    writeData(0xC1);
    writeData(0x30);

    writeCommand(ST7789V3_COLMOD);     // Set color mode
    writeData(0x55);                   // 16-bit color

    writeCommand(ST7789V3_MADCTL);     // Memory Access Control
    writeData(0x00);                   // Row/Column addressing

    writeCommand(ST7789V3_INVON);      // Display Inversion On
    delay(10);

    writeCommand(ST7789V3_NORON);      // Normal Display Mode On
    delay(10);

    writeCommand(ST7789V3_DISPON);     // Display On
    delay(120);
}

void TFT_Driver_ST7789V3::initPowerSettings() {
    // Power Control 1
    writeCommand(ST7789V3_PWCTRL1);
    writeData(0xA4);
    writeData(0xA1);

    // VCOM Setting
    writeCommand(ST7789V3_VCOMS);
    writeData(0x24);                   // VCOM = 0.9V

    // VDVVRH Enable
    writeCommand(ST7789V3_VDVVRHEN);
    writeData(0x01);
    writeData(0xFF);

    // VRH Set
    writeCommand(ST7789V3_VRHS);
    writeData(0x0B);                   // VRH = 4.1V

    // VDV Set
    writeCommand(ST7789V3_VDVS);
    writeData(0x20);                   // VDV = 0V

    // Frame Rate Control
    writeCommand(ST7789V3_FRCTRL2);
    writeData(0x0F);                   // 60Hz
}

void TFT_Driver_ST7789V3::initDisplaySettings() {
    // Porch Setting
    writeCommand(ST7789V3_PORCTRL);
    writeData(0x0C);
    writeData(0x0C);
    writeData(0x00);
    writeData(0x33);
    writeData(0x33);

    // Gate Control
    writeCommand(ST7789V3_GCTRL);
    writeData(0x35);                   // VGH = 13.26V, VGL = -10.43V

    // LCM Control
    writeCommand(ST7789V3_LCMCTRL);
    writeData(0x2C);                   // XOR RGB, MX, MH

    // VDV and VRH Command Enable
    writeCommand(ST7789V3_VDVVRHEN);
    writeData(0x01);
    writeData(0xFF);
}

void TFT_Driver_ST7789V3::initGamma() {
    // Positive Voltage Gamma Control
    writeCommand(ST7789V3_PVGAMCTRL);
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
    writeCommand(ST7789V3_NVGAMCTRL);
    writeData(0xD0);
    writeData(0x00);
    writeData(0x02);
    writeData(0x07);
    writeData(0x05);
    writeData(0x25);
    writeData(0x2D);
    writeData(0x44);
    writeData(0x44);
    writeData(0x1C);
    writeData(0x18);
    writeData(0x16);
    writeData(0x1C);
    writeData(0x1D);
}

void TFT_Driver_ST7789V3::writeCommand(uint8_t cmd) {
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

void TFT_Driver_ST7789V3::writeData(uint8_t data) {
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

void TFT_Driver_ST7789V3::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_ST7789V3::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(ST7789V3_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x00 : 0x08);
            _width = ST7789V3_TFTWIDTH;
            _height = ST7789V3_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x60 : 0x68);
            _width = ST7789V3_TFTHEIGHT;
            _height = ST7789V3_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0xC0 : 0xC8);
            _width = ST7789V3_TFTWIDTH;
            _height = ST7789V3_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xA0 : 0xA8);
            _width = ST7789V3_TFTHEIGHT;
            _height = ST7789V3_TFTWIDTH;
            break;
    }
}

void TFT_Driver_ST7789V3::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? ST7789V3_INVON : ST7789V3_INVOFF);
}

void TFT_Driver_ST7789V3::setVCOM(uint8_t value) {
    writeCommand(ST7789V3_VCOMS);
    writeData(value);
}

void TFT_Driver_ST7789V3::setVDV(uint8_t value) {
    writeCommand(ST7789V3_VDVS);
    writeData(value);
}

void TFT_Driver_ST7789V3::setVRHS(uint8_t value) {
    writeCommand(ST7789V3_VRHS);
    writeData(value);
}

void TFT_Driver_ST7789V3::enableSPI2(bool enable) {
    writeCommand(ST7789V3_SPI2EN);
    writeData(enable ? 0x01 : 0x00);
}

void TFT_Driver_ST7789V3::setEqualizer(uint8_t eq1, uint8_t eq2, uint8_t eq3) {
    writeCommand(ST7789V3_EQCTRL);
    writeData(eq1);
    writeData(eq2);
    writeData(eq3);
}

void TFT_Driver_ST7789V3::setGamma(uint8_t gamma) {
    writeCommand(ST7789V3_GAMSET);
    writeData(gamma);
}

void TFT_Driver_ST7789V3::setDigitalGammaLUT(bool enable) {
    if (enable) {
        writeCommand(ST7789V3_DGMLUTR);
        for (int i = 0; i < 64; i++) {
            writeData(i * 4);  // Linear gamma curve
        }
        writeCommand(ST7789V3_DGMLUTB);
        for (int i = 0; i < 64; i++) {
            writeData(i * 4);  // Linear gamma curve
        }
    }
}

void TFT_Driver_ST7789V3::setIdleMode(bool enable) {
    writeCommand(enable ? ST7789V3_IDMON : ST7789V3_IDMOFF);
}

void TFT_Driver_ST7789V3::setGateControl(uint8_t lines, uint8_t fine) {
    writeCommand(ST7789V3_GATECTRL);
    writeData(lines);
    writeData(fine);
}

void TFT_Driver_ST7789V3::setScrollArea(uint16_t topFixed, uint16_t scrollArea, uint16_t bottomFixed) {
    writeCommand(ST7789V3_VSCRDEF);
    writeData(topFixed >> 8);
    writeData(topFixed & 0xFF);
    writeData(scrollArea >> 8);
    writeData(scrollArea & 0xFF);
    writeData(bottomFixed >> 8);
    writeData(bottomFixed & 0xFF);
}

void TFT_Driver_ST7789V3::setScrollStart(uint16_t start) {
    writeCommand(ST7789V3_VSCRSADD);
    writeData(start >> 8);
    writeData(start & 0xFF);
}

void TFT_Driver_ST7789V3::setPartialArea(uint16_t start, uint16_t end) {
    writeCommand(ST7789V3_PTLAR);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_ST7789V3::setPowerControl(uint8_t mode) {
    writeCommand(ST7789V3_PWCTRL1);
    writeData(mode);
}

void TFT_Driver_ST7789V3::setDisplayControl(uint8_t mode) {
    writeCommand(ST7789V3_LCMCTRL);
    writeData(mode);
}

void TFT_Driver_ST7789V3::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(ST7789V3_CASET);      // Column addr set
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);             // Start col
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);             // End col

    writeCommand(ST7789V3_RASET);      // Row addr set
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);             // Start row
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);             // End row

    writeCommand(ST7789V3_RAMWR);      // Write to RAM
}

void TFT_Driver_ST7789V3::setWindowWithOffset(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    setWindow(x0 + _colstart, y0 + _rowstart, x1 + _colstart, y1 + _rowstart);
}

} // namespace TFT_Runtime
