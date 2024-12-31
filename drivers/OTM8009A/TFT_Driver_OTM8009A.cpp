#include <SPI.h>
#include "TFT_Driver_OTM8009A.h"

namespace TFT_Runtime {

TFT_Driver_OTM8009A::TFT_Driver_OTM8009A(Config& config) : TFT_Driver_Base(config) {
    _width = OTM8009A_TFTWIDTH;
    _height = OTM8009A_TFTHEIGHT;
}

void TFT_Driver_OTM8009A::init() {
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

    writeCommand(OTM8009A_SWRESET);    // Software reset
    delay(120);                        // Wait for reset to complete

    writeCommand(OTM8009A_SLPOUT);     // Exit Sleep
    delay(120);

    // Enable extended commands
    enableExtendedCommands(true);
    delay(10);

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

    // Set MIPI lanes
    setMIPILanes(4);                   // 4 lanes MIPI
    delay(10);

    // Set pixel format
    writeCommand(OTM8009A_COLMOD);
    writeData(0x55);                   // 16-bit color

    // Set display mode
    writeCommand(OTM8009A_WRCTRLD);
    writeData(0x2C);                   // Enable display control, BL

    // Set CABC
    writeCommand(OTM8009A_WRCABC);
    writeData(0x01);                   // Enable CABC

    writeCommand(OTM8009A_NORON);      // Normal Display Mode ON
    delay(10);

    writeCommand(OTM8009A_DISPON);     // Display ON
    delay(120);
}

void TFT_Driver_OTM8009A::initPowerSettings() {
    // Power Control
    writeCommand(OTM8009A_SETPOWER);
    writeData(0x07);                   // VCI1 = 2.5V
    writeData(0x42);                   // DDVDH = 5.94V
    writeData(0x1D);                   // VREG1 = 4.625V

    // VCOM Control
    writeCommand(OTM8009A_SETVCOM);
    writeData(0x00);                   // VCOMH = 4.250V
    writeData(0x1F);                   // VCOML = -1.500V
    writeData(0x1F);                   // VCOM offset
}

void TFT_Driver_OTM8009A::initDisplaySettings() {
    // Memory Access Control
    writeCommand(OTM8009A_MADCTL);
    writeData(0x48);                   // MX=0, MY=1, MV=0, ML=0, RGB=0

    // Panel Characteristics
    writeCommand(OTM8009A_SETPANEL);
    writeData(0x0B);                   // Panel characteristics set

    // Display Timing Setting
    writeCommand(OTM8009A_SETGIP1);
    writeData(0x50);                   // Gate timing control
    writeData(0x50);
    writeData(0x00);
    writeData(0x00);
}

void TFT_Driver_OTM8009A::initGIP() {
    // GIP 1
    writeCommand(OTM8009A_SETGIP1);
    writeData(0x40);                   // Gate timing control
    writeData(0x02);
    writeData(0x40);
    writeData(0x02);
    writeData(0x00);

    // GIP 2
    writeCommand(OTM8009A_SETGIP2);
    writeData(0xC1);                   // Gate timing control
    writeData(0x80);
    writeData(0x00);
    writeData(0x08);
}

void TFT_Driver_OTM8009A::initGamma() {
    writeCommand(OTM8009A_SETGAMMA);
    // Gamma settings (Positive)
    writeData(0x00);
    writeData(0x09);
    writeData(0x0F);
    writeData(0x0E);
    writeData(0x07);
    writeData(0x10);
    writeData(0x0B);
    writeData(0x0A);
    writeData(0x04);
    writeData(0x07);
    writeData(0x0B);
    writeData(0x08);
    writeData(0x0F);
    writeData(0x10);
    writeData(0x0A);
    writeData(0x01);
    
    // Gamma settings (Negative)
    writeData(0x00);
    writeData(0x09);
    writeData(0x0F);
    writeData(0x0E);
    writeData(0x07);
    writeData(0x10);
    writeData(0x0B);
    writeData(0x0A);
    writeData(0x04);
    writeData(0x07);
    writeData(0x0B);
    writeData(0x08);
    writeData(0x0F);
    writeData(0x10);
    writeData(0x0A);
    writeData(0x01);
}

void TFT_Driver_OTM8009A::writeCommand(uint8_t cmd) {
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

void TFT_Driver_OTM8009A::writeData(uint8_t data) {
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

void TFT_Driver_OTM8009A::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_OTM8009A::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(OTM8009A_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x48 : 0x40);
            _width = OTM8009A_TFTWIDTH;
            _height = OTM8009A_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x28 : 0x20);
            _width = OTM8009A_TFTHEIGHT;
            _height = OTM8009A_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0x88 : 0x80);
            _width = OTM8009A_TFTWIDTH;
            _height = OTM8009A_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xE8 : 0xE0);
            _width = OTM8009A_TFTHEIGHT;
            _height = OTM8009A_TFTWIDTH;
            break;
    }
}

void TFT_Driver_OTM8009A::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? OTM8009A_INVON : OTM8009A_INVOFF);
}

void TFT_Driver_OTM8009A::setBrightness(uint8_t brightness) {
    writeCommand(OTM8009A_WRDISBV);
    writeData(brightness);
}

void TFT_Driver_OTM8009A::setCABC(uint8_t mode) {
    writeCommand(OTM8009A_WRCABC);
    writeData(mode & 0x03);  // 0: Off, 1: UI Mode, 2: Still Mode, 3: Moving Mode
}

void TFT_Driver_OTM8009A::setVCOM(uint8_t value) {
    writeCommand(OTM8009A_SETVCOM);
    writeData(value);
}

void TFT_Driver_OTM8009A::setMIPILanes(uint8_t lanes) {
    writeCommand(OTM8009A_SETMIPI);
    writeData(lanes & 0x03);  // 0: 1 lane, 1: 2 lanes, 2: 3 lanes, 3: 4 lanes
}

void TFT_Driver_OTM8009A::setPanelCharacteristics(uint8_t mode) {
    writeCommand(OTM8009A_SETPANEL);
    writeData(mode);
}

void TFT_Driver_OTM8009A::setGIPTiming(uint8_t mode) {
    writeCommand(OTM8009A_SETGIP1);
    writeData(mode);
}

void TFT_Driver_OTM8009A::enableExtendedCommands(bool enable) {
    writeCommand(OTM8009A_SETEXTC);
    if (enable) {
        writeData(0xFF);
        writeData(0x80);
        writeData(0x09);
        writeData(0x01);
    } else {
        writeData(0xFF);
        writeData(0xFF);
        writeData(0xFF);
        writeData(0xFF);
    }
}

void TFT_Driver_OTM8009A::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(OTM8009A_CASET);      // Column addr set
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);             // Start col
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);             // End col

    writeCommand(OTM8009A_PASET);      // Row addr set
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);             // Start row
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);             // End row

    writeCommand(OTM8009A_RAMWR);      // Write to RAM
}

} // namespace TFT_Runtime
