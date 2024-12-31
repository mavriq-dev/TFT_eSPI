/***************************************************
  HX8357D Display Driver Implementation
  
  This file implements the HX8357D specific driver functionality
  for the TFT_eSPI Runtime Configuration system.
  
  Created: December 2024
 ****************************************************/

#include "HX8357D_Driver.h"
#include "HX8357D_Commands.h"
#include <SPI.h>

namespace TFT_Runtime {

HX8357D_Driver::HX8357D_Driver(const Configuration& cfg) : Driver(cfg) {
    _booted = false;
    _suspended = false;
}

bool HX8357D_Driver::init() {
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

void HX8357D_Driver::reset() {
    if (config.pins.rst >= 0) {
        digitalWrite(config.pins.rst, HIGH);
        delay(10);
        digitalWrite(config.pins.rst, LOW);
        delay(20);
        digitalWrite(config.pins.rst, HIGH);
        delay(200);  // HX8357D needs longer reset time
    }
}

void HX8357D_Driver::initBus() {
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

void HX8357D_Driver::initDisplay() {
    beginSPITransaction();
    
    // Get and execute initialization sequence
    auto initSeq = HX8357D_Commands::getInitSequence();
    executeSequence(initSeq);
    
    // Set power control settings
    setPowerControl();
    
    // Set VCOM voltage
    setVCOM();
    
    // Set rotation
    setRotation(config.geometry.rotation);
    
    endSPITransaction();
}

void HX8357D_Driver::writeCommand(uint8_t cmd) {
    DC_LOW();
    CS_LOW();
    SPI.transfer(cmd);
    CS_HIGH();
}

void HX8357D_Driver::writeData(uint8_t data) {
    DC_HIGH();
    CS_LOW();
    SPI.transfer(data);
    CS_HIGH();
}

void HX8357D_Driver::writeData16(uint16_t data) {
    DC_HIGH();
    CS_LOW();
    SPI.transfer16(data);
    CS_HIGH();
}

uint8_t HX8357D_Driver::readData() {
    DC_HIGH();
    CS_LOW();
    uint8_t data = SPI.transfer(0);
    CS_HIGH();
    return data;
}

void HX8357D_Driver::writeBlock(const uint16_t* data, uint32_t len) {
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

void HX8357D_Driver::readBlock(uint16_t* data, uint32_t len) {
    DC_HIGH();
    CS_LOW();
    
    while (len--) {
        *data++ = SPI.transfer16(0);
    }
    
    CS_HIGH();
}

void HX8357D_Driver::setRotation(uint8_t r) {
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
    
    writeCommand(HX8357D_CMD::MADCTL);
    writeData(madctl);
}

void HX8357D_Driver::invertDisplay(bool i) {
    auto invSeq = HX8357D_Commands::getInversionSequence(i);
    executeSequence(invSeq);
}

void HX8357D_Driver::displayOn() {
    if (_suspended) {
        auto wakeSeq = HX8357D_Commands::getWakeSequence();
        executeSequence(wakeSeq);
        _suspended = false;
    } else {
        writeCommand(HX8357D_CMD::DISPON);
    }
}

void HX8357D_Driver::displayOff() {
    if (!_suspended) {
        auto sleepSeq = HX8357D_Commands::getSleepSequence();
        executeSequence(sleepSeq);
        _suspended = true;
    } else {
        writeCommand(HX8357D_CMD::DISPOFF);
    }
}

void HX8357D_Driver::setWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    writeCommand(HX8357D_CMD::CASET);
    writeData16(x0);
    writeData16(x1);
    
    writeCommand(HX8357D_CMD::PASET);
    writeData16(y0);
    writeData16(y1);
    
    writeCommand(HX8357D_CMD::RAMWR);
}

void HX8357D_Driver::executeCommand(const Command& cmd) {
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

void HX8357D_Driver::beginSPITransaction() {
    if (config.interface == InterfaceMode::SPI) {
        SPI.beginTransaction(SPISettings(
            config.timing.freq_write,
            MSBFIRST,
            config.timing.spi_mode
        ));
    }
}

void HX8357D_Driver::endSPITransaction() {
    if (config.interface == InterfaceMode::SPI) {
        SPI.endTransaction();
    }
}

void HX8357D_Driver::setPowerControl() {
    auto powerSeq = HX8357D_Commands::getPowerControlSequence();
    executeSequence(powerSeq);
}

void HX8357D_Driver::setVCOM() {
    auto vcomSeq = HX8357D_Commands::getVCOMSequence();
    executeSequence(vcomSeq);
}

} // namespace TFT_Runtime
