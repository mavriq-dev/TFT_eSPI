#include <SPI.h>
#include "TFT_Driver_ILI9806.h"

namespace TFT_Runtime {

TFT_Driver_ILI9806::TFT_Driver_ILI9806(Config& config) : TFT_Driver_Base(config) {
    _width = ILI9806_TFTWIDTH;
    _height = ILI9806_TFTHEIGHT;
}

void TFT_Driver_ILI9806::init() {
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

    writeCommand(ILI9806_SWRESET);   // Software reset
    delay(120);                      // Wait for reset to complete

    writeCommand(ILI9806_SLPOUT);    // Exit Sleep
    delay(120);

    // Extended command set
    writeCommand(ILI9806_SETEXTC);
    writeData(0xFF);
    writeData(0x98);
    writeData(0x06);

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
    writeCommand(ILI9806_COLMOD);
    writeData(0x55);                 // 16-bit color

    // Memory Access Control
    writeCommand(ILI9806_MADCTL);
    writeData(0x00);                 // Row/Column addressing

    // Set MIPI control for high speed
    writeCommand(ILI9806_SETMIPI);
    writeData(0x03);                 // Enable high-speed mode

    // VCOM voltage setting
    writeCommand(ILI9806_SETVCOM);
    writeData(0x2C);                 // VCOM voltage
    writeData(0x2C);                 // VCOM offset

    // CABC (Content Adaptive Backlight Control)
    writeCommand(ILI9806_SETCABC);
    writeData(0x10);                 // Enable CABC
    writeData(0x40);                 // Still picture
    writeData(0x00);                 // Moving image
    writeData(0x00);                 // User interface

    // Set minimum brightness for CABC
    writeCommand(ILI9806_SETCABCMB);
    writeData(0x20);                 // Minimum brightness level

    // Display on
    writeCommand(ILI9806_DISPON);
    delay(120);
}

void TFT_Driver_ILI9806::initPowerSettings() {
    // Power Control 1
    writeCommand(ILI9806_SETPOWER);
    writeData(0x48);                 // AVDD: 6.6V
    writeData(0x12);                 // AVDD: 5.2V
    writeData(0x72);                 // VGH: 15V
    writeData(0x09);                 // VGL: -10V
    writeData(0x33);                 // DDVDH: 5.94V
    writeData(0x54);                 // Set pump clock frequency

    // VDD Setting
    writeCommand(ILI9806_SETVDD);
    writeData(0x3C);                 // VDD_18V
    writeData(0x34);                 // VGMP: 4.7V
    writeData(0x34);                 // VGSP: 0V

    // Power Control for Normal Mode
    writeCommand(0xC1);
    writeData(0x13);                 // AP[2:0]
    writeData(0x42);                 // DC[2:0]
    writeData(0x1B);                 // DC_DIV[1:0]
}

void TFT_Driver_ILI9806::initDisplaySettings() {
    // Display Timing Setting for Normal Mode
    writeCommand(ILI9806_SETDISP);
    writeData(0x00);                 // N_NW[7:0]
    writeData(0x58);                 // I_NW[7:0]
    writeData(0x56);                 // N_RTN[4:0]
    writeData(0x78);                 // I_RTN[4:0]
    writeData(0x00);                 // DIV[1:0]
    writeData(0x14);                 // DIV[1:0]

    // RGB Interface Setting
    writeCommand(ILI9806_SETRGB);
    writeData(0x80);                 // RGB interface control
    writeData(0x00);                 // RGB interface timing
    writeData(0x00);                 // RGB interface polarity

    // Frame Rate Control
    writeCommand(ILI9806_SETCYC);
    writeData(0x02);                 // 60Hz frame rate
    writeData(0x40);                 // Line inversion

    // Panel Characteristics
    writeCommand(ILI9806_SETPANEL);
    writeData(0x09);                 // Panel characteristics setting
}

void TFT_Driver_ILI9806::initGamma() {
    // Gamma Setting
    writeCommand(ILI9806_SETGAMMA);
    // Positive Gamma
    writeData(0x00);
    writeData(0x15);
    writeData(0x1D);
    writeData(0x0C);
    writeData(0x0F);
    writeData(0x0C);
    writeData(0x07);
    writeData(0x05);
    writeData(0x04);
    writeData(0x0E);
    writeData(0x12);
    writeData(0x0F);
    writeData(0x0C);
    writeData(0x07);
    writeData(0x05);
    writeData(0x08);
    // Negative Gamma
    writeData(0x00);
    writeData(0x15);
    writeData(0x1D);
    writeData(0x0C);
    writeData(0x0F);
    writeData(0x0C);
    writeData(0x07);
    writeData(0x05);
    writeData(0x04);
    writeData(0x0E);
    writeData(0x12);
    writeData(0x0F);
    writeData(0x0C);
    writeData(0x07);
    writeData(0x05);
    writeData(0x08);
}

void TFT_Driver_ILI9806::writeCommand(uint8_t cmd) {
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

void TFT_Driver_ILI9806::writeData(uint8_t data) {
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

void TFT_Driver_ILI9806::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_ILI9806::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(ILI9806_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x00 : 0x08);
            _width = ILI9806_TFTWIDTH;
            _height = ILI9806_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x60 : 0x68);
            _width = ILI9806_TFTHEIGHT;
            _height = ILI9806_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0xC0 : 0xC8);
            _width = ILI9806_TFTWIDTH;
            _height = ILI9806_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xA0 : 0xA8);
            _width = ILI9806_TFTHEIGHT;
            _height = ILI9806_TFTWIDTH;
            break;
    }
}

void TFT_Driver_ILI9806::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? ILI9806_INVON : ILI9806_INVOFF);
}

void TFT_Driver_ILI9806::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(ILI9806_CASET);
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);

    writeCommand(ILI9806_PASET);
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);

    writeCommand(ILI9806_RAMWR);
}

void TFT_Driver_ILI9806::setOscillator(uint8_t osc) {
    writeCommand(ILI9806_SETOSC);
    writeData(osc);
}

void TFT_Driver_ILI9806::setPowerControl(uint8_t vc, uint8_t bt, uint8_t dc) {
    writeCommand(ILI9806_SETPOWER);
    writeData(vc);
    writeData(bt);
    writeData(dc);
}

void TFT_Driver_ILI9806::setDisplayTiming(uint8_t rtna, uint8_t fpa, uint8_t bpa) {
    writeCommand(ILI9806_SETDISP);
    writeData(rtna);
    writeData(fpa);
    writeData(bpa);
}

void TFT_Driver_ILI9806::setRGBInterface(uint8_t rgb_if) {
    writeCommand(ILI9806_SETRGB);
    writeData(rgb_if);
}

void TFT_Driver_ILI9806::setCycle(uint8_t cycle) {
    writeCommand(ILI9806_SETCYC);
    writeData(cycle);
}

void TFT_Driver_ILI9806::setBacklightControl(uint8_t bl) {
    writeCommand(ILI9806_SETBGP);
    writeData(bl);
}

void TFT_Driver_ILI9806::setVCOMVoltage(uint8_t vcm) {
    writeCommand(ILI9806_SETVCOM);
    writeData(vcm);
}

void TFT_Driver_ILI9806::setExtendedCommands(bool enable) {
    writeCommand(ILI9806_SETEXTC);
    if (enable) {
        writeData(0xFF);
        writeData(0x98);
        writeData(0x06);
    }
}

void TFT_Driver_ILI9806::setMIPIControl(uint8_t mipi) {
    writeCommand(ILI9806_SETMIPI);
    writeData(mipi);
}

void TFT_Driver_ILI9806::setVDDSetting(uint8_t vdd) {
    writeCommand(ILI9806_SETVDD);
    writeData(vdd);
}

void TFT_Driver_ILI9806::setGamma(uint8_t gamma) {
    writeCommand(ILI9806_SETGAMMA);
    writeData(gamma);
}

void TFT_Driver_ILI9806::setPanelCharacteristics(uint8_t panel) {
    writeCommand(ILI9806_SETPANEL);
    writeData(panel);
}

void TFT_Driver_ILI9806::setCABC(uint8_t cabc) {
    writeCommand(ILI9806_SETCABC);
    writeData(cabc);
}

void TFT_Driver_ILI9806::setCABCMinBrightness(uint8_t min_bright) {
    writeCommand(ILI9806_SETCABCMB);
    writeData(min_bright);
}

void TFT_Driver_ILI9806::setDisplayOffset(uint16_t x, uint16_t y) {
    writeCommand(ILI9806_SETOFFSET);
    writeData(x >> 8);
    writeData(x & 0xFF);
    writeData(y >> 8);
    writeData(y & 0xFF);
}

void TFT_Driver_ILI9806::setCOMControl(uint8_t com) {
    writeCommand(ILI9806_SETCOM);
    writeData(com);
}

void TFT_Driver_ILI9806::setIdleMode(bool enable) {
    writeCommand(enable ? ILI9806_IDMON : ILI9806_IDMOFF);
}

void TFT_Driver_ILI9806::setInterface(uint8_t mode) {
    writeCommand(ILI9806_SETCOM);
    writeData(mode);
}

void TFT_Driver_ILI9806::setDisplayMode(uint8_t mode) {
    writeCommand(ILI9806_SETCOM);
    writeData(mode);
}

void TFT_Driver_ILI9806::setTearingEffect(bool enable, bool mode) {
    writeCommand(enable ? ILI9806_TEON : ILI9806_TEOFF);
    if (enable) {
        writeData(mode ? 0x01 : 0x00);
    }
}

void TFT_Driver_ILI9806::setPartialArea(uint16_t start, uint16_t end) {
    writeCommand(ILI9806_PTLAR);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_ILI9806::setPixelFormat(uint8_t format) {
    writeCommand(ILI9806_COLMOD);
    writeData(format);
}

void TFT_Driver_ILI9806::setMemoryAccessControl(uint8_t mode) {
    writeCommand(ILI9806_MADCTL);
    writeData(mode);
}

void TFT_Driver_ILI9806::setColumnAddress(uint16_t start, uint16_t end) {
    writeCommand(ILI9806_CASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_ILI9806::setPageAddress(uint16_t start, uint16_t end) {
    writeCommand(ILI9806_PASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_ILI9806::setGammaSet(uint8_t gamma_curve) {
    writeCommand(ILI9806_GAMSET);
    writeData(gamma_curve);
}

} // namespace TFT_Runtime
