#include <SPI.h>
#include "TFT_Driver_ILI9163.h"

namespace TFT_Runtime {

TFT_Driver_ILI9163::TFT_Driver_ILI9163(Config& config) : TFT_Driver_Base(config) {
    _width = ILI9163_TFTWIDTH;
    _height = ILI9163_TFTHEIGHT;
    _colstart = 0;
    _rowstart = 0;
}

void TFT_Driver_ILI9163::init() {
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

    writeCommand(ILI9163_SWRESET);   // Software reset
    delay(120);                      // Wait for reset to complete

    writeCommand(ILI9163_SLPOUT);    // Exit Sleep
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

    // Set pixel format
    writeCommand(ILI9163_COLMOD);
    writeData(0x05);                 // 16-bit color

    // Memory Access Control
    writeCommand(ILI9163_MADCTL);
    writeData(0x00);                 // Row/Column addressing

    // Display on
    writeCommand(ILI9163_DISPON);
    delay(120);
}

void TFT_Driver_ILI9163::initPowerSettings() {
    // Power Control 1
    writeCommand(ILI9163_PWCTR1);
    writeData(0x02);                 // VRH[5:0]
    writeData(0x70);                 // BT[2:0]

    // Power Control 2
    writeCommand(ILI9163_PWCTR2);
    writeData(0x05);                 // DC[2:0], AP[2:0]

    // Power Control 3
    writeCommand(ILI9163_PWCTR3);
    writeData(0x01);                 // VRH
    writeData(0x02);                 // VCI1

    // VCOM Control
    writeCommand(ILI9163_VMCTR1);
    writeData(0x3C);                 // VMH[6:0]
    writeData(0x38);                 // VML[6:0]
}

void TFT_Driver_ILI9163::initDisplaySettings() {
    // Frame Rate Control
    writeCommand(ILI9163_FRMCTR1);
    writeData(0x00);                 // Division ratio
    writeData(0x06);                 // Frame rate

    // Display Function Control
    writeCommand(ILI9163_DISSET5);
    writeData(0x0A);                 // Non-overlap period
    writeData(0x82);                 // Number of lines
    writeData(0x27);                 // Clock frequency
    writeData(0x00);                 // Gate start position
}

void TFT_Driver_ILI9163::initGamma() {
    // Positive Gamma Control
    writeCommand(ILI9163_GMCTRP1);
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
    writeCommand(ILI9163_GMCTRN1);
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

void TFT_Driver_ILI9163::writeCommand(uint8_t cmd) {
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

void TFT_Driver_ILI9163::writeData(uint8_t data) {
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

void TFT_Driver_ILI9163::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_ILI9163::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(ILI9163_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x00 : 0x08);
            _width = ILI9163_TFTWIDTH;
            _height = ILI9163_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x60 : 0x68);
            _width = ILI9163_TFTHEIGHT;
            _height = ILI9163_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0xC0 : 0xC8);
            _width = ILI9163_TFTWIDTH;
            _height = ILI9163_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xA0 : 0xA8);
            _width = ILI9163_TFTHEIGHT;
            _height = ILI9163_TFTWIDTH;
            break;
    }
}

void TFT_Driver_ILI9163::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? ILI9163_INVON : ILI9163_INVOFF);
}

void TFT_Driver_ILI9163::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    x0 += _colstart;
    x1 += _colstart;
    y0 += _rowstart;
    y1 += _rowstart;
    
    writeCommand(ILI9163_CASET);
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);

    writeCommand(ILI9163_PASET);
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);

    writeCommand(ILI9163_RAMWR);
}

void TFT_Driver_ILI9163::setFrameRate(uint8_t divider, uint8_t rtna) {
    writeCommand(ILI9163_FRMCTR1);
    writeData(divider);
    writeData(rtna);
}

void TFT_Driver_ILI9163::setDisplayTiming(uint8_t frs, uint8_t btt) {
    writeCommand(ILI9163_DISSET5);
    writeData(frs);
    writeData(btt);
}

void TFT_Driver_ILI9163::setPowerControl(uint8_t vc, uint8_t bt, uint8_t dc) {
    writeCommand(ILI9163_PWCTR1);
    writeData(vc);
    writeData(bt);
    writeData(dc);
}

void TFT_Driver_ILI9163::setVCOMVoltage(uint8_t vcm) {
    writeCommand(ILI9163_VMCTR1);
    writeData(vcm);
}

void TFT_Driver_ILI9163::setGamma(uint8_t gamma) {
    writeCommand(ILI9163_GAMSET);
    writeData(gamma);
}

void TFT_Driver_ILI9163::setIdleMode(bool enable) {
    writeCommand(enable ? ILI9163_IDMON : ILI9163_IDMOFF);
}

void TFT_Driver_ILI9163::setInterface(uint8_t mode) {
    writeCommand(ILI9163_DISSET5);
    writeData(mode);
}

void TFT_Driver_ILI9163::setDisplayMode(uint8_t mode) {
    writeCommand(ILI9163_DISSET5);
    writeData(mode);
}

void TFT_Driver_ILI9163::setTearingEffect(bool enable, bool mode) {
    writeCommand(enable ? ILI9163_TEON : ILI9163_TEOFF);
    if (enable) {
        writeData(mode ? 0x01 : 0x00);
    }
}

void TFT_Driver_ILI9163::setPartialArea(uint16_t start, uint16_t end) {
    writeCommand(ILI9163_PTLAR);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_ILI9163::setScrollArea(uint16_t topFixed, uint16_t scrollArea, uint16_t bottomFixed) {
    writeCommand(ILI9163_VSCRDEF);
    writeData(topFixed >> 8);
    writeData(topFixed & 0xFF);
    writeData(scrollArea >> 8);
    writeData(scrollArea & 0xFF);
    writeData(bottomFixed >> 8);
    writeData(bottomFixed & 0xFF);
}

void TFT_Driver_ILI9163::setScrollStart(uint16_t start) {
    writeCommand(ILI9163_VSCRSADD);
    writeData(start >> 8);
    writeData(start & 0xFF);
}

void TFT_Driver_ILI9163::setPixelFormat(uint8_t format) {
    writeCommand(ILI9163_COLMOD);
    writeData(format);
}

void TFT_Driver_ILI9163::setMemoryAccessControl(uint8_t mode) {
    writeCommand(ILI9163_MADCTL);
    writeData(mode);
}

void TFT_Driver_ILI9163::setColumnAddress(uint16_t start, uint16_t end) {
    start += _colstart;
    end += _colstart;
    writeCommand(ILI9163_CASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

void TFT_Driver_ILI9163::setPageAddress(uint16_t start, uint16_t end) {
    start += _rowstart;
    end += _rowstart;
    writeCommand(ILI9163_PASET);
    writeData(start >> 8);
    writeData(start & 0xFF);
    writeData(end >> 8);
    writeData(end & 0xFF);
}

} // namespace TFT_Runtime
