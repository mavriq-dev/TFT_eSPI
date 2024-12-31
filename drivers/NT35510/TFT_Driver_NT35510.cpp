#include <SPI.h>
#include "TFT_Driver_NT35510.h"

namespace TFT_Runtime {

TFT_Driver_NT35510::TFT_Driver_NT35510(Config& config) : TFT_Driver_Base(config) {
    _width = NT35510_TFTWIDTH;
    _height = NT35510_TFTHEIGHT;
}

void TFT_Driver_NT35510::init() {
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

    writeCommand(NT35510_SWRESET);    // Software reset
    delay(120);                       // Wait for reset to complete

    writeCommand(NT35510_SLPOUT);     // Exit Sleep
    delay(120);

    // Extended command set
    writeCommand(NT35510_SETEXTC);
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

    // Set MIPI control
    writeCommand(NT35510_SETMIPI);
    writeData(0x03);                  // MIPI 4 lanes
    writeData(0x02);                  // Non-burst with sync pulses
    writeData(0x3C);                  // 4 cycles HS-LPX
    writeData(0x01);                  // 1 cycle HS prepare

    // Set pixel format
    writeCommand(NT35510_COLMOD);
    writeData(0x55);                  // 16-bit color

    // Set display mode
    writeCommand(NT35510_SETDISPLAY);
    writeData(0x08);                  // GON=1, DTE=0, D1-0=00
    writeData(0x02);                  // BP3-0=0010

    // Set panel characteristics
    writeCommand(NT35510_SETPANEL);
    writeData(0x02);                  // REV=0, BGR=1
    writeData(0x40);                  // Forward scan
    writeData(0x00);                  // SS=0 (1:1), SM=0 (seq)

    writeCommand(NT35510_NORON);      // Normal Display Mode ON
    delay(10);

    writeCommand(NT35510_DISPON);     // Display ON
    delay(120);
}

void TFT_Driver_NT35510::initPowerSettings() {
    // Power Control 1
    writeCommand(NT35510_SETPOWER);
    writeData(0x07);                  // VCI1 = 2.5V
    writeData(0x42);                  // DDVDH = 5.94V
    writeData(0x1C);                  // VREG1 = 4.625V

    // VCOM Control 1
    writeCommand(NT35510_SETVCOM1);
    writeData(0x00);                  // VCOMH = 4.250V
    writeData(0x1F);                  // VCOML = -1.500V

    // VCOM Control 2
    writeCommand(NT35510_SETVCOM2);
    writeData(0x00);                  // VCOMH = 4.250V
    writeData(0x1F);                  // VCOML = -1.500V

    // Power Control for Internal Used
    writeCommand(NT35510_SETPWD);
    writeData(0x01);                  // VGH = 14V, VGL = -9V
    writeData(0x02);                  // DDVDH = 5.94V
}

void TFT_Driver_NT35510::initDisplaySettings() {
    // Memory Access Control
    writeCommand(NT35510_MADCTL);
    writeData(0x48);                  // MX=0, MY=1, MV=0, ML=0, RGB=0

    // Display Timing Setting for Normal Mode
    writeCommand(NT35510_SETCYC);
    writeData(0x15);                  // DIV1[3:0] = 5
    writeData(0x03);                  // RTN1[4:0] = 3
    writeData(0x08);                  // BP1[7:0] = 8
    writeData(0x08);                  // FP1[7:0] = 8

    // RGB Interface Setting
    writeCommand(NT35510_SETRGB);
    writeData(0x00);                  // RGB interface
    writeData(0x00);                  // DE polarity
    writeData(0x06);                  // VSYNC polarity

    // Display Inversion Control
    writeCommand(NT35510_INVOFF);     // Display inversion off
}

void TFT_Driver_NT35510::initGamma() {
    writeCommand(NT35510_SETGAMMA);
    // Gamma settings (Positive)
    writeData(0x00);
    writeData(0x32);
    writeData(0x36);
    writeData(0x45);
    writeData(0x06);
    writeData(0x16);
    writeData(0x37);
    writeData(0x75);
    writeData(0x77);
    writeData(0x54);
    writeData(0x0C);
    writeData(0x00);
    
    // Gamma settings (Negative)
    writeData(0x00);
    writeData(0x32);
    writeData(0x36);
    writeData(0x45);
    writeData(0x06);
    writeData(0x16);
    writeData(0x37);
    writeData(0x75);
    writeData(0x77);
    writeData(0x54);
    writeData(0x0C);
    writeData(0x00);
}

void TFT_Driver_NT35510::writeCommand(uint8_t cmd) {
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

void TFT_Driver_NT35510::writeData(uint8_t data) {
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

void TFT_Driver_NT35510::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_NT35510::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(NT35510_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x48 : 0x40);
            _width = NT35510_TFTWIDTH;
            _height = NT35510_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x28 : 0x20);
            _width = NT35510_TFTHEIGHT;
            _height = NT35510_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0x88 : 0x80);
            _width = NT35510_TFTWIDTH;
            _height = NT35510_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xE8 : 0xE0);
            _width = NT35510_TFTHEIGHT;
            _height = NT35510_TFTWIDTH;
            break;
    }
}

void TFT_Driver_NT35510::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? NT35510_INVON : NT35510_INVOFF);
}

void TFT_Driver_NT35510::setVCOM(uint8_t value) {
    writeCommand(NT35510_SETVCOM1);
    writeData(value);
}

void TFT_Driver_NT35510::setPanelSettings(uint8_t mode) {
    writeCommand(NT35510_SETPANEL);
    writeData(mode);
}

void TFT_Driver_NT35510::setGamma(uint8_t gamma) {
    writeCommand(NT35510_SETGAMMA);
    writeData(gamma);
}

void TFT_Driver_NT35510::setCABC(uint8_t mode) {
    writeCommand(NT35510_SETCABC);
    writeData(mode);
}

void TFT_Driver_NT35510::setMIPIControl(bool enable) {
    writeCommand(NT35510_SETMIPI);
    writeData(enable ? 0x03 : 0x00);
}

void TFT_Driver_NT35510::setDisplayMode(uint8_t mode) {
    writeCommand(NT35510_SETDISPLAY);
    writeData(mode);
}

void TFT_Driver_NT35510::setBacklightControl(uint8_t level) {
    writeCommand(NT35510_SETCABCMB);
    writeData(level);
}

void TFT_Driver_NT35510::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(NT35510_CASET);      // Column addr set
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);             // Start col
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);             // End col

    writeCommand(NT35510_PASET);      // Row addr set
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);             // Start row
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);             // End row

    writeCommand(NT35510_RAMWR);      // Write to RAM
}

} // namespace TFT_Runtime
