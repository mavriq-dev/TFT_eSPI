#include <SPI.h>
#include "TFT_Driver_ILI9327.h"

namespace TFT_Runtime {

TFT_Driver_ILI9327::TFT_Driver_ILI9327(Config& config) : TFT_Driver_Base(config) {
    _width = ILI9327_TFTWIDTH;
    _height = ILI9327_TFTHEIGHT;
}

void TFT_Driver_ILI9327::init() {
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

    writeCommand(ILI9327_SWRESET);   // Software reset
    delay(120);                      // Wait for reset to complete

    writeCommand(ILI9327_SLPOUT);    // Exit Sleep
    delay(120);

    // Extended command set
    writeCommand(ILI9327_SETEXTC);
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
    writeCommand(ILI9327_COLMOD);
    writeData(0x55);                 // 16-bit color

    // Memory Access Control
    writeCommand(ILI9327_MADCTL);
    writeData(0x00);                 // Row/Column addressing

    // Set GIP (Gate In Panel) control
    writeCommand(ILI9327_SETGIP);
    writeData(0x01);                 // GIP 1
    writeData(0x02);                 // GIP 2
    writeData(0x02);                 // GIP 3

    // VCOM voltage setting
    writeCommand(ILI9327_SETVCOM);
    writeData(0x3C);                 // VCOM voltage
    writeData(0x3C);                 // VCOM offset

    // Display on
    writeCommand(ILI9327_DISPON);
    delay(120);
}

void TFT_Driver_ILI9327::initPowerSettings() {
    // Power Control 1
    writeCommand(ILI9327_SETPWR1);
    writeData(0x00);                 // VRH1
    writeData(0x12);                 // VRH2
    writeData(0x12);                 // VRH3
    writeData(0x12);                 // VRH4
    writeData(0xC3);                 // BT

    // Power Control 2
    writeCommand(ILI9327_SETPWR2);
    writeData(0x44);                 // VGH/VGL protection
    writeData(0x02);                 // VGL protection level

    // Power Control 3
    writeCommand(ILI9327_SETPWR3);
    writeData(0x33);                 // Operational amplifier current
    writeData(0x33);                 // Booster efficiency
    writeData(0x33);                 // Booster frequency

    // Power Control 4
    writeCommand(ILI9327_SETPWR4);
    writeData(0x0A);                 // GVDD level

    // Power Control 5
    writeCommand(ILI9327_SETPWR5);
    writeData(0x28);                 // VGH level
    writeData(0x3C);                 // VGL level
}

void TFT_Driver_ILI9327::initDisplaySettings() {
    // Display Timing Setting
    writeCommand(ILI9327_SETCOM);
    writeData(0x0A);                 // Non-overlap period
    writeData(0xA2);                 // Source bias current
    writeData(0x27);                 // Gate EQ/timing
    writeData(0x00);                 // Source/Gate start position

    // RGB Interface Setting
    writeCommand(ILI9327_SETRGB);
    writeData(0x80);                 // RGB interface control
    writeData(0x00);                 // RGB interface timing
    writeData(0x00);                 // RGB interface polarity

    // Frame Rate Control
    writeCommand(ILI9327_SETCYC);
    writeData(0x08);                 // 65Hz frame rate
    writeData(0x02);                 // Line inversion

    // Panel Characteristics
    writeCommand(ILI9327_SETPANEL);
    writeData(0x09);                 // Panel characteristics setting
}

void TFT_Driver_ILI9327::initGamma() {
    // Gamma Setting
    writeCommand(ILI9327_SETGAMMA);
    // Positive Gamma
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
    // Negative Gamma
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

void TFT_Driver_ILI9327::writeCommand(uint8_t cmd) {
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

void TFT_Driver_ILI9327::writeData(uint8_t data) {
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

void TFT_Driver_ILI9327::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_ILI9327::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(ILI9327_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x00 : 0x08);
            _width = ILI9327_TFTWIDTH;
            _height = ILI9327_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x60 : 0x68);
            _width = ILI9327_TFTHEIGHT;
            _height = ILI9327_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0xC0 : 0xC8);
            _width = ILI9327_TFTWIDTH;
            _height = ILI9327_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xA0 : 0xA8);
            _width = ILI9327_TFTHEIGHT;
            _height = ILI9327_TFTWIDTH;
            break;
    }
}

void TFT_Driver_ILI9327::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? ILI9327_INVON : ILI9327_INVOFF);
}

void TFT_Driver_ILI9327::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(ILI9327_CASET);
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);

    writeCommand(ILI9327_PASET);
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);

    writeCommand(ILI9327_RAMWR);
}

void TFT_Driver_ILI9327::setOscillator(uint8_t osc) {
    writeCommand(ILI9327_SETOSC);
    writeData(osc);
}

void TFT_Driver_ILI9327::setPowerControl1(uint8_t vrh, uint8_t vc) {
    writeCommand(ILI9327_SETPWR1);
    writeData(vrh);
    writeData(vc);
}

void TFT_Driver_ILI9327::setPowerControl2(uint8_t bt, uint8_t apo) {
    writeCommand(ILI9327_SETPWR2);
    writeData(bt);
    writeData(apo);
}

void TFT_Driver_ILI9327::setPowerControl3(uint8_t dc, uint8_t ap) {
    writeCommand(ILI9327_SETPWR3);
    writeData(dc);
    writeData(ap);
}

void TFT_Driver_ILI9327::setPowerControl4(uint8_t gvdd) {
    writeCommand(ILI9327_SETPWR4);
    writeData(gvdd);
}

void TFT_Driver_ILI9327::setPowerControl5(uint8_t vgh, uint8_t vgl) {
    writeCommand(ILI9327_SETPWR5);
    writeData(vgh);
    writeData(vgl);
}

void TFT_Driver_ILI9327::setRGBInterface(uint8_t rgb_if) {
    writeCommand(ILI9327_SETRGB);
    writeData(rgb_if);
}

void TFT_Driver_ILI9327::setCycle(uint8_t cycle) {
    writeCommand(ILI9327_SETCYC);
    writeData(cycle);
}

void TFT_Driver_ILI9327::setDisplayControl(uint8_t mode) {
    writeCommand(ILI9327_SETCOM);
    writeData(mode);
}

void TFT_Driver_ILI9327::setVCOMVoltage(uint8_t vcm) {
    writeCommand(ILI9327_SETVCOM);
    writeData(vcm);
}

void TFT_Driver_ILI9327::setGamma(uint8_t gamma) {
    writeCommand(ILI9327_SETGAMMA);
    writeData(gamma);
}

void TFT_Driver_ILI9327::setPanelCharacteristics(uint8_t panel) {
    writeCommand(ILI9327_SETPANEL);
    writeData(panel);
}

void TFT_Driver_ILI9327::setIdleMode(bool enable) {
    writeCommand(enable ? ILI9327_IDMON : ILI9327_IDMOFF);
}

void TFT_Driver_ILI9327::setInterface(uint8_t mode) {
    writeCommand(ILI9327_SETCOM);
    writeData(mode);
}

void TFT_Driver_ILI9327::setDisplayMode(uint8_t mode) {
    writeCommand(ILI9327_SETCOM);
    writeData(mode);
}

void TFT_Driver_ILI9327::setTearingEffect(bool enable, bool mode) {
    writeCommand(enable ? ILI9327_TEON : ILI9327_TEOFF);
    if (enable) {
        writeData(mode ? 0x01 : 0x00);
    }
}

void TFT_Driver_ILI9327::setPartialArea(uint16_t start, uint16_t end) {
    writeCommand(ILI9327_PTLAR);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_ILI9327::setPixelFormat(uint8_t format) {
    writeCommand(ILI9327_COLMOD);
    writeData(format);
}

void TFT_Driver_ILI9327::setMemoryAccessControl(uint8_t mode) {
    writeCommand(ILI9327_MADCTL);
    writeData(mode);
}

void TFT_Driver_ILI9327::setColumnAddress(uint16_t start, uint16_t end) {
    writeCommand(ILI9327_CASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_ILI9327::setPageAddress(uint16_t start, uint16_t end) {
    writeCommand(ILI9327_PASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_ILI9327::setGammaSet(uint8_t gamma_curve) {
    writeCommand(ILI9327_GAMSET);
    writeData(gamma_curve);
}

void TFT_Driver_ILI9327::setExtendedCommands(bool enable) {
    writeCommand(ILI9327_SETEXTC);
    if (enable) {
        writeData(0xFF);
        writeData(0x83);
        writeData(0x57);
    }
}

void TFT_Driver_ILI9327::setGIPControl(uint8_t gip) {
    writeCommand(ILI9327_SETGIP);
    writeData(gip);
}

} // namespace TFT_Runtime
