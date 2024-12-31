/***************************************************
  ILI9163 Display Driver Implementation
  
  This file implements the ILI9163 specific driver functionality
  for the TFT_eSPI Runtime Configuration system.
  
  Created: December 2024
 ****************************************************/

#include "ILI9163_Driver.h"
#include "ILI9163_Commands.h"
#include <SPI.h>

namespace TFT_Runtime {

ILI9163_Driver::ILI9163_Driver(const Configuration& cfg) : Driver(cfg) {
    _booted = false;
    _suspended = false;
}

bool ILI9163_Driver::init() {
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

void ILI9163_Driver::reset() {
    if (config.pins.rst >= 0) {
        digitalWrite(config.pins.rst, HIGH);
        delay(5);
        digitalWrite(config.pins.rst, LOW);
        delay(20);
        digitalWrite(config.pins.rst, HIGH);
        delay(150);
    }
}

void ILI9163_Driver::initBus() {
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

void ILI9163_Driver::initDisplay() {
    beginSPITransaction();
    
    // Get and execute initialization sequence
    auto initSeq = ILI9163_Commands::getInitSequence();
    executeSequence(initSeq);
    
    // Set rotation
    setRotation(config.geometry.rotation);
    
    // Set column and row offsets
    setColRowOffset();
    
    endSPITransaction();
}

void ILI9163_Driver::writeCommand(uint8_t cmd) {
    DC_LOW();
    CS_LOW();
    SPI.transfer(cmd);
    CS_HIGH();
}

void ILI9163_Driver::writeData(uint8_t data) {
    DC_HIGH();
    CS_LOW();
    SPI.transfer(data);
    CS_HIGH();
}

void ILI9163_Driver::writeData16(uint16_t data) {
    DC_HIGH();
    CS_LOW();
    SPI.transfer16(data);
    CS_HIGH();
}

uint8_t ILI9163_Driver::readData() {
    DC_HIGH();
    CS_LOW();
    uint8_t data = SPI.transfer(0);
    CS_HIGH();
    return data;
}

void ILI9163_Driver::writeBlock(const uint16_t* data, uint32_t len) {
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

void ILI9163_Driver::readBlock(uint16_t* data, uint32_t len) {
    DC_HIGH();
    CS_LOW();
    
    while (len--) {
        *data++ = SPI.transfer16(0);
    }
    
    CS_HIGH();
}

void ILI9163_Driver::setRotation(uint8_t r) {
    uint8_t madctl = 0;
    
    switch (r & 0x03) {
        case 0:  // Portrait
            madctl = 0x40 | 0x08;
            break;
        case 1:  // Landscape
            madctl = 0x20 | 0x08;
            break;
        case 2:  // Portrait inverted
            madctl = 0x80 | 0x08;
            break;
        case 3:  // Landscape inverted
            madctl = 0x40 | 0x80 | 0x20 | 0x08;
            break;
    }
    
    // Add color order bit
    madctl |= config.geometry.madctl.colorOrder ? 0x08 : 0x00;
    
    writeCommand(ILI9163_CMD::MADCTL);
    writeData(madctl);
}

void ILI9163_Driver::invertDisplay(bool i) {
    auto invSeq = ILI9163_Commands::getInversionSequence(i);
    executeSequence(invSeq);
}

void ILI9163_Driver::displayOn() {
    if (_suspended) {
        auto wakeSeq = ILI9163_Commands::getWakeSequence();
        executeSequence(wakeSeq);
        _suspended = false;
    } else {
        writeCommand(ILI9163_CMD::DISPON);
    }
}

void ILI9163_Driver::displayOff() {
    if (!_suspended) {
        auto sleepSeq = ILI9163_Commands::getSleepSequence();
        executeSequence(sleepSeq);
        _suspended = true;
    } else {
        writeCommand(ILI9163_CMD::DISPOFF);
    }
}

void ILI9163_Driver::setWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    // Add offsets
    x0 += _xstart;
    x1 += _xstart;
    y0 += _ystart;
    y1 += _ystart;
    
    writeCommand(ILI9163_CMD::CASET);
    writeData16(x0);
    writeData16(x1);
    
    writeCommand(ILI9163_CMD::RASET);
    writeData16(y0);
    writeData16(y1);
    
    writeCommand(ILI9163_CMD::RAMWR);
}

void ILI9163_Driver::executeCommand(const Command& cmd) {
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

void ILI9163_Driver::beginSPITransaction() {
    if (config.interface == InterfaceMode::SPI) {
        SPI.beginTransaction(SPISettings(
            config.timing.freq_write,
            MSBFIRST,
            config.timing.spi_mode
        ));
    }
}

void ILI9163_Driver::endSPITransaction() {
    if (config.interface == InterfaceMode::SPI) {
        SPI.endTransaction();
    }
}

void ILI9163_Driver::setColRowOffset() {
    writeCommand(ILI9163_CMD::CASET);
    writeData16(_xstart);
    writeData16(_xstart + config.geometry.width - 1);
    
    writeCommand(ILI9163_CMD::RASET);
    writeData16(_ystart);
    writeData16(_ystart + config.geometry.height - 1);
}

} // namespace TFT_Runtime
