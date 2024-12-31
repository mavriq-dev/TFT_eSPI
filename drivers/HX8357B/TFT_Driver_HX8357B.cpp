#include <SPI.h>
#include "TFT_Driver_HX8357B.h"

namespace TFT_Runtime {

TFT_Driver_HX8357B::TFT_Driver_HX8357B(Config& config) : TFT_Driver_Base(config) {
    _width = HX8357B_TFTWIDTH;
    _height = HX8357B_TFTHEIGHT;
}

void TFT_Driver_HX8357B::init() {
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

    writeCommand(HX8357B_SWRESET);    // Software reset
    delay(120);                       // Wait for reset to complete

    // Enable extended commands
    enableExtendedCommands(true);
    delay(10);

    writeCommand(HX8357B_SLPOUT);     // Exit Sleep
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

    // Set oscillator
    writeCommand(HX8357B_SETOSC);
    writeData(0x68);                  // Normal mode 65Hz

    // Set power control
    writeCommand(HX8357B_SETPWR1);
    writeData(0x00);                  // Not deep standby
    writeData(0x15);                  // BT
    writeData(0x1C);                  // VSPR
    writeData(0x1C);                  // VSNR
    writeData(0x83);                  // AP
    writeData(0xAA);                  // FS

    // Set VCOM
    writeCommand(HX8357B_SETVCOM);
    writeData(0x2B);                  // VCOMH = 3.925V
    writeData(0x2B);                  // VCOML = -1.375V

    // Set pixel format
    writeCommand(HX8357B_COLMOD);
    writeData(0x55);                  // 16-bit color

    writeCommand(HX8357B_MADCTL);     // Memory Access Control
    writeData(0x48);                  // MX=0, MY=1, MV=0, ML=0, RGB=0

    writeCommand(HX8357B_TEON);       // Tearing effect line on
    writeData(0x00);                  // V-blanking only

    writeCommand(HX8357B_NORON);      // Normal Display Mode ON
    delay(10);

    writeCommand(HX8357B_DISPON);     // Display ON
    delay(120);
}

void TFT_Driver_HX8357B::initPowerSettings() {
    // Set power control
    writeCommand(HX8357B_SETPOWER);
    writeData(0x44);                  // Internal power
    writeData(0x41);                  // Temperature range
    writeData(0x06);                  // DDVDH = 5.94V

    // Set VCOM
    writeCommand(HX8357B_SETVCOM);
    writeData(0x40);                  // VCOMH = 4.250V
    writeData(0x0B);                  // VCOML = -1.500V

    // Set panel driving
    writeCommand(HX8357B_SETPANEL);
    writeData(0x09);                  // REV=1, BGR=0
}

void TFT_Driver_HX8357B::initDisplaySettings() {
    // Set RGB interface 
    writeCommand(HX8357B_SETRGB);
    writeData(0x00);                  // RGB interface
    writeData(0x00);                  // RGB mode
    writeData(0x06);                  // RGB format

    // Set display cycle
    writeCommand(HX8357B_SETCYC);
    writeData(0x04);                  // 2 dot inversion
    writeData(0x14);                  // NW=1, RTN=4
    writeData(0x03);                  // DIV=3
    writeData(0x3B);                  // DUM=3, GDON=11
    writeData(0x4B);                  // GDOFF=11
}

void TFT_Driver_HX8357B::initGamma() {
    writeCommand(HX8357B_SETGAMMA);
    // Gamma settings
    writeData(0x01);  // Gamma curve 1
    writeData(0x02);
    writeData(0x03);
    writeData(0x05);
    writeData(0x0E);
    writeData(0x22);
    writeData(0x32);
    writeData(0x3B);
    writeData(0x5C);
    writeData(0x54);
    writeData(0x4C);
    writeData(0x41);
    writeData(0x3D);
    writeData(0x37);
    writeData(0x31);
    writeData(0x21);
    writeData(0x01);
    writeData(0x02);
    writeData(0x03);
    writeData(0x05);
    writeData(0x0E);
    writeData(0x22);
    writeData(0x32);
    writeData(0x3B);
    writeData(0x5C);
    writeData(0x54);
    writeData(0x4C);
    writeData(0x41);
    writeData(0x3D);
    writeData(0x37);
    writeData(0x31);
    writeData(0x21);
    writeData(0x00);
    writeData(0x01);
}

void TFT_Driver_HX8357B::writeCommand(uint8_t cmd) {
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

void TFT_Driver_HX8357B::writeData(uint8_t data) {
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

void TFT_Driver_HX8357B::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_HX8357B::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(HX8357B_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x48 : 0x40);
            _width = HX8357B_TFTWIDTH;
            _height = HX8357B_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x28 : 0x20);
            _width = HX8357B_TFTHEIGHT;
            _height = HX8357B_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0x88 : 0x80);
            _width = HX8357B_TFTWIDTH;
            _height = HX8357B_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xE8 : 0xE0);
            _width = HX8357B_TFTHEIGHT;
            _height = HX8357B_TFTWIDTH;
            break;
    }
}

void TFT_Driver_HX8357B::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? HX8357B_INVON : HX8357B_INVOFF);
}

void TFT_Driver_HX8357B::setOscillator(uint8_t osc) {
    writeCommand(HX8357B_SETOSC);
    writeData(osc);
}

void TFT_Driver_HX8357B::setPowerControl(uint8_t mode) {
    writeCommand(HX8357B_SETPOWER);
    writeData(mode);
}

void TFT_Driver_HX8357B::setRGBInterface(uint8_t mode) {
    writeCommand(HX8357B_SETRGB);
    writeData(mode);
}

void TFT_Driver_HX8357B::setCycle(uint8_t mode) {
    writeCommand(HX8357B_SETCYC);
    writeData(mode);
}

void TFT_Driver_HX8357B::setDisplayMode(uint8_t mode) {
    writeCommand(HX8357B_SETPANEL);
    writeData(mode);
}

void TFT_Driver_HX8357B::setStandbyMode(bool enable) {
    writeCommand(HX8357B_SETSTBA);
    writeData(enable ? 0x01 : 0x00);
}

void TFT_Driver_HX8357B::setPanelCharacteristics(uint8_t mode) {
    writeCommand(HX8357B_SETPANEL);
    writeData(mode);
}

void TFT_Driver_HX8357B::setVCOM(uint8_t value) {
    writeCommand(HX8357B_SETVCOM);
    writeData(value);
}

void TFT_Driver_HX8357B::enableExtendedCommands(bool enable) {
    writeCommand(HX8357B_SETEXTC);
    if (enable) {
        writeData(0xFF);
        writeData(0x83);
        writeData(0x57);
    } else {
        writeData(0xFF);
        writeData(0xFF);
        writeData(0xFF);
    }
}

void TFT_Driver_HX8357B::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(HX8357B_CASET);      // Column addr set
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);             // Start col
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);             // End col

    writeCommand(HX8357B_PASET);      // Row addr set
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);             // Start row
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);             // End row

    writeCommand(HX8357B_RAMWR);      // Write to RAM
}

} // namespace TFT_Runtime
