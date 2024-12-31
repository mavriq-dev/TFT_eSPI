/***************************************************
  ILI9163 Display Driver
  
  This file implements the ILI9163 specific driver functionality
  for the TFT_eSPI Runtime Configuration system.
  
  Created: December 2024
 ****************************************************/

#ifndef _ILI9163_DRIVER_H_
#define _ILI9163_DRIVER_H_

#include "../TFT_Runtime.h"
#include "../TFT_Commands.h"

namespace TFT_Runtime {

// ILI9163 Command definitions
enum ILI9163_CMD : uint8_t {
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
    FRMCTR1    = 0xB1,
    FRMCTR2    = 0xB2,
    FRMCTR3    = 0xB3,
    INVCTR     = 0xB4,
    DISSET5    = 0xB6,
    PWCTR1     = 0xC0,
    PWCTR2     = 0xC1,
    PWCTR3     = 0xC2,
    PWCTR4     = 0xC3,
    PWCTR5     = 0xC4,
    VMCTR1     = 0xC5,
    VMCTR2     = 0xC7,
    GMCTRP1    = 0xE0,
    GMCTRN1    = 0xE1,
    GAMMASEL   = 0xF2
};

class ILI9163_Driver : public Driver, public CommandExecutor {
public:
    explicit ILI9163_Driver(const Configuration& cfg);
    ~ILI9163_Driver() override = default;
    
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
    
    // ILI9163-specific validation
    static bool validateConfig(const Configuration& config) {
        // First check common driver validation
        if (!Driver::validateConfig(config)) {
            return false;
        }
        
        // ILI9163 is typically 128x128
        if (config.geometry.width != 128 || config.geometry.height != 128) {
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
    uint16_t _ystart = 2;  // ILI9163 typically has a 2-pixel offset
};

} // namespace TFT_Runtime

#endif // _ILI9163_DRIVER_H_
