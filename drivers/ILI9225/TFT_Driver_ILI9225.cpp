#include <SPI.h>
#include "TFT_Driver_ILI9225.h"

namespace TFT_Runtime {

TFT_Driver_ILI9225::TFT_Driver_ILI9225(Config& config) : TFT_Driver_Base(config) {
    _width = ILI9225_TFTWIDTH;
    _height = ILI9225_TFTHEIGHT;
}

void TFT_Driver_ILI9225::init() {
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

    // Power-on sequence
    writeRegister(ILI9225_POWER_CTRL1, 0x0000); // Set SAP,DSTB,STB
    writeRegister(ILI9225_POWER_CTRL2, 0x0000); // Set APON,PON,AON,VCI1EN,VC
    writeRegister(ILI9225_POWER_CTRL3, 0x0000); // Set BT,DC1,DC2,DC3
    writeRegister(ILI9225_POWER_CTRL4, 0x0000); // Set GVDD
    writeRegister(ILI9225_POWER_CTRL5, 0x0000); // Set VCOMH/VCOML voltage
    delay(40);

    // Power supply settings
    writeRegister(ILI9225_POWER_CTRL2, 0x0018); // Set APON,PON,AON,VCI1EN,VC
    writeRegister(ILI9225_POWER_CTRL3, 0x6121); // Set BT,DC1,DC2,DC3
    writeRegister(ILI9225_POWER_CTRL4, 0x006F); // Set GVDD
    writeRegister(ILI9225_POWER_CTRL5, 0x495F); // Set VCOMH/VCOML voltage
    writeRegister(ILI9225_POWER_CTRL1, 0x0800); // Set SAP,DSTB,STB
    delay(10);

    // Display settings
    writeRegister(ILI9225_DRIVER_OUTPUT_CTRL, 0x011C); // Set DMODE,TRI,REV,BGR
    writeRegister(ILI9225_LCD_AC_DRIVING_CTRL, 0x0100); // Set line inversion
    writeRegister(ILI9225_ENTRY_MODE, 0x1030); // Set GRAM write direction and BGR=1
    writeRegister(ILI9225_DISP_CTRL1, 0x0000); // Display off
    writeRegister(ILI9225_BLANK_PERIOD_CTRL1, 0x0808); // Set front/back porch
    writeRegister(ILI9225_FRAME_CYCLE_CTRL, 0x1100); // Set clocks per line
    writeRegister(ILI9225_INTERFACE_CTRL, 0x0000); // CPU interface
    writeRegister(ILI9225_OSC_CTRL, 0x0D01); // Set internal oscillator
    writeRegister(ILI9225_VCI_RECYCLING, 0x0020); // Set VCI recycling
    
    // Set GRAM area
    writeRegister(ILI9225_GATE_SCAN_CTRL, 0x0000); // Scan start position
    writeRegister(ILI9225_VERTICAL_SCROLL_CTRL1, 0x00DB); // Scroll area setup
    writeRegister(ILI9225_VERTICAL_SCROLL_CTRL2, 0x0000);
    writeRegister(ILI9225_VERTICAL_SCROLL_CTRL3, 0x0000);
    writeRegister(ILI9225_PARTIAL_DRIVING_POS1, 0x00DB);
    writeRegister(ILI9225_PARTIAL_DRIVING_POS2, 0x0000);

    // Initialize gamma
    initGamma();
    delay(50);

    // Display on
    writeRegister(ILI9225_DISP_CTRL1, 0x1017); // Display on
}

void TFT_Driver_ILI9225::initGamma() {
    writeRegister(ILI9225_GAMMA_CTRL1, 0x0000);
    writeRegister(ILI9225_GAMMA_CTRL2, 0x0808);
    writeRegister(ILI9225_GAMMA_CTRL3, 0x0808);
    writeRegister(ILI9225_GAMMA_CTRL4, 0x0000);
    writeRegister(ILI9225_GAMMA_CTRL5, 0x0404);
    writeRegister(ILI9225_GAMMA_CTRL6, 0x0404);
    writeRegister(ILI9225_GAMMA_CTRL7, 0x0000);
    writeRegister(ILI9225_GAMMA_CTRL8, 0x0404);
    writeRegister(ILI9225_GAMMA_CTRL9, 0x0404);
    writeRegister(ILI9225_GAMMA_CTRL10, 0x0000);
}

void TFT_Driver_ILI9225::writeCommand(uint8_t cmd) {
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

void TFT_Driver_ILI9225::writeData(uint8_t data) {
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

void TFT_Driver_ILI9225::writeRegister(uint8_t reg, uint16_t data) {
    writeCommand(reg);
    writeData(data >> 8);
    writeData(data & 0xFF);
}

void TFT_Driver_ILI9225::writeBlock(uint16_t* data, uint32_t len) {
    if (config.interface == InterfaceMode::SPI) {
        digitalWrite(config.spi.dc_pin, HIGH);
        if (config.spi.cs_pin >= 0) digitalWrite(config.spi.cs_pin, LOW);
        
        // Use hardware SPI for better performance
        for (uint32_t i = 0; i < len; i++) {
            uint16_t color = data[i];
            SPI.transfer(color >> 8);
            SPI.transfer(color & 0xFF);
        }
        
        if (config.spi.cs_pin >= 0) digitalWrite(config.spi.cs_pin, HIGH);
    } else {
        // Parallel 8-bit implementation
        digitalWrite(config.parallel.dc_pin, HIGH);
        if (config.parallel.cs_pin >= 0) digitalWrite(config.parallel.cs_pin, LOW);
        
        // Write to parallel bus - implementation depends on hardware setup
        for (uint32_t i = 0; i < len; i++) {
            uint16_t color = data[i];
            // Write high byte then low byte
            // Implementation depends on hardware setup
        }
        
        if (config.parallel.cs_pin >= 0) digitalWrite(config.parallel.cs_pin, HIGH);
    }
}

void TFT_Driver_ILI9225::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    uint16_t entryMode = 0;
    
    switch (_rotation) {
        case 0:
            entryMode = 0x1030;
            _width = ILI9225_TFTWIDTH;
            _height = ILI9225_TFTHEIGHT;
            break;
        case 1:
            entryMode = 0x1028;
            _width = ILI9225_TFTHEIGHT;
            _height = ILI9225_TFTWIDTH;
            break;
        case 2:
            entryMode = 0x1000;
            _width = ILI9225_TFTWIDTH;
            _height = ILI9225_TFTHEIGHT;
            break;
        case 3:
            entryMode = 0x1018;
            _width = ILI9225_TFTHEIGHT;
            _height = ILI9225_TFTWIDTH;
            break;
    }
    
    if (config.color_order == ColorOrder::BGR) {
        entryMode |= 0x1000;
    }
    
    writeRegister(ILI9225_ENTRY_MODE, entryMode);
}

void TFT_Driver_ILI9225::invertDisplay(bool invert) {
    _invert = invert;
    uint16_t value = invert ? 0x1017 : 0x1017;
    writeRegister(ILI9225_DISP_CTRL1, value);
}

void TFT_Driver_ILI9225::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeRegister(ILI9225_HORIZONTAL_WINDOW_ADDR1, x0);
    writeRegister(ILI9225_HORIZONTAL_WINDOW_ADDR2, x1);
    writeRegister(ILI9225_VERTICAL_WINDOW_ADDR1, y0);
    writeRegister(ILI9225_VERTICAL_WINDOW_ADDR2, y1);
    
    writeRegister(ILI9225_RAM_ADDR_SET1, x0);
    writeRegister(ILI9225_RAM_ADDR_SET2, y0);
    
    writeCommand(ILI9225_GRAM_DATA_REG);
}

void TFT_Driver_ILI9225::setDisplayMode(uint8_t mode) {
    writeRegister(ILI9225_DISP_CTRL1, mode);
}

void TFT_Driver_ILI9225::setOscillator(uint8_t osc) {
    writeRegister(ILI9225_OSC_CTRL, osc);
}

void TFT_Driver_ILI9225::setPowerControl(uint8_t ctrl1, uint8_t ctrl2, uint8_t ctrl3) {
    writeRegister(ILI9225_POWER_CTRL1, ctrl1);
    writeRegister(ILI9225_POWER_CTRL2, ctrl2);
    writeRegister(ILI9225_POWER_CTRL3, ctrl3);
}

void TFT_Driver_ILI9225::setDrivingControl(uint8_t mode) {
    writeRegister(ILI9225_DRIVER_OUTPUT_CTRL, mode);
}

void TFT_Driver_ILI9225::setGamma(uint8_t gamma1, uint8_t gamma2, uint8_t gamma3) {
    writeRegister(ILI9225_GAMMA_CTRL1, gamma1);
    writeRegister(ILI9225_GAMMA_CTRL2, gamma2);
    writeRegister(ILI9225_GAMMA_CTRL3, gamma3);
}

void TFT_Driver_ILI9225::setPartialArea(uint16_t start, uint16_t end) {
    writeRegister(ILI9225_PARTIAL_DRIVING_POS1, start);
    writeRegister(ILI9225_PARTIAL_DRIVING_POS2, end);
}

void TFT_Driver_ILI9225::setScrollArea(uint16_t top, uint16_t scroll, uint16_t bottom) {
    writeRegister(ILI9225_VERTICAL_SCROLL_CTRL1, top);
    writeRegister(ILI9225_VERTICAL_SCROLL_CTRL2, scroll);
    writeRegister(ILI9225_VERTICAL_SCROLL_CTRL3, bottom);
}

} // namespace TFT_Runtime
