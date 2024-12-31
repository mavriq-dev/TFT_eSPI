#include <SPI.h>
#include "TFT_Driver_SSD1351.h"

namespace TFT_Runtime {

TFT_Driver_SSD1351::TFT_Driver_SSD1351(Config& config) : TFT_Driver_Base(config) {
    _width = SSD1351_TFTWIDTH;
    _height = SSD1351_TFTHEIGHT;
}

void TFT_Driver_SSD1351::init() {
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

    // Unlock commands
    setCommandLock(false);
    
    // Display off
    writeCommand(SSD1351_CMD_DISPLAYOFF);

    // Set clock divider
    writeCommand(SSD1351_CMD_CLOCKDIV);
    writeData(0xF1);  // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio

    // Set MUX ratio
    writeCommand(SSD1351_CMD_MUXRATIO);
    writeData(0x7F);  // 127

    // Set display offset
    writeCommand(SSD1351_CMD_DISPLAYOFFSET);
    writeData(0x00);

    // Set start line
    writeCommand(SSD1351_CMD_STARTLINE);
    writeData(0x00);

    // Set remap format
    writeCommand(SSD1351_CMD_SETREMAP);
    writeData(0x74);  // 0x74 = Color RGB, Enable COM Split Odd/Even

    // Set VSL
    writeCommand(SSD1351_CMD_SETVSL);
    writeData(0xA0);  // External VSL
    writeData(0xB5);
    writeData(0x55);

    // Set contrast
    writeCommand(SSD1351_CMD_CONTRASTABC);
    writeData(0xC8);  // R
    writeData(0x80);  // G
    writeData(0xC8);  // B

    // Set master contrast
    writeCommand(SSD1351_CMD_CONTRASTMASTER);
    writeData(0x0F);

    // Set precharge
    writeCommand(SSD1351_CMD_PRECHARGE);
    writeData(0x32);

    // Set precharge2
    writeCommand(SSD1351_CMD_PRECHARGE2);
    writeData(0x01);

    // Set VCOMH
    writeCommand(SSD1351_CMD_VCOMH);
    writeData(0x05);

    // Normal display mode
    writeCommand(SSD1351_CMD_NORMALDISPLAY);

    // Enable display enhancement
    writeCommand(SSD1351_CMD_DISPLAYENHANCE);
    writeData(0xA4);
    writeData(0x00);
    writeData(0x00);

    // Clear display
    setWindow(0, 0, _width - 1, _height - 1);
    for (uint32_t i = 0; i < _width * _height; i++) {
        writeData(0x00);
        writeData(0x00);
    }

    // Display on
    writeCommand(SSD1351_CMD_DISPLAYON);
    delay(100);
}

void TFT_Driver_SSD1351::writeCommand(uint8_t cmd) {
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

void TFT_Driver_SSD1351::writeData(uint8_t data) {
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

void TFT_Driver_SSD1351::writeBlock(uint16_t* data, uint32_t len) {
    if (config.interface == InterfaceMode::SPI) {
        digitalWrite(config.spi.dc_pin, HIGH);
        if (config.spi.cs_pin >= 0) digitalWrite(config.spi.cs_pin, LOW);
        
        // Use hardware SPI for better performance
        for (uint32_t i = 0; i < len; i++) {
            uint16_t color = data[i];
            // SSD1351 expects 18-bit color in RGB format
            uint8_t r = (color & 0xF800) >> 8;  // 5 bits red
            uint8_t g = (color & 0x07E0) >> 3;  // 6 bits green
            uint8_t b = (color & 0x001F) << 3;  // 5 bits blue
            SPI.transfer(r);
            SPI.transfer(g);
            SPI.transfer(b);
        }
        
        if (config.spi.cs_pin >= 0) digitalWrite(config.spi.cs_pin, HIGH);
    } else {
        // Parallel 8-bit implementation
        digitalWrite(config.parallel.dc_pin, HIGH);
        if (config.parallel.cs_pin >= 0) digitalWrite(config.parallel.cs_pin, LOW);
        
        // Write to parallel bus - implementation depends on hardware setup
        for (uint32_t i = 0; i < len; i++) {
            uint16_t color = data[i];
            // Implementation depends on hardware setup
        }
        
        if (config.parallel.cs_pin >= 0) digitalWrite(config.parallel.cs_pin, HIGH);
    }
}

void TFT_Driver_SSD1351::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    uint8_t madctl = 0x74;  // Default: RGB color order, split odd/even COM lines
    
    switch (_rotation) {
        case 0:
            madctl |= 0x00;  // Normal orientation
            _width = SSD1351_TFTWIDTH;
            _height = SSD1351_TFTHEIGHT;
            break;
        case 1:
            madctl |= 0x61;  // 90 degree rotation
            _width = SSD1351_TFTHEIGHT;
            _height = SSD1351_TFTWIDTH;
            break;
        case 2:
            madctl |= 0x62;  // 180 degree rotation
            _width = SSD1351_TFTWIDTH;
            _height = SSD1351_TFTHEIGHT;
            break;
        case 3:
            madctl |= 0x63;  // 270 degree rotation
            _width = SSD1351_TFTHEIGHT;
            _height = SSD1351_TFTWIDTH;
            break;
    }

    writeCommand(SSD1351_CMD_SETREMAP);
    writeData(madctl);
    writeCommand(SSD1351_CMD_STARTLINE);
    writeData(_rotation < 2 ? 0 : SSD1351_TFTHEIGHT - 1);
}

void TFT_Driver_SSD1351::invertDisplay(bool invert) {
    _invert = invert;
    writeCommand(invert ? SSD1351_CMD_INVERTDISPLAY : SSD1351_CMD_NORMALDISPLAY);
}

void TFT_Driver_SSD1351::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(SSD1351_CMD_SETCOLUMN);
    writeData(x0);
    writeData(x1);
    writeCommand(SSD1351_CMD_SETROW);
    writeData(y0);
    writeData(y1);
    writeCommand(SSD1351_CMD_WRITERAM);
}

void TFT_Driver_SSD1351::setContrast(uint8_t contrast) {
    writeCommand(SSD1351_CMD_CONTRASTMASTER);
    writeData(contrast);
}

void TFT_Driver_SSD1351::setContrastColor(uint8_t r, uint8_t g, uint8_t b) {
    writeCommand(SSD1351_CMD_CONTRASTABC);
    writeData(r);
    writeData(g);
    writeData(b);
}

void TFT_Driver_SSD1351::setDisplayEnhancement(bool enable) {
    writeCommand(SSD1351_CMD_DISPLAYENHANCE);
    writeData(enable ? 0xA4 : 0x00);
    writeData(0x00);
    writeData(0x00);
}

void TFT_Driver_SSD1351::setPrecharge(uint8_t phase1, uint8_t phase2) {
    writeCommand(SSD1351_CMD_PRECHARGE);
    writeData(phase1);
    writeCommand(SSD1351_CMD_PRECHARGE2);
    writeData(phase2);
}

void TFT_Driver_SSD1351::setVSL(uint8_t vsl) {
    writeCommand(SSD1351_CMD_SETVSL);
    writeData(0xA0 | (vsl & 0x0F));
}

void TFT_Driver_SSD1351::setCommandLock(bool lock) {
    writeCommand(SSD1351_CMD_COMMANDLOCK);
    writeData(lock ? 0x16 : 0x12);
}

void TFT_Driver_SSD1351::startScrolling(uint8_t direction, uint8_t start, uint8_t stop, uint8_t speed) {
    writeCommand(SSD1351_CMD_HORIZSCROLL);
    writeData(direction);  // 0x00 = right, 0x01 = left
    writeData(start);
    writeData(speed);      // Number of frames between scroll steps
    writeData(stop);
    writeCommand(SSD1351_CMD_STARTSCROLL);
}

void TFT_Driver_SSD1351::stopScrolling() {
    writeCommand(SSD1351_CMD_STOPSCROLL);
}

void TFT_Driver_SSD1351::enableGrayScale(bool enable) {
    writeCommand(SSD1351_CMD_SETGRAY);
    if (enable) {
        for (uint8_t i = 0; i < 63; i++) {
            writeData(i * 4);  // Linear grayscale
        }
    }
}

void TFT_Driver_SSD1351::setMuxRatio(uint8_t ratio) {
    writeCommand(SSD1351_CMD_MUXRATIO);
    writeData(ratio);
}

void TFT_Driver_SSD1351::setDisplayOffset(uint8_t offset) {
    writeCommand(SSD1351_CMD_DISPLAYOFFSET);
    writeData(offset);
}

void TFT_Driver_SSD1351::setStartLine(uint8_t line) {
    writeCommand(SSD1351_CMD_STARTLINE);
    writeData(line);
}

} // namespace TFT_Runtime
