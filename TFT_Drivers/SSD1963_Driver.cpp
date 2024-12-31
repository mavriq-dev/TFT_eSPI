/***************************************************
  SSD1963 Display Driver Implementation
  
  This file implements the SSD1963 specific driver functionality
  for the TFT_eSPI Runtime Configuration system.
  
  Created: December 2024
 ****************************************************/

#include "SSD1963_Driver.h"
#include "SSD1963_Commands.h"
#include <SPI.h>

namespace TFT_Runtime {

SSD1963_Driver::SSD1963_Driver(const Configuration& cfg) : Driver(cfg) {
    _booted = false;
    _suspended = false;
    _pixelClock = 100000000;  // Default to 100MHz
    
    // Default LCD timing for 480x272 panel
    _timing = {
        .horizontalTotal = 531,
        .horizontalDisplay = 480,
        .horizontalSyncStart = 43,
        .horizontalSyncWidth = 8,
        .verticalTotal = 288,
        .verticalDisplay = 272,
        .verticalSyncStart = 12,
        .verticalSyncWidth = 4
    };
}

bool SSD1963_Driver::init() {
    if (_booted) return true;
    
    // Initialize bus
    initBus();
    
    // Hardware reset
    reset();
    
    // Initialize PLL
    initPLL();
    
    // Initialize LCD
    initLCD();
    
    // Initialize PWM
    initPWM();
    
    // Initialize display
    initDisplay();
    
    _booted = true;
    return true;
}

void SSD1963_Driver::reset() {
    if (config.pins.rst >= 0) {
        digitalWrite(config.pins.rst, HIGH);
        delay(10);
        digitalWrite(config.pins.rst, LOW);
        delay(20);
        digitalWrite(config.pins.rst, HIGH);
        delay(120);
    }
}

void SSD1963_Driver::initBus() {
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

void SSD1963_Driver::initPLL() {
    // Configure PLL with default settings
    setPLL(35, 2);  // Multiply by 35, divide by 2
}

void SSD1963_Driver::initLCD() {
    beginSPITransaction();
    
    // Set LCD mode
    setLCDMode(0x20);  // 24-bit TFT mode
    
    // Set LCD timing
    setLCDTiming(_timing);
    
    endSPITransaction();
}

void SSD1963_Driver::initPWM() {
    // Configure PWM for backlight control
    setPWM(6, 255, 0);  // Frequency = PLL/(256*(1+5)), full brightness
}

void SSD1963_Driver::initDisplay() {
    beginSPITransaction();
    
    // Get and execute initialization sequence
    auto initSeq = SSD1963_Commands::getInitSequence(config);
    executeSequence(initSeq);
    
    // Set rotation
    setRotation(config.geometry.rotation);
    
    // Enable tearing effect
    enableTearingEffect(true);
    
    endSPITransaction();
}

void SSD1963_Driver::writeCommand(uint8_t cmd) {
    DC_LOW();
    CS_LOW();
    SPI.transfer(cmd);
    CS_HIGH();
}

void SSD1963_Driver::writeData(uint8_t data) {
    DC_HIGH();
    CS_LOW();
    SPI.transfer(data);
    CS_HIGH();
}

void SSD1963_Driver::writeData16(uint16_t data) {
    DC_HIGH();
    CS_LOW();
    SPI.transfer16(data);
    CS_HIGH();
}

uint8_t SSD1963_Driver::readData() {
    DC_HIGH();
    CS_LOW();
    uint8_t data = SPI.transfer(0);
    CS_HIGH();
    return data;
}

void SSD1963_Driver::writeBlock(const uint16_t* data, uint32_t len) {
    DC_HIGH();
    CS_LOW();
    
    if (config.performance.use_dma) {
        // TODO: Implement DMA transfer
    } else {
        while (len--) {
            uint16_t color = *data++;
            // Convert 16-bit to 24-bit color
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

void SSD1963_Driver::readBlock(uint16_t* data, uint32_t len) {
    DC_HIGH();
    CS_LOW();
    
    while (len--) {
        // Read 24-bit color and convert to 16-bit
        uint8_t r = SPI.transfer(0);
        uint8_t g = SPI.transfer(0);
        uint8_t b = SPI.transfer(0);
        
        // Convert 24-bit to 16-bit color
        *data++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }
    
    CS_HIGH();
}

void SSD1963_Driver::setRotation(uint8_t r) {
    uint8_t madctl = 0;
    
    switch (r & 0x03) {
        case 0:  // Portrait
            madctl = 0x00;
            break;
        case 1:  // Landscape
            madctl = 0x20;
            break;
        case 2:  // Portrait inverted
            madctl = 0x40;
            break;
        case 3:  // Landscape inverted
            madctl = 0x60;
            break;
    }
    
    writeCommand(SSD1963_CMD::SET_ADDR_MODE);
    writeData(madctl);
}

void SSD1963_Driver::invertDisplay(bool i) {
    writeCommand(i ? SSD1963_CMD::ENTER_INVERT : SSD1963_CMD::EXIT_INVERT);
}

void SSD1963_Driver::displayOn() {
    if (_suspended) {
        auto wakeSeq = SSD1963_Commands::getWakeSequence();
        executeSequence(wakeSeq);
        _suspended = false;
    } else {
        writeCommand(SSD1963_CMD::ON_DISPLAY);
    }
}

void SSD1963_Driver::displayOff() {
    if (!_suspended) {
        auto sleepSeq = SSD1963_Commands::getSleepSequence();
        executeSequence(sleepSeq);
        _suspended = true;
    } else {
        writeCommand(SSD1963_CMD::BLANK_DISPLAY);
    }
}

void SSD1963_Driver::setWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    writeCommand(SSD1963_CMD::SET_COLUMN);
    writeData16(x0);
    writeData16(x1);
    
    writeCommand(SSD1963_CMD::SET_PAGE);
    writeData16(y0);
    writeData16(y1);
    
    writeCommand(SSD1963_CMD::WR_MEMSTART);
}

void SSD1963_Driver::executeCommand(const Command& cmd) {
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

void SSD1963_Driver::beginSPITransaction() {
    if (config.interface == InterfaceMode::SPI) {
        SPI.beginTransaction(SPISettings(
            config.timing.freq_write,
            MSBFIRST,
            config.timing.spi_mode
        ));
    }
}

void SSD1963_Driver::endSPITransaction() {
    if (config.interface == InterfaceMode::SPI) {
        SPI.endTransaction();
    }
}

void SSD1963_Driver::setPLL(uint8_t mult, uint8_t div) {
    auto pllSeq = SSD1963_Commands::getPLLSequence(mult, div);
    executeSequence(pllSeq);
}

void SSD1963_Driver::setLCDMode(uint8_t mode) {
    writeCommand(SSD1963_CMD::SET_LCD_MODE);
    writeData(mode);
    writeData((config.geometry.width >> 8) & 0xFF);
    writeData(config.geometry.width & 0xFF);
    writeData((config.geometry.height >> 8) & 0xFF);
    writeData(config.geometry.height & 0xFF);
    writeData(0x00);  // RGB mode
}

void SSD1963_Driver::setPixelClock(uint32_t clock) {
    _pixelClock = clock;
    // TODO: Calculate and set PLL parameters based on desired pixel clock
}

void SSD1963_Driver::setLCDTiming(const LCDTiming& timing) {
    _timing = timing;
    auto timingSeq = SSD1963_Commands::getLCDTimingSequence(timing);
    executeSequence(timingSeq);
}

void SSD1963_Driver::setGPIO(uint8_t gpio, bool value) {
    writeCommand(SSD1963_CMD::SET_GPIO_VAL);
    writeData((1 << gpio) | (value ? (1 << gpio) : 0));
}

void SSD1963_Driver::setPWM(uint8_t prescaler, uint8_t compare, uint8_t period) {
    auto pwmSeq = SSD1963_Commands::getPWMSequence(prescaler, compare, period);
    executeSequence(pwmSeq);
}

void SSD1963_Driver::setBacklight(uint8_t level) {
    auto blSeq = SSD1963_Commands::getBacklightSequence(level);
    executeSequence(blSeq);
}

void SSD1963_Driver::enableTearingEffect(bool enable, bool vblank) {
    auto teSeq = SSD1963_Commands::getTearingSequence(enable, vblank);
    executeSequence(teSeq);
}

} // namespace TFT_Runtime
