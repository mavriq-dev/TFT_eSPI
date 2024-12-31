#include <SPI.h>
#include "TFT_Driver_HX8357C.h"

namespace TFT_Runtime {

TFT_Driver_HX8357C::TFT_Driver_HX8357C(Config& config) : TFT_Driver_Base(config) {
    _width = HX8357C_TFTWIDTH;
    _height = HX8357C_TFTHEIGHT;
}

void TFT_Driver_HX8357C::init() {
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

    writeCommand(HX8357C_SWRESET);    // Software reset
    delay(120);                       // Wait for reset to complete

    // Enable extended commands
    writeCommand(HX8357C_SETEXTC);
    writeData(0xFF);
    writeData(0x83);
    writeData(0x57);

    // Set oscillator
    writeCommand(HX8357C_SETOSC);
    writeData(0x68);                  // Normal mode 70Hz, Idle mode 55Hz

    // Set power control
    writeCommand(HX8357C_SETPOWER);
    writeData(0x07);                  // VGH = 15V, VGL = -10V
    writeData(0x42);                  // Vdd = 5V
    writeData(0x1D);                  // DDVDH = 5.6V

    // Set VCOM voltage
    writeCommand(HX8357C_SETVCOM);
    writeData(0x00);
    writeData(0x1C);                  // VCOMH = 4.0V
    writeData(0x1C);                  // VCOML = -1.0V
    writeData(0x45);                  // VCOMDC = 0V

    // Set internal oscillator
    writeCommand(HX8357C_SETPWR1);
    writeData(0x00);                  // Not deep standby
    writeData(0x15);                  // BT
    writeData(0x0D);                  // AP
    writeData(0x0D);                  // APE
    writeData(0x83);                  // OSC_EN=1, PON=0
    writeData(0x48);                  // VC1=0, VC3=0, VRH=7

    // Set RGB interface
    writeCommand(HX8357C_SETRGB);
    writeData(0x00);                  // RGB interface setting
    writeData(0x00);                  // RGB mode=0x0 (DBI), RIM=0 (1-transfer), ENC=0 (DE/SYNC mode)
    writeData(0x06);                  // RPM=0 (Vsync), EPM=0 (DE)
    writeData(0x06);                  // DPL=0, HSPL=0, VSPL=0, EPL=0

    // Set display cycle
    writeCommand(HX8357C_SETCYC);
    writeData(0x50);                  // N_DC=1, I_DC=1, EQ_DC=1
    writeData(0x02);                  // DIV=2 (low power)
    writeData(0x40);                  // RTN=4 (interval)
    writeData(0x00);                  // DIV=0 (normal)
    writeData(0x2A);                  // DUM=2, GDON=10
    writeData(0x2A);                  // GDOFF=10
    writeData(0x0C);                  // NW=12
    writeData(0x67);                  // GTD=6, REV=7

    // Set gamma
    initGamma();

    // Set panel characteristics
    writeCommand(HX8357C_SETPANEL);
    writeData(0x00);                  // SS_PANEL = 0, GS_PANEL = 0

    // Set display
    writeCommand(HX8357C_SETDGC);
    writeData(0x20);                  // DGC_EN = 0, GAMMA_EN = 1

    // Set GRAM area
    writeCommand(HX8357C_MADCTL);
    writeData(0x00);

    writeCommand(HX8357C_COLMOD);
    writeData(0x55);                  // 16-bit color

    // Exit sleep
    writeCommand(HX8357C_SLPOUT);
    delay(120);

    // Display on
    writeCommand(HX8357C_DISPON);
    delay(10);
}

void TFT_Driver_HX8357C::initGamma() {
    writeCommand(HX8357C_SETGAMMA);
    // Gamma values optimized for HX8357C
    writeData(0x00);  // VRP0[5:0]
    writeData(0x15);  // VRP1[5:0]
    writeData(0x1C);  // VRP2[5:0]
    writeData(0x1C);  // VRP3[5:0]
    writeData(0x1C);  // VRP4[5:0]
    writeData(0x1C);  // VRP5[5:0]
    writeData(0x1C);  // VRP6[5:0]
    writeData(0x1C);  // VRP7[5:0]
    writeData(0x1C);  // VRP8[5:0]
    writeData(0x1C);  // VRP9[5:0]
    writeData(0x1C);  // VRP10[5:0]
    writeData(0x1C);  // VRP11[5:0]
    writeData(0x1C);  // VRP12[5:0]
    writeData(0x1C);  // VRP13[5:0]
    writeData(0x1C);  // VRP14[5:0]
    writeData(0x1C);  // VRP15[5:0]
    writeData(0x1C);  // VRP16[5:0]
    writeData(0x1C);  // VRP17[5:0]
    writeData(0x1C);  // VRP18[5:0]
    writeData(0x1C);  // VRP19[5:0]
    writeData(0x1C);  // VRP20[5:0]
    writeData(0x1C);  // VRP21[5:0]
    writeData(0x1C);  // VRP22[5:0]
    writeData(0x1C);  // VRP23[5:0]
}

void TFT_Driver_HX8357C::writeCommand(uint8_t cmd) {
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

void TFT_Driver_HX8357C::writeData(uint8_t data) {
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

void TFT_Driver_HX8357C::writeBlock(uint16_t* data, uint32_t len) {
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

void TFT_Driver_HX8357C::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    writeCommand(HX8357C_MADCTL);
    
    switch (_rotation) {
        case 0:
            writeData(config.color_order == ColorOrder::RGB ? 0x00 : 0x08);
            _width = HX8357C_TFTWIDTH;
            _height = HX8357C_TFTHEIGHT;
            break;
        case 1:
            writeData(config.color_order == ColorOrder::RGB ? 0x60 : 0x68);
            _width = HX8357C_TFTHEIGHT;
            _height = HX8357C_TFTWIDTH;
            break;
        case 2:
            writeData(config.color_order == ColorOrder::RGB ? 0xC0 : 0xC8);
            _width = HX8357C_TFTWIDTH;
            _height = HX8357C_TFTHEIGHT;
            break;
        case 3:
            writeData(config.color_order == ColorOrder::RGB ? 0xA0 : 0xA8);
            _width = HX8357C_TFTHEIGHT;
            _height = HX8357C_TFTWIDTH;
            break;
    }
}

void TFT_Driver_HX8357C::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? HX8357C_INVON : HX8357C_INVOFF);
}

void TFT_Driver_HX8357C::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(HX8357C_CASET);
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);

    writeCommand(HX8357C_PASET);
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);

    writeCommand(HX8357C_RAMWR);
}

void TFT_Driver_HX8357C::setOscillator(uint8_t osc) {
    writeCommand(HX8357C_SETOSC);
    writeData(osc);
}

void TFT_Driver_HX8357C::setPowerControl(uint8_t vc, uint8_t bt, uint8_t dc) {
    writeCommand(HX8357C_SETPOWER);
    writeData(vc);
    writeData(bt);
    writeData(dc);
}

void TFT_Driver_HX8357C::setRGBInterface(uint8_t rgb_if) {
    writeCommand(HX8357C_SETRGB);
    writeData(rgb_if);
}

void TFT_Driver_HX8357C::setCycle(uint8_t cycle) {
    writeCommand(HX8357C_SETCYC);
    writeData(cycle);
}

void TFT_Driver_HX8357C::setDisplayControl(uint8_t mode) {
    writeCommand(HX8357C_SETCOM);
    writeData(mode);
}

void TFT_Driver_HX8357C::setGamma(uint8_t gamma) {
    writeCommand(HX8357C_GAMSET);
    writeData(gamma);
}

void TFT_Driver_HX8357C::setPanelCharacteristics(uint8_t panel) {
    writeCommand(HX8357C_SETPANEL);
    writeData(panel);
}

void TFT_Driver_HX8357C::setPower(uint8_t power) {
    writeCommand(HX8357C_SETPOWER);
    writeData(power);
}

void TFT_Driver_HX8357C::setVCOM(uint8_t vcom) {
    writeCommand(HX8357C_SETVCOM);
    writeData(vcom);
}

void TFT_Driver_HX8357C::setNormalMode(uint8_t normal) {
    writeCommand(HX8357C_SETNOR);
    writeData(normal);
}

void TFT_Driver_HX8357C::setPartialMode(uint8_t partial) {
    writeCommand(HX8357C_SETPAR);
    writeData(partial);
}

void TFT_Driver_HX8357C::setPanelRelated(uint8_t related) {
    writeCommand(HX8357C_SETPANELREL);
    writeData(related);
}

void TFT_Driver_HX8357C::setStandbyMode(uint8_t standby) {
    writeCommand(HX8357C_SETSTBA);
    writeData(standby);
}

void TFT_Driver_HX8357C::setDigitalGammaControl(bool enable) {
    writeCommand(HX8357C_SETDGC);
    writeData(enable ? 0x20 : 0x00);
}

void TFT_Driver_HX8357C::setExtendedCommands(bool enable) {
    writeCommand(HX8357C_SETEXTC);
    if (enable) {
        writeData(0xFF);
        writeData(0x83);
        writeData(0x57);
    }
}

void TFT_Driver_HX8357C::setTearingEffect(bool enable) {
    writeCommand(HX8357C_TEON);
    writeData(enable ? 0x00 : 0x01);
}

void TFT_Driver_HX8357C::setMemoryAccessControl(uint8_t mode) {
    writeCommand(HX8357C_MADCTL);
    writeData(mode);
}

void TFT_Driver_HX8357C::setPixelFormat(uint8_t format) {
    writeCommand(HX8357C_COLMOD);
    writeData(format);
}

} // namespace TFT_Runtime
