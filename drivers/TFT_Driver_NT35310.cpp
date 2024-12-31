#include <SPI.h>
#include "TFT_Driver_NT35310.h"

namespace TFT_Runtime {

TFT_Driver_NT35310::TFT_Driver_NT35310(Config& config) : TFT_Driver_Base(config) {
    _width = NT35310_TFTWIDTH;
    _height = NT35310_TFTHEIGHT;
}

void TFT_Driver_NT35310::init() {
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

    writeCommand(NT35310_SWRESET);   // Software reset
    delay(120);                      // Wait for reset to complete

    writeCommand(NT35310_SLPOUT);    // Exit Sleep
    delay(120);

    // Extended command set
    writeCommand(NT35310_SETEXTC);
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
    writeCommand(NT35310_COLMOD);
    writeData(0x55);                 // 16-bit color

    // Memory Access Control
    writeCommand(NT35310_MADCTL);
    writeData(0x00);                 // Row/Column addressing

    // Set MIPI control
    writeCommand(NT35310_SETMIPI);
    writeData(0x03);                 // Enable high-speed mode

    // VCOM voltage setting
    writeCommand(NT35310_SETVCOM);
    writeData(0x3E);                 // VCOM voltage
    writeData(0x28);                 // VCOM offset

    // Display on
    writeCommand(NT35310_DISPON);
    delay(120);
}

void TFT_Driver_NT35310::initPowerSettings() {
    // Power Control 1
    writeCommand(NT35310_SETPWR1);
    writeData(0x00);                 // AVDD: 6.0V
    writeData(0x12);                 // AVDD: 5.2V
    writeData(0x12);                 // VGH: 14V
    writeData(0x12);                 // VGL: -7.35V
    writeData(0xC3);                 // DDVDH: 5.94V

    // Power Control 2
    writeCommand(NT35310_SETPWR2);
    writeData(0x44);                 // VGH/VGL protection
    writeData(0x02);                 // VGL protection level

    // Power Control 3
    writeCommand(NT35310_SETPWR3);
    writeData(0x33);                 // Operational amplifier current
    writeData(0x33);                 // Booster efficiency
    writeData(0x33);                 // Booster frequency
}

void TFT_Driver_NT35310::initDisplaySettings() {
    // Display Timing Setting
    writeCommand(NT35310_SETCOM);
    writeData(0x0A);                 // Non-overlap period
    writeData(0xA2);                 // Source bias current
    writeData(0x27);                 // Gate EQ/timing
    writeData(0x00);                 // Source/Gate start position

    // RGB Interface Setting
    writeCommand(NT35310_SETRGB);
    writeData(0x80);                 // RGB interface control
    writeData(0x00);                 // RGB interface timing
    writeData(0x00);                 // RGB interface polarity

    // Frame Rate Control
    writeCommand(NT35310_SETCYC);
    writeData(0x08);                 // 65Hz frame rate
    writeData(0x02);                 // Line inversion

    // Panel Characteristics
    writeCommand(NT35310_SETPANEL);
    writeData(0x09);                 // Panel characteristics setting
}

void TFT_Driver_NT35310::initGamma() {
    // Gamma Setting
    writeCommand(NT35310_SETGAMMA);
    // Positive Gamma
    writeData(0x00);
    writeData(0x07);
    writeData(0x0F);
    writeData(0x0D);
    writeData(0x1B);
    writeData(0x0A);
    writeData(0x3C);
    writeData(0x78);
    writeData(0x4A);
    writeData(0x07);
    writeData(0x0E);
    writeData(0x09);
    writeData(0x1B);
    writeData(0x1E);
    writeData(0x0F);
    // Negative Gamma
    writeData(0x00);
    writeData(0x22);
    writeData(0x24);
    writeData(0x2A);
    writeData(0x29);
    writeData(0x2B);
    writeData(0x3C);
    writeData(0x78);
    writeData(0x4A);
    writeData(0x07);
    writeData(0x0E);
    writeData(0x09);
    writeData(0x1B);
    writeData(0x1E);
    writeData(0x0F);
}

void TFT_Driver_NT35310::writeCommand(uint8_t cmd) {
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

void TFT_Driver_NT35310::writeData(uint8_t data) {
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

void TFT_Driver_NT35310::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_NT35310::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(NT35310_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x00 : 0x08);
            _width = NT35310_TFTWIDTH;
            _height = NT35310_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x60 : 0x68);
            _width = NT35310_TFTHEIGHT;
            _height = NT35310_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0xC0 : 0xC8);
            _width = NT35310_TFTWIDTH;
            _height = NT35310_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xA0 : 0xA8);
            _width = NT35310_TFTHEIGHT;
            _height = NT35310_TFTWIDTH;
            break;
    }
}

void TFT_Driver_NT35310::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? NT35310_INVON : NT35310_INVOFF);
}

void TFT_Driver_NT35310::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(NT35310_CASET);
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);

    writeCommand(NT35310_PASET);
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);

    writeCommand(NT35310_RAMWR);
}

void TFT_Driver_NT35310::setOscillator(uint8_t osc) {
    writeCommand(NT35310_SETOSC);
    writeData(osc);
}

void TFT_Driver_NT35310::setPowerControl1(uint8_t vrh, uint8_t vc) {
    writeCommand(NT35310_SETPWR1);
    writeData(vrh);
    writeData(vc);
}

void TFT_Driver_NT35310::setPowerControl2(uint8_t bt, uint8_t apo) {
    writeCommand(NT35310_SETPWR2);
    writeData(bt);
    writeData(apo);
}

void TFT_Driver_NT35310::setPowerControl3(uint8_t dc, uint8_t ap) {
    writeCommand(NT35310_SETPWR3);
    writeData(dc);
    writeData(ap);
}

void TFT_Driver_NT35310::setRGBInterface(uint8_t rgb_if) {
    writeCommand(NT35310_SETRGB);
    writeData(rgb_if);
}

void TFT_Driver_NT35310::setCycle(uint8_t cycle) {
    writeCommand(NT35310_SETCYC);
    writeData(cycle);
}

void TFT_Driver_NT35310::setDisplayControl(uint8_t mode) {
    writeCommand(NT35310_SETCOM);
    writeData(mode);
}

void TFT_Driver_NT35310::setVCOMVoltage(uint8_t vcm) {
    writeCommand(NT35310_SETVCOM);
    writeData(vcm);
}

void TFT_Driver_NT35310::setGamma(uint8_t gamma) {
    writeCommand(NT35310_SETGAMMA);
    writeData(gamma);
}

void TFT_Driver_NT35310::setPanelCharacteristics(uint8_t panel) {
    writeCommand(NT35310_SETPANEL);
    writeData(panel);
}

void TFT_Driver_NT35310::setIdleMode(bool enable) {
    writeCommand(enable ? NT35310_IDMON : NT35310_IDMOFF);
}

void TFT_Driver_NT35310::setInterface(uint8_t mode) {
    writeCommand(NT35310_SETCOM);
    writeData(mode);
}

void TFT_Driver_NT35310::setDisplayMode(uint8_t mode) {
    writeCommand(NT35310_SETCOM);
    writeData(mode);
}

void TFT_Driver_NT35310::setTearingEffect(bool enable, bool mode) {
    writeCommand(enable ? NT35310_TEON : NT35310_TEOFF);
    if (enable) {
        writeData(mode ? 0x01 : 0x00);
    }
}

void TFT_Driver_NT35310::setPartialArea(uint16_t start, uint16_t end) {
    writeCommand(NT35310_PTLAR);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_NT35310::setPixelFormat(uint8_t format) {
    writeCommand(NT35310_COLMOD);
    writeData(format);
}

void TFT_Driver_NT35310::setMemoryAccessControl(uint8_t mode) {
    writeCommand(NT35310_MADCTL);
    writeData(mode);
}

void TFT_Driver_NT35310::setColumnAddress(uint16_t start, uint16_t end) {
    writeCommand(NT35310_CASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_NT35310::setPageAddress(uint16_t start, uint16_t end) {
    writeCommand(NT35310_PASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_NT35310::setGammaSet(uint8_t gamma_curve) {
    writeCommand(NT35310_GAMSET);
    writeData(gamma_curve);
}

void TFT_Driver_NT35310::setExtendedCommands(bool enable) {
    writeCommand(NT35310_SETEXTC);
    if (enable) {
        writeData(0xFF);
        writeData(0x83);
        writeData(0x57);
    }
}

void TFT_Driver_NT35310::setMIPIControl(uint8_t mipi) {
    writeCommand(NT35310_SETMIPI);
    writeData(mipi);
}

} // namespace TFT_Runtime
