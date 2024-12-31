#include <SPI.h>
#include "TFT_Driver_ILI9342.h"

namespace TFT_Runtime {

TFT_Driver_ILI9342::TFT_Driver_ILI9342(Config& config) : TFT_Driver_Base(config) {
    _width = ILI9342_TFTWIDTH;
    _height = ILI9342_TFTHEIGHT;
}

void TFT_Driver_ILI9342::init() {
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

    writeCommand(ILI9342_SWRESET);    // Software reset
    delay(120);                       // Wait for reset to complete

    writeCommand(ILI9342_SLPOUT);     // Exit Sleep
    delay(120);

    // Power Control
    initPowerSequence();
    delay(10);

    // VCOM Control
    writeCommand(ILI9342_VMCTR1);     // VCOM Control 1
    writeData(0x3E);                  // VCOMH = 4.25V
    writeData(0x28);                  // VCOML = -1.5V

    // Memory Access Control
    writeCommand(ILI9342_MADCTL);
    writeData(0x48);                  // MX, BGR

    // Pixel Format Set
    writeCommand(ILI9342_PIXFMT);
    writeData(0x55);                  // 16-bit color

    // Frame Rate Control
    writeCommand(ILI9342_FRMCTR1);    // Frame rate 70Hz
    writeData(0x00);
    writeData(0x18);

    // Display Function Control
    writeCommand(ILI9342_DFUNCTR);
    writeData(0x08);                  // All scan lines are displayed
    writeData(0x82);                  // Reserved
    writeData(0x27);                  // 320 lines

    // Interface Control
    writeCommand(ILI9342_IFCTL);
    writeData(0x00);                  // System interface
    writeData(0x00);                  // RGB interface
    writeData(0x00);                  // VSPL: Low active

    // Gamma settings
    initGamma();
    delay(10);

    // Display settings
    initDisplay();
    delay(120);

    writeCommand(ILI9342_DISPON);     // Display ON
    delay(120);
}

void TFT_Driver_ILI9342::initPowerSequence() {
    writeCommand(ILI9342_PWCTR1);     // Power Control 1
    writeData(0x23);                  // VRH[5:0]
    writeData(0x00);                  // SAP[2:0]

    writeCommand(ILI9342_PWCTR2);     // Power Control 2
    writeData(0x10);                  // BT[2:0]

    writeCommand(ILI9342_PWCTR3);     // Power Control 3
    writeData(0x02);                  // AP[2:0]
    writeData(0x02);                  // DC[2:0]

    writeCommand(ILI9342_PWCTR4);     // Power Control 4
    writeData(0x02);                  // DC[2:0]

    writeCommand(ILI9342_PWCTR5);     // Power Control 5
    writeData(0x02);                  // DC[2:0]
    writeData(0x00);                  // CDC[2:0]
}

void TFT_Driver_ILI9342::initGamma() {
    // Positive Gamma Control
    writeCommand(ILI9342_GMCTRP1);
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
    writeCommand(ILI9342_GMCTRN1);
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

void TFT_Driver_ILI9342::initDisplay() {
    writeCommand(ILI9342_CASET);      // Column addr set
    writeData(0x00);
    writeData(0x00);                  // Start 0
    writeData(0x01);
    writeData(0x3F);                  // End 319

    writeCommand(ILI9342_PASET);      // Row addr set
    writeData(0x00);
    writeData(0x00);                  // Start 0
    writeData(0x00);
    writeData(0xEF);                  // End 239

    writeCommand(ILI9342_NORON);      // Normal display mode on
    delay(10);
}

void TFT_Driver_ILI9342::writeCommand(uint8_t cmd) {
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

void TFT_Driver_ILI9342::writeData(uint8_t data) {
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

void TFT_Driver_ILI9342::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_ILI9342::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(ILI9342_MADCTL);
    
    switch (_rotation) {
        case 0: // Portrait
            writeData(config.color_order == ColorOrder::RGB ? 0x48 : 0x40);
            _width = ILI9342_TFTWIDTH;
            _height = ILI9342_TFTHEIGHT;
            break;
        case 1: // Landscape
            writeData(config.color_order == ColorOrder::RGB ? 0x28 : 0x20);
            _width = ILI9342_TFTHEIGHT;
            _height = ILI9342_TFTWIDTH;
            break;
        case 2: // Portrait inverted
            writeData(config.color_order == ColorOrder::RGB ? 0x88 : 0x80);
            _width = ILI9342_TFTWIDTH;
            _height = ILI9342_TFTHEIGHT;
            break;
        case 3: // Landscape inverted
            writeData(config.color_order == ColorOrder::RGB ? 0xE8 : 0xE0);
            _width = ILI9342_TFTHEIGHT;
            _height = ILI9342_TFTWIDTH;
            break;
    }
}

void TFT_Driver_ILI9342::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? ILI9342_INVON : ILI9342_INVOFF);
}

void TFT_Driver_ILI9342::setInterfaceControl(bool mdt, bool epf) {
    writeCommand(ILI9342_IFCTL);
    writeData(mdt ? 0x01 : 0x00);     // MDT
    writeData(epf ? 0x01 : 0x00);     // EPF
    writeData(0x00);                  // RCM
}

void TFT_Driver_ILI9342::setDisplayFunction(uint8_t div, bool vrh, bool vgl) {
    writeCommand(ILI9342_DFUNCTR);
    writeData((div & 0x07) | (vrh ? 0x08 : 0x00));
    writeData(0x82);                  // Reserved
    writeData(vgl ? 0x27 : 0x07);     // VGL
}

void TFT_Driver_ILI9342::setGamma(uint8_t gamma) {
    writeCommand(ILI9342_GAMMASET);
    writeData(gamma);
}

void TFT_Driver_ILI9342::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(ILI9342_CASET);      // Column addr set
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);             // Start col
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);             // End col

    writeCommand(ILI9342_PASET);      // Row addr set
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);             // Start row
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);             // End row

    writeCommand(ILI9342_RAMWR);      // Write to RAM
}

} // namespace TFT_Runtime
