/***************************************************
  ST7789 Display Driver
  
  This file implements the ST7789 specific driver functionality
  for the TFT_eSPI Runtime Configuration system.
  
  Created: December 2024
 ****************************************************/

#ifndef _ST7789_DRIVER_H_
#define _ST7789_DRIVER_H_

#include "../TFT_Runtime.h"
#include "../TFT_Commands.h"

namespace TFT_Runtime {

// ST7789 Command definitions
enum ST7789_CMD : uint8_t {
    NOP        = 0x00,
    SWRESET    = 0x01,
    RDDID      = 0x04,
    RDDST      = 0x09,
    SLPIN      = 0x10,
    SLPOUT     = 0x11,
    PTLON      = 0x12,
    NORON      = 0x13,
    INVOFF     = 0x20,
    INVON      = 0x21,
    DISPOFF    = 0x28,
    DISPON     = 0x29,
    CASET      = 0x2A,
    RASET      = 0x2B,
    RAMWR      = 0x2C,
    RAMRD      = 0x2E,
    PTLAR      = 0x30,
    VSCRDEF    = 0x33,
    COLMOD     = 0x3A,
    MADCTL     = 0x36,
    VSCSAD     = 0x37,
    PORCTRL    = 0xB2,
    GCTRL      = 0xB7,
    VCOMS      = 0xBB,
    LCMCTRL    = 0xC0,
    VDVVRHEN   = 0xC2,
    VRHS       = 0xC3,
    VDVS       = 0xC4,
    FRCTRL2    = 0xC6,
    PWCTRL1    = 0xD0,
    PVGAMCTRL  = 0xE0,
    NVGAMCTRL  = 0xE1,
    DGMLUTR    = 0xE2,
    DGMLUTB    = 0xE3,
    GATECTRL   = 0xE4,
    SPI2EN     = 0xE7,
    PWCTRL2    = 0xE8,
    EQCTRL     = 0xE9,
    PROMCTRL   = 0xEC,
    PROMEN     = 0xFA,
    NVMSET     = 0xFC,
    PROMACT    = 0xFE
};

class ST7789_Driver : public Driver, public CommandExecutor {
public:
    explicit ST7789_Driver(const Configuration& cfg);
    ~ST7789_Driver() override = default;
    
    // Initialization
    bool init() override;
    void reset() override;
    
    // Basic operations
    void writeCommand(uint8_t cmd) override;
    void writeData(uint8_t data) override;
    void writeData16(uint16_t data) override;
    uint8_t readData() override;
    
    // Block operations
    void writeBlock(const uint16_t* data, uint32_t len) override;
    void readBlock(uint16_t* data, uint32_t len) override;
    
    // Display control
    void setRotation(uint8_t r) override;
    void invertDisplay(bool i) override;
    void displayOn() override;
    void displayOff() override;
    
    // Window operations
    void setWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1) override;
    
    // Command execution
    void executeCommand(const Command& cmd) override;
    
    // ST7789-specific validation
    static bool validateConfig(const Configuration& config) {
        // First check common driver validation
        if (!Driver::validateConfig(config)) {
            return false;
        }
        
        // ST7789 supports multiple resolutions, common ones are:
        // 240x240, 240x320, 135x240
        bool validResolution = 
            (config.geometry.width == 240 && config.geometry.height == 240) ||
            (config.geometry.width == 240 && config.geometry.height == 320) ||
            (config.geometry.width == 135 && config.geometry.height == 240);
            
        if (!validResolution) {
            return false;
        }
        
        return true;
    }

private:
    // Initialization helpers
    void initBus();
    void initDisplay();
    
    // Pin control helpers
    inline void CS_HIGH() { if(config.pins.cs >= 0) digitalWrite(config.pins.cs, HIGH); }
    inline void CS_LOW()  { if(config.pins.cs >= 0) digitalWrite(config.pins.cs, LOW); }
    inline void DC_HIGH() { if(config.pins.dc >= 0) digitalWrite(config.pins.dc, HIGH); }
    inline void DC_LOW()  { if(config.pins.dc >= 0) digitalWrite(config.pins.dc, LOW); }
    
    // SPI helpers
    void beginSPITransaction();
    void endSPITransaction();
    
    // Window offset helpers
    void setColRowOffset();
    
    // State tracking
    bool _booted = false;
    bool _suspended = false;
    uint16_t _xstart = 0;
    uint16_t _ystart = 0;
};

} // namespace TFT_Runtime

#endif // _ST7789_DRIVER_H_
