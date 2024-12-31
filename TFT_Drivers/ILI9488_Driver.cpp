/***************************************************
  ILI9488 Display Driver Implementation
  
  This file implements the ILI9488 specific driver functionality
  for the TFT_eSPI Runtime Configuration system.
  
  Created: December 2024
 ****************************************************/

#include "ILI9488_Driver.h"
#include "ILI9488_Commands.h"
#include <SPI.h>

namespace TFT_Runtime {

ILI9488_Driver::ILI9488_Driver(const Configuration& cfg) : Driver(cfg) {
    _booted = false;
    _suspended = false;
}

bool ILI9488_Driver::init() {
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

void ILI9488_Driver::reset() {
    if (config.pins.rst >= 0) {
        digitalWrite(config.pins.rst, HIGH);
        delay(10);
        digitalWrite(config.pins.rst, LOW);
        delay(20);
        digitalWrite(config.pins.rst, HIGH);
        delay(120);
    }
}

void ILI9488_Driver::initBus() {
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

void ILI9488_Driver::initDisplay() {
    beginSPITransaction();
    
    // Get and execute initialization sequence
    auto initSeq = ILI9488_Commands::getInitSequence();
    executeSequence(initSeq);
    
    // Set power control settings
    setPowerControl();
    
    // Set VCOM voltage
    setVCOM();
    
    // Set rotation
    setRotation(config.geometry.rotation);
    
    // Set initial backlight level
    setBacklight(255);  // Full brightness by default
    
    endSPITransaction();
}

void ILI9488_Driver::writeCommand(uint8_t cmd) {
    DC_LOW();
    CS_LOW();
    SPI.transfer(cmd);
    CS_HIGH();
}

void ILI9488_Driver::writeData(uint8_t data) {
    DC_HIGH();
    CS_LOW();
    SPI.transfer(data);
    CS_HIGH();
}

void ILI9488_Driver::writeData16(uint16_t data) {
    DC_HIGH();
    CS_LOW();
    // ILI9488 uses 18-bit color, so we need to convert 16-bit to 18-bit
    uint8_t r = (data & 0xF800) >> 8;  // 5 bits red
    uint8_t g = (data & 0x07E0) >> 3;  // 6 bits green
    uint8_t b = (data & 0x001F) << 3;  // 5 bits blue
    SPI.transfer(r);
    SPI.transfer(g);
    SPI.transfer(b);
    CS_HIGH();
}

uint8_t ILI9488_Driver::readData() {
    DC_HIGH();
    CS_LOW();
    uint8_t data = SPI.transfer(0);
    CS_HIGH();
    return data;
}

void ILI9488_Driver::writeBlock(const uint16_t* data, uint32_t len) {
    DC_HIGH();
    CS_LOW();
    
    if (config.performance.use_dma) {
        // TODO: Implement DMA transfer
    } else {
        while (len--) {
            uint16_t color = *data++;
            // Convert 16-bit to 18-bit color
            uint8_t r = (color & 0xF800) >> 8;
            uint8_t g = (color & 0x07E0) >> 3;
            uint8_t b = (color & 0x001F) << 3;
            SPI.transfer(r);
            SPI.transfer(g);
            SPI.transfer(b);
        }
    }
    
    CS_HIGH();
}

void ILI9488_Driver::readBlock(uint16_t* data, uint32_t len) {
    DC_HIGH();
    CS_LOW();
    
    while (len--) {
        // Read 18-bit color and convert to 16-bit
        uint8_t r = SPI.transfer(0);
        uint8_t g = SPI.transfer(0);
        uint8_t b = SPI.transfer(0);
        
        // Convert 18-bit to 16-bit color
        *data++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }
    
    CS_HIGH();
}

void ILI9488_Driver::setRotation(uint8_t r) {
    uint8_t madctl = 0;
    
    switch (r & 0x03) {
        case 0:  // Portrait
            madctl = 0x48;
            break;
        case 1:  // Landscape
            madctl = 0x28;
            break;
        case 2:  // Portrait inverted
            madctl = 0x88;
            break;
        case 3:  // Landscape inverted
            madctl = 0xE8;
            break;
    }
    
    // Add color order bit
    madctl |= config.geometry.madctl.colorOrder ? 0x08 : 0x00;
    
    writeCommand(ILI9488_CMD::MADCTL);
    writeData(madctl);
}

void ILI9488_Driver::invertDisplay(bool i) {
    auto invSeq = ILI9488_Commands::getInversionSequence(i);
    executeSequence(invSeq);
}

void ILI9488_Driver::displayOn() {
    if (_suspended) {
        auto wakeSeq = ILI9488_Commands::getWakeSequence();
        executeSequence(wakeSeq);
        _suspended = false;
    } else {
        writeCommand(ILI9488_CMD::DISPON);
    }
}

void ILI9488_Driver::displayOff() {
    if (!_suspended) {
        auto sleepSeq = ILI9488_Commands::getSleepSequence();
        executeSequence(sleepSeq);
        _suspended = true;
    } else {
        writeCommand(ILI9488_CMD::DISPOFF);
    }
}

void ILI9488_Driver::setWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    writeCommand(ILI9488_CMD::CASET);
    writeData16(x0);
    writeData16(x1);
    
    writeCommand(ILI9488_CMD::PASET);
    writeData16(y0);
    writeData16(y1);
    
    writeCommand(ILI9488_CMD::RAMWR);
}

void ILI9488_Driver::executeCommand(const Command& cmd) {
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

void ILI9488_Driver::beginSPITransaction() {
    if (config.interface == InterfaceMode::SPI) {
        SPI.beginTransaction(SPISettings(
            config.timing.freq_write,
            MSBFIRST,
            config.timing.spi_mode
        ));
    }
}

void ILI9488_Driver::endSPITransaction() {
    if (config.interface == InterfaceMode::SPI) {
        SPI.endTransaction();
    }
}

void ILI9488_Driver::setPowerControl() {
    auto powerSeq = ILI9488_Commands::getPowerControlSequence();
    executeSequence(powerSeq);
}

void ILI9488_Driver::setVCOM() {
    auto vcomSeq = ILI9488_Commands::getVCOMSequence();
    executeSequence(vcomSeq);
}

void ILI9488_Driver::setBacklight(uint8_t level) {
    auto blSeq = ILI9488_Commands::getBacklightSequence(level);
    executeSequence(blSeq);
}

} // namespace TFT_Runtime
