/***************************************************
  ST7789 Display Driver Implementation
  
  This file implements the ST7789 specific driver functionality
  for the TFT_eSPI Runtime Configuration system.
  
  Created: December 2024
 ****************************************************/

#include "ST7789_Driver.h"
#include "ST7789_Commands.h"
#include <SPI.h>

namespace TFT_Runtime {

ST7789_Driver::ST7789_Driver(const Configuration& cfg) : Driver(cfg) {
    _booted = false;
    _suspended = false;
    
    // Set display offsets based on resolution
    if (config.geometry.width == 240 && config.geometry.height == 240) {
        _xstart = 0;
        _ystart = 0;
    } else if (config.geometry.width == 240 && config.geometry.height == 320) {
        _xstart = 0;
        _ystart = 0;
    } else if (config.geometry.width == 135 && config.geometry.height == 240) {
        _xstart = 52;
        _ystart = 40;
    }
}

bool ST7789_Driver::init() {
    if (_booted) return true;
    
    // Initialize bus
    initBus();
    
    // Hardware reset
    reset();
    
    // Initialize display
    initDisplay();
    
    _booted = true;
    return true;
}

void ST7789_Driver::reset() {
    if (config.pins.rst >= 0) {
        digitalWrite(config.pins.rst, HIGH);
        delay(5);
        digitalWrite(config.pins.rst, LOW);
        delay(20);
        digitalWrite(config.pins.rst, HIGH);
        delay(150);
    }
}

void ST7789_Driver::initBus() {
    // Initialize pins
    if (config.pins.cs >= 0) {
        pinMode(config.pins.cs, OUTPUT);
        digitalWrite(config.pins.cs, HIGH);
    }
    
    if (config.pins.dc >= 0) {
        pinMode(config.pins.dc, OUTPUT);
        digitalWrite(config.pins.dc, HIGH);
    }
    
    if (config.pins.rst >= 0) {
        pinMode(config.pins.rst, OUTPUT);
        digitalWrite(config.pins.rst, HIGH);
    }
    
    if (config.interface == InterfaceMode::SPI) {
        SPI.begin();
    }
}

void ST7789_Driver::initDisplay() {
    beginSPITransaction();
    
    // Get and execute initialization sequence
    auto initSeq = ST7789_Commands::getInitSequence();
    executeSequence(initSeq);
    
    // Set rotation
    setRotation(config.geometry.rotation);
    
    // Set column and row offsets
    setColRowOffset();
    
    endSPITransaction();
}

void ST7789_Driver::writeCommand(uint8_t cmd) {
    DC_LOW();
    CS_LOW();
    SPI.transfer(cmd);
    CS_HIGH();
}

void ST7789_Driver::writeData(uint8_t data) {
    DC_HIGH();
    CS_LOW();
    SPI.transfer(data);
    CS_HIGH();
}

void ST7789_Driver::writeData16(uint16_t data) {
    DC_HIGH();
    CS_LOW();
    SPI.transfer16(data);
    CS_HIGH();
}

uint8_t ST7789_Driver::readData() {
    DC_HIGH();
    CS_LOW();
    uint8_t data = SPI.transfer(0);
    CS_HIGH();
    return data;
}

void ST7789_Driver::writeBlock(const uint16_t* data, uint32_t len) {
    DC_HIGH();
    CS_LOW();
    
    if (config.performance.use_dma) {
        // TODO: Implement DMA transfer
    } else {
        while (len--) {
            SPI.transfer16(*data++);
        }
    }
    
    CS_HIGH();
}

void ST7789_Driver::readBlock(uint16_t* data, uint32_t len) {
    DC_HIGH();
    CS_LOW();
    
    while (len--) {
        *data++ = SPI.transfer16(0);
    }
    
    CS_HIGH();
}

void ST7789_Driver::setRotation(uint8_t r) {
    uint8_t madctl = 0;
    
    switch (r & 0x03) {
        case 0:  // Portrait
            madctl = 0x00;
            break;
        case 1:  // Landscape
            madctl = 0x60;
            break;
        case 2:  // Portrait inverted
            madctl = 0xC0;
            break;
        case 3:  // Landscape inverted
            madctl = 0xA0;
            break;
    }
    
    // Add color order bit
    madctl |= config.geometry.madctl.colorOrder ? 0x08 : 0x00;
    
    writeCommand(ST7789_CMD::MADCTL);
    writeData(madctl);
}

void ST7789_Driver::invertDisplay(bool i) {
    auto invSeq = ST7789_Commands::getInversionSequence(i);
    executeSequence(invSeq);
}

void ST7789_Driver::displayOn() {
    if (_suspended) {
        auto wakeSeq = ST7789_Commands::getWakeSequence();
        executeSequence(wakeSeq);
        _suspended = false;
    } else {
        writeCommand(ST7789_CMD::DISPON);
    }
}

void ST7789_Driver::displayOff() {
    if (!_suspended) {
        auto sleepSeq = ST7789_Commands::getSleepSequence();
        executeSequence(sleepSeq);
        _suspended = true;
    } else {
        writeCommand(ST7789_CMD::DISPOFF);
    }
}

void ST7789_Driver::setWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    // Add offsets
    x0 += _xstart;
    x1 += _xstart;
    y0 += _ystart;
    y1 += _ystart;
    
    writeCommand(ST7789_CMD::CASET);
    writeData16(x0);
    writeData16(x1);
    
    writeCommand(ST7789_CMD::RASET);
    writeData16(y0);
    writeData16(y1);
    
    writeCommand(ST7789_CMD::RAMWR);
}

void ST7789_Driver::executeCommand(const Command& cmd) {
    switch (cmd.type) {
        case CommandType::COMMAND:
            writeCommand(cmd.value);
            break;
            
        case CommandType::DATA:
            writeData(cmd.value);
            break;
            
        case CommandType::DATA16:
            writeData16(cmd.value);
            break;
            
        case CommandType::DELAY:
            delay(cmd.delay_ms);
            break;
            
        case CommandType::END_SEQUENCE:
            break;
    }
}

void ST7789_Driver::beginSPITransaction() {
    if (config.interface == InterfaceMode::SPI) {
        SPI.beginTransaction(SPISettings(
            config.timing.freq_write,
            MSBFIRST,
            config.timing.spi_mode
        ));
    }
}

void ST7789_Driver::endSPITransaction() {
    if (config.interface == InterfaceMode::SPI) {
        SPI.endTransaction();
    }
}

void ST7789_Driver::setColRowOffset() {
    writeCommand(ST7789_CMD::CASET);
    writeData16(_xstart);
    writeData16(_xstart + config.geometry.width - 1);
    
    writeCommand(ST7789_CMD::RASET);
    writeData16(_ystart);
    writeData16(_ystart + config.geometry.height - 1);
}

} // namespace TFT_Runtime
