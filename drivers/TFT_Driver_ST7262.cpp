#include <SPI.h>
#include "TFT_Driver_ST7262.h"

namespace TFT_Runtime {

TFT_Driver_ST7262::TFT_Driver_ST7262(Config& config) : TFT_Driver_Base(config) {
    _width = ST7262_TFTWIDTH;
    _height = ST7262_TFTHEIGHT;
}

void TFT_Driver_ST7262::init() {
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

    writeCommand(ST7262_SWRESET);    // Software reset
    delay(120);                      // Wait for reset to complete

    writeCommand(ST7262_SLPOUT);     // Exit Sleep
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
    writeCommand(ST7262_COLMOD);
    writeData(0x55);                 // 16-bit color

    // Memory Access Control
    writeCommand(ST7262_MADCTL);
    writeData(0x00);                 // Normal orientation

    // Configure charge pump
    writeCommand(ST7262_SET_PUMP);
    writeData(0x20);                 // Enable charge pump during display on
    
    // Power Control 1
    writeCommand(ST7262_SETPWCTR1);
    writeData(0x23);                 // GVDD = 4.6V
    writeData(0x00);                 // AVDD = VCI * 2

    // Power Control 2
    writeCommand(ST7262_SETPWCTR2);
    writeData(0x10);                 // VGH = VCI * 6, VGL = -VCI * 3

    // VCOM Control
    writeCommand(ST7262_SETVCOM1);
    writeData(0x36);                 // VCOMH = 4.25V
    writeData(0x3E);                 // VCOML = -0.75V

    // Display on
    writeCommand(ST7262_DISPON);
    delay(120);
}

void TFT_Driver_ST7262::initPowerSettings() {
    // Power Control 1
    writeCommand(ST7262_SETPWCTR1);
    writeData(0x23);                 // GVDD = 4.6V
    writeData(0x00);                 // AVDD = VCI * 2

    // Power Control 2
    writeCommand(ST7262_SETPWCTR2);
    writeData(0x10);                 // VGH = VCI * 6, VGL = -VCI * 3

    // Power Control 3
    writeCommand(ST7262_SETPWCTR3);
    writeData(0x3E);                 // Source driving current level
    writeData(0x28);                 // Source driving current level for RGB

    // Power Control 4
    writeCommand(ST7262_SETPWCTR4);
    writeData(0x55);                 // VCOM amplitude
    writeData(0x0F);                 // VCOM amplitude

    // Power Control 5
    writeCommand(ST7262_SETPWCTR5);
    writeData(0x36);                 // VCOMH = 4.25V
    writeData(0x24);                 // VCOML = -0.75V
}

void TFT_Driver_ST7262::initDisplaySettings() {
    // Frame Rate Control
    writeCommand(0xB1);
    writeData(0x00);                 // Frame rate = 79Hz
    writeData(0x18);

    // Display Function Control
    writeCommand(0xB6);
    writeData(0x0A);
    writeData(0xA2);

    // Entry Mode Set
    writeCommand(0xB7);
    writeData(0x06);

    // Interface Control
    writeCommand(0xF6);
    writeData(0x01);
    writeData(0x30);
    writeData(0x00);
}

void TFT_Driver_ST7262::initGamma() {
    // Positive Gamma Control
    writeCommand(ST7262_SETPGAMMA);
    writeData(0x0F);
    writeData(0x31);
    writeData(0x2B);
    writeData(0x0C);
    writeData(0x0E);
    writeData(0x08);
    writeData(0x4E);
    writeData(0xF1);
    writeData(0x37);
    writeData(0x07);
    writeData(0x10);
    writeData(0x03);
    writeData(0x0E);
    writeData(0x09);
    writeData(0x00);

    // Negative Gamma Control
    writeCommand(ST7262_SETNGAMMA);
    writeData(0x00);
    writeData(0x0E);
    writeData(0x14);
    writeData(0x03);
    writeData(0x11);
    writeData(0x07);
    writeData(0x31);
    writeData(0xC1);
    writeData(0x48);
    writeData(0x08);
    writeData(0x0F);
    writeData(0x0C);
    writeData(0x31);
    writeData(0x36);
    writeData(0x0F);
}

void TFT_Driver_ST7262::writeCommand(uint8_t cmd) {
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

void TFT_Driver_ST7262::writeData(uint8_t data) {
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

void TFT_Driver_ST7262::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_ST7262::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(ST7262_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x00 : 0x08);
            _width = ST7262_TFTWIDTH;
            _height = ST7262_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x60 : 0x68);
            _width = ST7262_TFTHEIGHT;
            _height = ST7262_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0xC0 : 0xC8);
            _width = ST7262_TFTWIDTH;
            _height = ST7262_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xA0 : 0xA8);
            _width = ST7262_TFTHEIGHT;
            _height = ST7262_TFTWIDTH;
            break;
    }
}

void TFT_Driver_ST7262::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? ST7262_INVON : ST7262_INVOFF);
}

void TFT_Driver_ST7262::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(ST7262_CASET);
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);

    writeCommand(ST7262_PASET);
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);

    writeCommand(ST7262_RAMWR);
}

void TFT_Driver_ST7262::setPumpControl(uint8_t control) {
    writeCommand(ST7262_SET_PUMP);
    writeData(control);
}

void TFT_Driver_ST7262::setPowerControl1(uint8_t vrh) {
    writeCommand(ST7262_SETPWCTR1);
    writeData(vrh);
}

void TFT_Driver_ST7262::setPowerControl2(uint8_t bt) {
    writeCommand(ST7262_SETPWCTR2);
    writeData(bt);
}

void TFT_Driver_ST7262::setPowerControl3(uint8_t mode) {
    writeCommand(ST7262_SETPWCTR3);
    writeData(mode);
}

void TFT_Driver_ST7262::setPowerControl4(uint8_t mode) {
    writeCommand(ST7262_SETPWCTR4);
    writeData(mode);
}

void TFT_Driver_ST7262::setPowerControl5(uint8_t mode) {
    writeCommand(ST7262_SETPWCTR5);
    writeData(mode);
}

void TFT_Driver_ST7262::setVCOM1(uint8_t vcm) {
    writeCommand(ST7262_SETVCOM1);
    writeData(vcm);
}

void TFT_Driver_ST7262::setVCOM2(uint8_t vcm) {
    writeCommand(ST7262_SETVCOM2);
    writeData(vcm);
}

void TFT_Driver_ST7262::setMemoryAccessControl(uint8_t mode) {
    writeCommand(ST7262_MADCTL);
    writeData(mode);
}

void TFT_Driver_ST7262::setPixelFormat(uint8_t format) {
    writeCommand(ST7262_COLMOD);
    writeData(format);
}

void TFT_Driver_ST7262::setGamma(uint8_t gamma) {
    writeCommand(ST7262_GAMSET);
    writeData(gamma);
}

void TFT_Driver_ST7262::setPositiveGamma(const uint8_t* gamma) {
    writeCommand(ST7262_SETPGAMMA);
    for(uint8_t i = 0; i < 15; i++) {
        writeData(gamma[i]);
    }
}

void TFT_Driver_ST7262::setNegativeGamma(const uint8_t* gamma) {
    writeCommand(ST7262_SETNGAMMA);
    for(uint8_t i = 0; i < 15; i++) {
        writeData(gamma[i]);
    }
}

void TFT_Driver_ST7262::setOTP(uint8_t otp) {
    writeCommand(ST7262_SETOTP);
    writeData(otp);
}

void TFT_Driver_ST7262::setColumnAddress(uint16_t start, uint16_t end) {
    writeCommand(ST7262_CASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_ST7262::setPageAddress(uint16_t start, uint16_t end) {
    writeCommand(ST7262_PASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_ST7262::setPartialArea(uint16_t start, uint16_t end) {
    writeCommand(ST7262_PTLAR);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_ST7262::setTearingEffect(bool enable, bool mode) {
    writeCommand(enable ? ST7262_TEON : ST7262_TEOFF);
    if (enable) {
        writeData(mode ? 0x01 : 0x00);
    }
}

void TFT_Driver_ST7262::setIdleMode(bool enable) {
    writeCommand(enable ? ST7262_IDMON : ST7262_IDMOFF);
}

uint32_t TFT_Driver_ST7262::readID() {
    uint32_t id = 0;
    
    writeCommand(ST7262_RDID1);
    id = readData() << 16;
    
    writeCommand(ST7262_RDID2);
    id |= readData() << 8;
    
    writeCommand(ST7262_RDID3);
    id |= readData();
    
    return id;
}

} // namespace TFT_Runtime
