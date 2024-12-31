#include <SPI.h>
#include "TFT_Driver_ST7789V2.h"

namespace TFT_Runtime {

TFT_Driver_ST7789V2::TFT_Driver_ST7789V2(Config& config) : TFT_Driver_Base(config) {
    _width = ST7789V2_TFTWIDTH;
    _height = ST7789V2_TFTHEIGHT;
    _colstart = 0;
    _rowstart = 0;
}

void TFT_Driver_ST7789V2::init() {
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

    writeCommand(ST7789V2_SWRESET);    // Software reset
    delay(120);                        // Wait for reset to complete

    writeCommand(ST7789V2_SLPOUT);     // Exit Sleep
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

    writeCommand(ST7789V2_INVON);      // Display Inversion On
    delay(10);

    writeCommand(ST7789V2_NORON);      // Normal Display Mode On
    delay(10);

    writeCommand(ST7789V2_DISPON);     // Display On
    delay(120);
}

void TFT_Driver_ST7789V2::initPowerSettings() {
    // Power Control 1
    writeCommand(ST7789V2_PWCTRL1);
    writeData(0xA4);
    writeData(0xA1);

    // VCOM Setting
    writeCommand(ST7789V2_VCOMS);
    writeData(0x24);                   // VCOM = 0.9V

    // VDVVRH Enable
    writeCommand(ST7789V2_VDVVRHEN);
    writeData(0x01);
    writeData(0xFF);

    // VRH Set
    writeCommand(ST7789V2_VRHS);
    writeData(0x0B);                   // VRH = 4.1V

    // VDV Set
    writeCommand(ST7789V2_VDVS);
    writeData(0x20);                   // VDV = 0V

    // Frame Rate Control
    writeCommand(ST7789V2_FRCTRL2);
    writeData(0x0F);                   // 60Hz

    // Power Control 2
    writeCommand(ST7789V2_PWRCTRL2);
    writeData(0x00);
    writeData(0x02);                   // Frequency control
}

void TFT_Driver_ST7789V2::initDisplaySettings() {
    // Memory Data Access Control
    writeCommand(ST7789V2_MADCTL);
    writeData(0x48);                   // Default rotation (0)

    // Interface Pixel Format
    writeCommand(ST7789V2_COLMOD);
    writeData(0x55);                   // 16-bit color

    // Porch Setting
    writeCommand(ST7789V2_PORCTRL);
    writeData(0x0C);
    writeData(0x0C);
    writeData(0x00);
    writeData(0x33);
    writeData(0x33);

    // Gate Control
    writeCommand(ST7789V2_GCTRL);
    writeData(0x35);                   // VGH = 13.26V, VGL = -10.43V

    // Enhanced Gate Control
    writeCommand(ST7789V2_GCTRL2);
    writeData(0x01);                   // Enhanced gate pump

    // VCMCTRL
    writeCommand(ST7789V2_VCMCTRL);
    writeData(0x2B);                   // VCOM control

    // LCM Control
    writeCommand(ST7789V2_LCMCTRL);
    writeData(0x2C);                   // XOR RGB, MX, MH

    // VDV and VRH Command Enable
    writeCommand(ST7789V2_VDVVRHEN);
    writeData(0x01);
    writeData(0xFF);
}

void TFT_Driver_ST7789V2::initGamma() {
    // Positive Voltage Gamma Control
    writeCommand(ST7789V2_PVGAMCTRL);
    writeData(0xD0);
    writeData(0x00);
    writeData(0x02);
    writeData(0x07);
    writeData(0x0A);
    writeData(0x28);
    writeData(0x32);
    writeData(0x44);
    writeData(0x42);
    writeData(0x06);
    writeData(0x0E);
    writeData(0x12);
    writeData(0x14);
    writeData(0x17);

    // Negative Voltage Gamma Control
    writeCommand(ST7789V2_NVGAMCTRL);
    writeData(0xD0);
    writeData(0x00);
    writeData(0x02);
    writeData(0x07);
    writeData(0x0A);
    writeData(0x28);
    writeData(0x31);
    writeData(0x54);
    writeData(0x47);
    writeData(0x0E);
    writeData(0x1C);
    writeData(0x17);
    writeData(0x1B);
    writeData(0x1E);
}

void TFT_Driver_ST7789V2::writeCommand(uint8_t cmd) {
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

void TFT_Driver_ST7789V2::writeData(uint8_t data) {
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

void TFT_Driver_ST7789V2::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_ST7789V2::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(ST7789V2_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x48 : 0x40);
            _width = ST7789V2_TFTWIDTH;
            _height = ST7789V2_TFTHEIGHT;
            _colstart = 0;
            _rowstart = 0;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x28 : 0x20);
            _width = ST7789V2_TFTHEIGHT;
            _height = ST7789V2_TFTWIDTH;
            _colstart = 0;
            _rowstart = 0;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0x88 : 0x80);
            _width = ST7789V2_TFTWIDTH;
            _height = ST7789V2_TFTHEIGHT;
            _colstart = 0;
            _rowstart = 0;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xE8 : 0xE0);
            _width = ST7789V2_TFTHEIGHT;
            _height = ST7789V2_TFTWIDTH;
            _colstart = 0;
            _rowstart = 0;
            break;
    }
}

void TFT_Driver_ST7789V2::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? ST7789V2_INVON : ST7789V2_INVOFF);
}

void TFT_Driver_ST7789V2::setVCOM(uint8_t value) {
    writeCommand(ST7789V2_VCOMS);
    writeData(value);
}

void TFT_Driver_ST7789V2::setVDV(uint8_t value) {
    writeCommand(ST7789V2_VDVS);
    writeData(value);
}

void TFT_Driver_ST7789V2::setVRHS(uint8_t value) {
    writeCommand(ST7789V2_VRHS);
    writeData(value);
}

void TFT_Driver_ST7789V2::enableCommandSet2(bool enable) {
    writeCommand(ST7789V2_CMD2EN);
    writeData(enable ? 0x5A : 0x00);
}

void TFT_Driver_ST7789V2::setEqualizerControl(uint8_t eq1, uint8_t eq2, uint8_t eq3) {
    writeCommand(ST7789V2_EQCTRL);
    writeData(eq1);
    writeData(eq2);
    writeData(eq3);
}

void TFT_Driver_ST7789V2::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(ST7789V2_CASET);      // Column addr set
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);             // Start col
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);             // End col

    writeCommand(ST7789V2_RASET);      // Row addr set
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);             // Start row
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);             // End row

    writeCommand(ST7789V2_RAMWR);      // Write to RAM
}

void TFT_Driver_ST7789V2::setWindowWithOffset(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    setWindow(x0 + _colstart, y0 + _rowstart, x1 + _colstart, y1 + _rowstart);
}

} // namespace TFT_Runtime
