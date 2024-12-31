/***************************************************
  ST7735 Display Driver Implementation
  
  This file implements the ST7735 specific driver functionality
  for the TFT_eSPI Runtime Configuration system.
  
  Created: December 2024
 ****************************************************/

#include "ST7735_Driver.h"
#include "ST7735_Commands.h"
#include <SPI.h>

namespace TFT_Runtime {

ST7735_Driver::ST7735_Driver(const Configuration& cfg) : Driver(cfg) {
    _booted = false;
    _suspended = false;
}

bool ST7735_Driver::init() {
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

void ST7735_Driver::reset() {
    if (config.pins.rst >= 0) {
        digitalWrite(config.pins.rst, HIGH);
        delay(5);
        digitalWrite(config.pins.rst, LOW);
        delay(20);
        digitalWrite(config.pins.rst, HIGH);
        delay(150);
    }
}

void ST7735_Driver::initBus() {
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

void ST7735_Driver::initDisplay() {
    beginSPITransaction();
    
    // Get and execute initialization sequence for the specific variant
    auto initSeq = ST7735_Commands::getInitSequence(config.driver_config.st7735.variant);
    executeSequence(initSeq);
    
    // Set rotation
    setRotation(config.geometry.rotation);
    
    endSPITransaction();
}

void ST7735_Driver::writeCommand(uint8_t cmd) {
    DC_LOW();
    CS_LOW();
    SPI.transfer(cmd);
    CS_HIGH();
}

void ST7735_Driver::writeData(uint8_t data) {
    DC_HIGH();
    CS_LOW();
    SPI.transfer(data);
    CS_HIGH();
}

void ST7735_Driver::writeData16(uint16_t data) {
    DC_HIGH();
    CS_LOW();
    SPI.transfer16(data);
    CS_HIGH();
}

uint8_t ST7735_Driver::readData() {
    DC_HIGH();
    CS_LOW();
    uint8_t data = SPI.transfer(0);
    CS_HIGH();
    return data;
}

void ST7735_Driver::writeBlock(const uint16_t* data, uint32_t len) {
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

void ST7735_Driver::readBlock(uint16_t* data, uint32_t len) {
    DC_HIGH();
    CS_LOW();
    
    while (len--) {
        *data++ = SPI.transfer16(0);
    }
    
    CS_HIGH();
}

void ST7735_Driver::setRotation(uint8_t r) {
    uint8_t madctl = 0;
    
    switch (r & 0x03) {
        case 0:  // Portrait
            madctl = 0x00;
            config.geometry.width = config.geometry.height;
            config.geometry.height = config.geometry.width;
            break;
        case 1:  // Landscape
            madctl = 0x60;
            break;
        case 2:  // Portrait inverted
            madctl = 0xC0;
            config.geometry.width = config.geometry.height;
            config.geometry.height = config.geometry.width;
            break;
        case 3:  // Landscape inverted
            madctl = 0xA0;
            break;
    }
    
    // Add color order bit
    madctl |= config.geometry.madctl.colorOrder ? 0x08 : 0x00;
    
    writeCommand(ST7735_CMD::MADCTL);
    writeData(madctl);
}

void ST7735_Driver::invertDisplay(bool i) {
    writeCommand(i ? ST7735_CMD::INVON : ST7735_CMD::INVOFF);
}

void ST7735_Driver::displayOn() {
    writeCommand(ST7735_CMD::DISPON);
}

void ST7735_Driver::displayOff() {
    writeCommand(ST7735_CMD::DISPOFF);
}

void ST7735_Driver::setWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    // Account for screen specific offsets
    x0 += config.geometry.colStart;
    x1 += config.geometry.colStart;
    y0 += config.geometry.rowStart;
    y1 += config.geometry.rowStart;
    
    writeCommand(ST7735_CMD::CASET);
    writeData16(x0);
    writeData16(x1);
    
    writeCommand(ST7735_CMD::RASET);
    writeData16(y0);
    writeData16(y1);
    
    writeCommand(ST7735_CMD::RAMWR);
}

void ST7735_Driver::executeCommand(const Command& cmd) {
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

void ST7735_Driver::executeSequence(const std::vector<Command>& seq) {
    for (const auto& cmd : seq) {
        executeCommand(cmd);
    }
}

void ST7735_Driver::beginSPITransaction() {
    if (config.interface == InterfaceMode::SPI) {
        SPI.beginTransaction(SPISettings(
            config.timing.freq_write,
            MSBFIRST,
            config.timing.spi_mode
        ));
    }
}

void ST7735_Driver::endSPITransaction() {
    if (config.interface == InterfaceMode::SPI) {
        SPI.endTransaction();
    }
}

} // namespace TFT_Runtime
