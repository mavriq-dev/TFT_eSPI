#include <SPI.h>
#include "TFT_Driver_ST7735R.h"

namespace TFT_Runtime {

TFT_Driver_ST7735R::TFT_Driver_ST7735R(Config& config) : TFT_Driver_Base(config) {
    _width = ST7735R_TFTWIDTH;
    _height = ST7735R_TFTHEIGHT;
    _tabcolor = ST7735R_INITR_GREENTAB;
}

void TFT_Driver_ST7735R::init() {
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

    commonInit();
    initR(_tabcolor);
}

void TFT_Driver_ST7735R::commonInit() {
    writeCommand(ST7735R_SWRESET);    // Software reset
    delay(150);

    writeCommand(ST7735R_SLPOUT);     // Out of sleep mode
    delay(120);

    writeCommand(ST7735R_FRMCTR1);    // Frame rate ctrl - normal mode
    writeData(0x01);                  // Rate = fosc/(1x2+40) * (LINE+2C+2D)
    writeData(0x2C);
    writeData(0x2D);

    writeCommand(ST7735R_FRMCTR2);    // Frame rate ctrl - idle mode
    writeData(0x01);                  // Rate = fosc/(1x2+40) * (LINE+2C+2D)
    writeData(0x2C);
    writeData(0x2D);

    writeCommand(ST7735R_FRMCTR3);    // Frame rate ctrl - partial mode
    writeData(0x01);                  // Dot inversion mode
    writeData(0x2C);
    writeData(0x2D);
    writeData(0x01);                  // Line inversion mode
    writeData(0x2C);
    writeData(0x2D);

    writeCommand(ST7735R_INVCTR);     // Display inversion ctrl
    writeData(0x07);                  // No inversion

    writeCommand(ST7735R_PWCTR1);     // Power control
    writeData(0xA2);
    writeData(0x02);                  // -4.6V
    writeData(0x84);                  // AUTO mode

    writeCommand(ST7735R_PWCTR2);     // Power control
    writeData(0xC5);                  // VGH25=2.4C VGSEL=-10 VGH=3 * AVDD

    writeCommand(ST7735R_PWCTR3);     // Power control
    writeData(0x0A);                  // Opamp current small
    writeData(0x00);                  // Boost frequency

    writeCommand(ST7735R_PWCTR4);     // Power control
    writeData(0x8A);                  // BCLK/2,
    writeData(0x2A);                  // opamp current small & medium low

    writeCommand(ST7735R_PWCTR5);     // Power control
    writeData(0x8A);
    writeData(0xEE);

    writeCommand(ST7735R_VMCTR1);     // Power control
    writeData(0x0E);

    writeCommand(ST7735R_COLMOD);     // Interface pixel format
    writeData(0x05);                  // 16-bit color
}

void TFT_Driver_ST7735R::initR(uint8_t options) {
    switch (options) {
        case ST7735R_INITR_GREENTAB:
            initRGreenTab();
            break;
        case ST7735R_INITR_REDTAB:
            initRRedTab();
            break;
        case ST7735R_INITR_BLACKTAB:
            initRBlackTab();
            break;
        case ST7735R_INITR_144GREENTAB:
            initR144();
            break;
    }
}

void TFT_Driver_ST7735R::initRGreenTab() {
    writeCommand(ST7735R_CASET);      // Column addr set
    writeData(0x00);                  // XS
    writeData(0x02);                  // XS
    writeData(0x00);
    writeData(0x7F + 0x02);           // XE

    writeCommand(ST7735R_RASET);      // Row addr set
    writeData(0x00);                  // YS
    writeData(0x01);                  // YS
    writeData(0x00);
    writeData(0x9F + 0x01);           // YE

    writeCommand(ST7735R_GMCTRP1);    // Gamma Adjustments (pos. polarity)
    writeData(0x02);
    writeData(0x1c);
    writeData(0x07);
    writeData(0x12);
    writeData(0x37);
    writeData(0x32);
    writeData(0x29);
    writeData(0x2d);
    writeData(0x29);
    writeData(0x25);
    writeData(0x2B);
    writeData(0x39);
    writeData(0x00);
    writeData(0x01);
    writeData(0x03);
    writeData(0x10);

    writeCommand(ST7735R_GMCTRN1);    // Gamma Adjustments (neg. polarity)
    writeData(0x03);
    writeData(0x1d);
    writeData(0x07);
    writeData(0x06);
    writeData(0x2E);
    writeData(0x2C);
    writeData(0x29);
    writeData(0x2D);
    writeData(0x2E);
    writeData(0x2E);
    writeData(0x37);
    writeData(0x3F);
    writeData(0x00);
    writeData(0x00);
    writeData(0x02);
    writeData(0x10);

    writeCommand(ST7735R_NORON);      // Normal display on
    delay(10);
    writeCommand(ST7735R_DISPON);     // Display on
}

void TFT_Driver_ST7735R::initRRedTab() {
    // Red tab specific initialization
    writeCommand(ST7735R_CASET);      // Column addr set
    writeData(0x00);
    writeData(0x00);                  // XS = 0
    writeData(0x00);
    writeData(0x7F);                  // XE = 127

    writeCommand(ST7735R_RASET);      // Row addr set
    writeData(0x00);
    writeData(0x00);                  // YS = 0
    writeData(0x00);
    writeData(0x9F);                  // YE = 159

    // Rest is same as green tab
    initRGreenTab();
}

void TFT_Driver_ST7735R::initRBlackTab() {
    // Black tab specific initialization
    writeCommand(ST7735R_CASET);      // Column addr set
    writeData(0x00);
    writeData(0x00);                  // XS = 0
    writeData(0x00);
    writeData(0x7F);                  // XE = 127

    writeCommand(ST7735R_RASET);      // Row addr set
    writeData(0x00);
    writeData(0x00);                  // YS = 0
    writeData(0x00);
    writeData(0x9F);                  // YE = 159

    // Rest is same as green tab
    initRGreenTab();
}

void TFT_Driver_ST7735R::initR144() {
    // 1.44" display specific initialization
    _height = ST7735R_TFTHEIGHT + 16;
    
    writeCommand(ST7735R_CASET);      // Column addr set
    writeData(0x00);
    writeData(0x00);                  // XS = 0
    writeData(0x00);
    writeData(0x7F);                  // XE = 127

    writeCommand(ST7735R_RASET);      // Row addr set
    writeData(0x00);
    writeData(0x00);                  // YS = 0
    writeData(0x00);
    writeData(0x7F);                  // YE = 127

    // Rest is same as green tab
    initRGreenTab();
}

void TFT_Driver_ST7735R::setTabColor(uint8_t option) {
    _tabcolor = option;
    init();  // Reinitialize with new tab color
}

void TFT_Driver_ST7735R::writeCommand(uint8_t cmd) {
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

void TFT_Driver_ST7735R::writeData(uint8_t data) {
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

void TFT_Driver_ST7735R::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_ST7735R::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(ST7735R_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x48 : 0x40);
            _width = ST7735R_TFTWIDTH;
            _height = ST7735R_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x28 : 0x20);
            _width = ST7735R_TFTHEIGHT;
            _height = ST7735R_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0x88 : 0x80);
            _width = ST7735R_TFTWIDTH;
            _height = ST7735R_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xE8 : 0xE0);
            _width = ST7735R_TFTHEIGHT;
            _height = ST7735R_TFTWIDTH;
            break;
    }
}

void TFT_Driver_ST7735R::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? ST7735R_INVON : ST7735R_INVOFF);
}

void TFT_Driver_ST7735R::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(ST7735R_CASET);      // Column addr set
    writeData(0x00);
    writeData(x0 + (_tabcolor == ST7735R_INITR_GREENTAB ? 2 : 0));     // XS
    writeData(0x00);
    writeData(x1 + (_tabcolor == ST7735R_INITR_GREENTAB ? 2 : 0));     // XE

    writeCommand(ST7735R_RASET);      // Row addr set
    writeData(0x00);
    writeData(y0 + (_tabcolor == ST7735R_INITR_GREENTAB ? 1 : 0));     // YS
    writeData(0x00);
    writeData(y1 + (_tabcolor == ST7735R_INITR_GREENTAB ? 1 : 0));     // YE

    writeCommand(ST7735R_RAMWR);      // Write to RAM
}

} // namespace TFT_Runtime
