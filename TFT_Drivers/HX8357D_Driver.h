/***************************************************
  HX8357D Display Driver
  
  This file implements the HX8357D specific driver functionality
  for the TFT_eSPI Runtime Configuration system.
  
  Created: December 2024
 ****************************************************/

#ifndef _HX8357D_DRIVER_H_
#define _HX8357D_DRIVER_H_

#include "../TFT_Runtime.h"
#include "../TFT_Commands.h"

namespace TFT_Runtime {

// HX8357D Command definitions
enum HX8357D_CMD : uint8_t {
    NOP        = 0x00,
    SWRESET    = 0x01,
    RDDID      = 0x04,
    RDDST      = 0x09,
    RDPOWMODE  = 0x0A,
    RDMADCTL   = 0x0B,
    RDCOLMOD   = 0x0C,
    RDDIM      = 0x0D,
    RDDSDR     = 0x0F,
    SLPIN      = 0x10,
    SLPOUT     = 0x11,
    PTLON      = 0x12,
    NORON      = 0x13,
    INVOFF     = 0x20,
    INVON      = 0x21,
    DISPOFF    = 0x28,
    DISPON     = 0x29,
    CASET      = 0x2A,
    PASET      = 0x2B,
    RAMWR      = 0x2C,
    RAMRD      = 0x2E,
    TEON       = 0x35,
    MADCTL     = 0x36,
    COLMOD     = 0x3A,
    SETOSC     = 0xB0,
    SETPWR1    = 0xB1,
    SETRGB     = 0xB3,
    SETCYC     = 0xB4,
    SETCOM     = 0xB6,
    SETEXTC    = 0xB9,
    SETGAMMA   = 0xC0,
    SETPANEL   = 0xCC,
    SETPOWER   = 0xD0,
    SETVCOM    = 0xD1,
    SETPWRNOR  = 0xD2,
    RDID1      = 0xDA,
    RDID2      = 0xDB,
    RDID3      = 0xDC,
    RDID4      = 0xDD,
    SETPWRPART = 0xE0,
    SETPARTIALSCAN = 0xE1,
    SETGATESCAN = 0xE2,
    SETPARTDISP = 0xE3,
    SETSCROLL  = 0xE4,
    SETDEEPSL  = 0xE5,
    SETVDCS    = 0xE6,
    SETIDLEON  = 0xE7,
    SETIDLEOFF = 0xE8,
    SETPIXFMT  = 0xE9,
    SETDDB     = 0xEA,
    SETVDV     = 0xF0,
    SETVCM     = 0xF1,
    SETGAMMA   = 0xF2,
    SETPWRSAVE = 0xF3,
    SETDIV     = 0xF4,
    SETDGC     = 0xF5,
    SETVCOM    = 0xF6,
    SETMIPI    = 0xF7
};

class HX8357D_Driver : public Driver, public CommandExecutor {
public:
    explicit HX8357D_Driver(const Configuration& cfg);
    ~HX8357D_Driver() override = default;
    
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
    
    // HX8357D-specific validation
    static bool validateConfig(const Configuration& config) {
        // First check common driver validation
        if (!Driver::validateConfig(config)) {
            return false;
        }
        
        // HX8357D is typically 480x320
        if (config.geometry.width != 480 || config.geometry.height != 320) {
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
    
    // Power control helpers
    void setPowerControl();
    void setVCOM();
    
    // State tracking
    bool _booted = false;
    bool _suspended = false;
};

} // namespace TFT_Runtime

#endif // _HX8357D_DRIVER_H_
