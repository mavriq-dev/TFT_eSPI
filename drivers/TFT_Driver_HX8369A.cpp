#include <SPI.h>
#include "TFT_Driver_HX8369A.h"

namespace TFT_Runtime {

TFT_Driver_HX8369A::TFT_Driver_HX8369A(Config& config) : TFT_Driver_Base(config) {
    _width = HX8369A_TFTWIDTH;
    _height = HX8369A_TFTHEIGHT;
}

void TFT_Driver_HX8369A::init() {
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

    writeCommand(HX8369A_SWRESET);    // Software reset
    delay(120);                       // Wait for reset to complete

    writeCommand(HX8369A_SLPOUT);     // Exit Sleep
    delay(120);

    // Set password for extended commands
    writeCommand(HX8369A_SETEXTC);
    writeData(0xFF);
    writeData(0x83);
    writeData(0x69);

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
    writeCommand(HX8369A_COLMOD);
    writeData(0x55);

    // Memory Access Control
    writeCommand(HX8369A_MADCTL);
    writeData(0x00);

    // Set MIPI control for high speed
    writeCommand(HX8369A_SETMIPI);
    writeData(0x03);

    // VCOM voltage setting
    writeCommand(HX8369A_SETVCOM);
    writeData(0x2C);
    writeData(0x2C);

    // CABC (Content Adaptive Backlight Control)
    writeCommand(HX8369A_SETCABC);
    writeData(0x10);
    writeData(0x40);
    writeData(0x00);
    writeData(0x00);

    // Display on
    writeCommand(HX8369A_DISPON);
    delay(120);
}

void TFT_Driver_HX8369A::initPowerSettings() {
    // Power Control 1
    writeCommand(HX8369A_SETPWD);
    writeData(0x01);    // AVDD: 6.6V
    writeData(0x00);    // AVDD: 5.2V
    writeData(0x34);    // VGH: 15V
    writeData(0x06);    // VGL: -10V
    writeData(0x00);    // DDVDH: 5.94V
    writeData(0x11);    // Set pump clock frequency
    writeData(0x11);    // Set pump clock frequency
    writeData(0x30);    // Set pump clock frequency

    // Power Control for Normal Mode
    writeCommand(HX8369A_SETPOWER);
    writeData(0x13);    // AP[2:0]
    writeData(0x42);    // DC[2:0]
    writeData(0x1B);    // DC_DIV[1:0]
}

void TFT_Driver_HX8369A::initDisplaySettings() {
    // Display Timing Setting for Normal Mode
    writeCommand(HX8369A_SETDISPLAY);
    writeData(0x00);    // N_NW[7:0]
    writeData(0x58);    // I_NW[7:0]
    writeData(0x56);    // N_RTN[4:0]
    writeData(0x78);    // I_RTN[4:0]
    writeData(0x00);    // DIV[1:0]
    writeData(0x14);    // DIV[1:0]

    // RGB Interface Setting
    writeCommand(HX8369A_SETRGB);
    writeData(0x80);    // RGB interface control
    writeData(0x00);    // RGB interface timing
    writeData(0x00);    // RGB interface polarity

    // Frame Rate Control
    writeCommand(HX8369A_SETCYC);
    writeData(0x02);    // 60Hz frame rate
    writeData(0x40);    // Line inversion
}

void TFT_Driver_HX8369A::initGamma() {
    // Gamma Setting
    writeCommand(0xE0);    // Positive Gamma
    writeData(0x00);
    writeData(0x04);
    writeData(0x09);
    writeData(0x0F);
    writeData(0x1F);
    writeData(0x3F);
    writeData(0x47);
    writeData(0x4F);
    writeData(0x57);
    writeData(0x5F);
    writeData(0x67);
    writeData(0x6F);
    writeData(0x77);
    writeData(0x7F);
    writeData(0x87);
    writeData(0x8F);

    writeCommand(0xE1);    // Negative Gamma
    writeData(0x00);
    writeData(0x04);
    writeData(0x09);
    writeData(0x0F);
    writeData(0x1F);
    writeData(0x3F);
    writeData(0x47);
    writeData(0x4F);
    writeData(0x57);
    writeData(0x5F);
    writeData(0x67);
    writeData(0x6F);
    writeData(0x77);
    writeData(0x7F);
    writeData(0x87);
    writeData(0x8F);
}

void TFT_Driver_HX8369A::writeCommand(uint8_t cmd) {
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

void TFT_Driver_HX8369A::writeData(uint8_t data) {
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

void TFT_Driver_HX8369A::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_HX8369A::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(HX8369A_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x00 : 0x08);
            _width = HX8369A_TFTWIDTH;
            _height = HX8369A_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x60 : 0x68);
            _width = HX8369A_TFTHEIGHT;
            _height = HX8369A_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0xC0 : 0xC8);
            _width = HX8369A_TFTWIDTH;
            _height = HX8369A_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xA0 : 0xA8);
            _width = HX8369A_TFTHEIGHT;
            _height = HX8369A_TFTWIDTH;
            break;
    }
}

void TFT_Driver_HX8369A::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? HX8369A_INVON : HX8369A_INVOFF);
}

void TFT_Driver_HX8369A::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(HX8369A_CASET);
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);

    writeCommand(HX8369A_PASET);
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);

    writeCommand(HX8369A_RAMWR);
}

void TFT_Driver_HX8369A::setPassword(uint8_t pwd) {
    writeCommand(HX8369A_SETPWD);
    writeData(pwd);
}

void TFT_Driver_HX8369A::setDisplayMode(uint8_t mode) {
    writeCommand(HX8369A_SETDISPLAY);
    writeData(mode);
}

void TFT_Driver_HX8369A::setRGBInterface(uint8_t rgb_if) {
    writeCommand(HX8369A_SETRGB);
    writeData(rgb_if);
}

void TFT_Driver_HX8369A::setCycle(uint8_t cycle) {
    writeCommand(HX8369A_SETCYC);
    writeData(cycle);
}

void TFT_Driver_HX8369A::setVCOM(uint8_t vcom) {
    writeCommand(HX8369A_SETVCOM);
    writeData(vcom);
}

void TFT_Driver_HX8369A::setExtendedCommands(bool enable) {
    writeCommand(HX8369A_SETEXTC);
    if (enable) {
        writeData(0xFF);
        writeData(0x83);
        writeData(0x69);
    }
}

void TFT_Driver_HX8369A::setMIPIControl(uint8_t mipi) {
    writeCommand(HX8369A_SETMIPI);
    writeData(mipi);
}

void TFT_Driver_HX8369A::setOTPControl(uint8_t otp) {
    writeCommand(HX8369A_SETOTP);
    writeData(otp);
}

void TFT_Driver_HX8369A::setPowerControl(uint8_t power) {
    writeCommand(HX8369A_SETPOWER);
    writeData(power);
}

void TFT_Driver_HX8369A::setCABC(uint8_t cabc) {
    writeCommand(HX8369A_SETCABC);
    writeData(cabc);
}

void TFT_Driver_HX8369A::setGamma(uint8_t gamma) {
    writeCommand(HX8369A_GAMSET);
    writeData(gamma);
}

void TFT_Driver_HX8369A::setMemoryAccessControl(uint8_t mode) {
    writeCommand(HX8369A_MADCTL);
    writeData(mode);
}

void TFT_Driver_HX8369A::setPixelFormat(uint8_t format) {
    writeCommand(HX8369A_COLMOD);
    writeData(format);
}

void TFT_Driver_HX8369A::setColumnAddress(uint16_t start, uint16_t end) {
    writeCommand(HX8369A_CASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_HX8369A::setPageAddress(uint16_t start, uint16_t end) {
    writeCommand(HX8369A_PASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_HX8369A::setScrollArea(uint16_t tfa, uint16_t vsa, uint16_t bfa) {
    writeCommand(HX8369A_VSCRDEF);
    writeData(tfa >> 8);
    writeData(tfa & 0xFF);
    writeData(vsa >> 8);
    writeData(vsa & 0xFF);
    writeData(bfa >> 8);
    writeData(bfa & 0xFF);
}

void TFT_Driver_HX8369A::setScrollStart(uint16_t start) {
    writeCommand(HX8369A_VSCRSADD);
    writeData(start >> 8);
    writeData(start & 0xFF);
}

void TFT_Driver_HX8369A::setTearingEffect(bool enable, bool mode) {
    writeCommand(enable ? HX8369A_TEON : HX8369A_TEOFF);
    if (enable) {
        writeData(mode ? 0x01 : 0x00);
    }
}

void TFT_Driver_HX8369A::setPartialArea(uint16_t start, uint16_t end) {
    writeCommand(HX8369A_PTLAR);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_HX8369A::setIdleMode(bool enable) {
    writeCommand(enable ? HX8369A_IDMON : HX8369A_IDMOFF);
}

} // namespace TFT_Runtime
