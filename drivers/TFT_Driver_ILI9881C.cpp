#include <SPI.h>
#include "TFT_Driver_ILI9881C.h"

namespace TFT_Runtime {

TFT_Driver_ILI9881C::TFT_Driver_ILI9881C(Config& config) : TFT_Driver_Base(config) {
    _width = ILI9881C_TFTWIDTH;
    _height = ILI9881C_TFTHEIGHT;
}

void TFT_Driver_ILI9881C::init() {
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

    writeCommand(ILI9881C_SWRESET);    // Software reset
    delay(120);                        // Wait for reset to complete

    writeCommand(ILI9881C_SLPOUT);     // Exit Sleep
    delay(120);

    // Set password for extended commands
    writeCommand(ILI9881C_SETEXTC);
    writeData(0xFF);
    writeData(0x98);
    writeData(0x81);
    writeData(0x03);

    // Power settings
    initPowerSettings();
    delay(10);

    // Display settings
    initDisplaySettings();
    delay(10);

    // GIP settings
    initGIP();
    delay(10);

    // Gamma settings
    initGamma();
    delay(10);

    // Set pixel format - 24bit/pixel
    writeCommand(ILI9881C_COLMOD);
    writeData(0x77);                   // 24-bit color

    // Memory Access Control
    writeCommand(ILI9881C_MADCTL);
    writeData(0x00);                   // Normal orientation

    // Set MIPI control
    writeCommand(ILI9881C_SETMIPI);
    writeData(0x03);                   // MIPI 4 lanes

    // Set VCOM voltage
    writeCommand(ILI9881C_SETVCOM);
    writeData(0x2C);
    writeData(0x2C);

    // Display on
    writeCommand(ILI9881C_DISPON);
    delay(120);
}

void TFT_Driver_ILI9881C::initPowerSettings() {
    // Power Control 1
    writeCommand(ILI9881C_SETPOWER);
    writeData(0x53);                   // AVDD: 6.6V
    writeData(0x14);                   // AVDD: 5.2V
    writeData(0x52);                   // VGH: 15V
    writeData(0x0C);                   // VGL: -10V
    writeData(0x03);                   // DDVDH: 5.94V
    writeData(0x04);                   // Set pump clock frequency

    // Power Control 2
    writeCommand(ILI9881C_SETPOWER2);
    writeData(0x01);                   // AP[2:0]
    writeData(0x70);                   // DC[2:0]
    writeData(0x1D);                   // DC_DIV[1:0]

    // Power Control 3
    writeCommand(ILI9881C_SETPOWER3);
    writeData(0x01);                   // GVDD: 4.7V
    writeData(0x70);                   // NGVDD: -4.7V
    writeData(0x1D);                   // VGHO: 15V
}

void TFT_Driver_ILI9881C::initDisplaySettings() {
    // Display Timing Setting
    writeCommand(ILI9881C_SETPANEL);
    writeData(0x00);                   // N_NW[7:0]
    writeData(0x14);                   // I_NW[7:0]
    writeData(0x13);                   // N_RTN[4:0]
    writeData(0x12);                   // I_RTN[4:0]
    writeData(0x11);                   // DIV[1:0]
    writeData(0x10);                   // DIV[1:0]

    // RGB Interface Setting
    writeCommand(ILI9881C_SETRGB);
    writeData(0x00);                   // RGB interface control
    writeData(0x00);                   // RGB interface timing
    writeData(0x06);                   // RGB interface polarity

    // Frame Rate Control
    writeCommand(ILI9881C_SETCYC);
    writeData(0x01);                   // 60Hz frame rate
    writeData(0x02);                   // Line inversion
}

void TFT_Driver_ILI9881C::initGIP() {
    // GIP 1
    writeCommand(ILI9881C_SETGIP1);
    writeData(0x00);
    writeData(0x09);
    writeData(0x0F);
    writeData(0x0E);
    writeData(0x07);
    writeData(0x10);
    writeData(0x0B);
    writeData(0x3A);
    writeData(0x42);
    writeData(0x3B);
    writeData(0x0B);
    writeData(0x14);
    writeData(0x14);
    writeData(0x27);
    writeData(0x2D);

    // GIP 2
    writeCommand(ILI9881C_SETGIP2);
    writeData(0x00);
    writeData(0x09);
    writeData(0x0F);
    writeData(0x0E);
    writeData(0x07);
    writeData(0x10);
    writeData(0x0B);
    writeData(0x3A);
    writeData(0x42);
    writeData(0x3B);
    writeData(0x0B);
    writeData(0x14);
    writeData(0x14);
    writeData(0x27);
    writeData(0x2D);

    // GIP 3
    writeCommand(ILI9881C_SETGIP3);
    for(uint8_t i = 0; i < 15; i++) {
        writeData(0x00 + i);
    }

    // GIP 4
    writeCommand(ILI9881C_SETGIP4);
    for(uint8_t i = 0; i < 15; i++) {
        writeData(0x00 + i);
    }
}

void TFT_Driver_ILI9881C::initGamma() {
    // Gamma Setting
    writeCommand(0xE0);    // Positive Gamma
    writeData(0x00);
    writeData(0x07);
    writeData(0x0C);
    writeData(0x0F);
    writeData(0x10);
    writeData(0x1B);
    writeData(0x1D);
    writeData(0x22);
    writeData(0x25);
    writeData(0x28);
    writeData(0x36);
    writeData(0x3F);
    writeData(0x44);
    writeData(0x4B);
    writeData(0x52);
    writeData(0x5F);

    writeCommand(0xE1);    // Negative Gamma
    writeData(0x00);
    writeData(0x19);
    writeData(0x1E);
    writeData(0x24);
    writeData(0x28);
    writeData(0x2E);
    writeData(0x30);
    writeData(0x34);
    writeData(0x38);
    writeData(0x3C);
    writeData(0x44);
    writeData(0x49);
    writeData(0x4E);
    writeData(0x53);
    writeData(0x58);
    writeData(0x5F);
}

void TFT_Driver_ILI9881C::writeCommand(uint8_t cmd) {
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

void TFT_Driver_ILI9881C::writeData(uint8_t data) {
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

void TFT_Driver_ILI9881C::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_ILI9881C::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(ILI9881C_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x00 : 0x08);
            _width = ILI9881C_TFTWIDTH;
            _height = ILI9881C_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x60 : 0x68);
            _width = ILI9881C_TFTHEIGHT;
            _height = ILI9881C_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0xC0 : 0xC8);
            _width = ILI9881C_TFTWIDTH;
            _height = ILI9881C_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xA0 : 0xA8);
            _width = ILI9881C_TFTHEIGHT;
            _height = ILI9881C_TFTWIDTH;
            break;
    }
}

void TFT_Driver_ILI9881C::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? ILI9881C_INVON : ILI9881C_INVOFF);
}

void TFT_Driver_ILI9881C::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(ILI9881C_CASET);
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);

    writeCommand(ILI9881C_PASET);
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);

    writeCommand(ILI9881C_RAMWR);
}

void TFT_Driver_ILI9881C::setImageFunction(uint8_t mode) {
    writeCommand(ILI9881C_SETIMAGE);
    writeData(mode);
}

void TFT_Driver_ILI9881C::setPanelCharacteristics(uint8_t panel) {
    writeCommand(ILI9881C_SETPANEL);
    writeData(panel);
}

void TFT_Driver_ILI9881C::setPowerControl(uint8_t power) {
    writeCommand(ILI9881C_SETPOWER);
    writeData(power);
}

void TFT_Driver_ILI9881C::setRGBInterface(uint8_t rgb_if) {
    writeCommand(ILI9881C_SETRGB);
    writeData(rgb_if);
}

void TFT_Driver_ILI9881C::setCycle(uint8_t cycle) {
    writeCommand(ILI9881C_SETCYC);
    writeData(cycle);
}

void TFT_Driver_ILI9881C::setVCOM(uint8_t vcom) {
    writeCommand(ILI9881C_SETVCOM);
    writeData(vcom);
}

void TFT_Driver_ILI9881C::setExtendedCommands(bool enable) {
    writeCommand(ILI9881C_SETEXTC);
    if (enable) {
        writeData(0xFF);
        writeData(0x98);
        writeData(0x81);
        writeData(0x03);
    }
}

void TFT_Driver_ILI9881C::setMIPIControl(uint8_t mipi) {
    writeCommand(ILI9881C_SETMIPI);
    writeData(mipi);
}

void TFT_Driver_ILI9881C::setOTPControl(uint8_t otp) {
    writeCommand(ILI9881C_SETOTP);
    writeData(otp);
}

void TFT_Driver_ILI9881C::setPowerControl2(uint8_t power) {
    writeCommand(ILI9881C_SETPOWER2);
    writeData(power);
}

void TFT_Driver_ILI9881C::setPowerControl3(uint8_t power) {
    writeCommand(ILI9881C_SETPOWER3);
    writeData(power);
}

void TFT_Driver_ILI9881C::setDGC(uint8_t dgc) {
    writeCommand(ILI9881C_SETDGC);
    writeData(dgc);
}

void TFT_Driver_ILI9881C::setVCOM2(uint8_t vcom) {
    writeCommand(ILI9881C_SETVCOM2);
    writeData(vcom);
}

void TFT_Driver_ILI9881C::setGIP1(uint8_t gip) {
    writeCommand(ILI9881C_SETGIP1);
    writeData(gip);
}

void TFT_Driver_ILI9881C::setGIP2(uint8_t gip) {
    writeCommand(ILI9881C_SETGIP2);
    writeData(gip);
}

void TFT_Driver_ILI9881C::setGIP3(uint8_t gip) {
    writeCommand(ILI9881C_SETGIP3);
    writeData(gip);
}

void TFT_Driver_ILI9881C::setGIP4(uint8_t gip) {
    writeCommand(ILI9881C_SETGIP4);
    writeData(gip);
}

void TFT_Driver_ILI9881C::setGIP5(uint8_t gip) {
    writeCommand(ILI9881C_SETGIP5);
    writeData(gip);
}

void TFT_Driver_ILI9881C::setGIP6(uint8_t gip) {
    writeCommand(ILI9881C_SETGIP6);
    writeData(gip);
}

void TFT_Driver_ILI9881C::setGIP7(uint8_t gip) {
    writeCommand(ILI9881C_SETGIP7);
    writeData(gip);
}

void TFT_Driver_ILI9881C::setGamma(uint8_t gamma) {
    writeCommand(ILI9881C_GAMSET);
    writeData(gamma);
}

void TFT_Driver_ILI9881C::setMemoryAccessControl(uint8_t mode) {
    writeCommand(ILI9881C_MADCTL);
    writeData(mode);
}

void TFT_Driver_ILI9881C::setPixelFormat(uint8_t format) {
    writeCommand(ILI9881C_COLMOD);
    writeData(format);
}

void TFT_Driver_ILI9881C::setColumnAddress(uint16_t start, uint16_t end) {
    writeCommand(ILI9881C_CASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_ILI9881C::setPageAddress(uint16_t start, uint16_t end) {
    writeCommand(ILI9881C_PASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_ILI9881C::setPartialArea(uint16_t start, uint16_t end) {
    writeCommand(ILI9881C_PTLAR);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_ILI9881C::setTearingEffect(bool enable, bool mode) {
    writeCommand(enable ? ILI9881C_TEON : ILI9881C_TEOFF);
    if (enable) {
        writeData(mode ? 0x01 : 0x00);
    }
}

void TFT_Driver_ILI9881C::setIdleMode(bool enable) {
    writeCommand(enable ? ILI9881C_IDMON : ILI9881C_IDMOFF);
}

} // namespace TFT_Runtime
