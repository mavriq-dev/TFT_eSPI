/***************************************************
  ILI9486 Display Driver
  
  This file implements the ILI9486 specific driver functionality
  for the TFT_eSPI Runtime Configuration system.
  
  Created: December 2024
 ****************************************************/

#ifndef _ILI9486_DRIVER_H_
#define _ILI9486_DRIVER_H_

#include "../TFT_Runtime.h"
#include "../TFT_Commands.h"

namespace TFT_Runtime {

// ILI9486 Command definitions
enum ILI9486_CMD : uint8_t {
    NOP        = 0x00,
    SWRESET    = 0x01,
    RDDID      = 0x04,
    RDDST      = 0x09,
    RDMODE     = 0x0A,
    RDMADCTL   = 0x0B,
    RDPIXFMT   = 0x0C,
    RDIMGFMT   = 0x0D,
    RDSELFDIAG = 0x0F,
    SLPIN      = 0x10,
    SLPOUT     = 0x11,
    PTLON      = 0x12,
    NORON      = 0x13,
    INVOFF     = 0x20,
    INVON      = 0x21,
    GAMMASET   = 0x26,
    DISPOFF    = 0x28,
    DISPON     = 0x29,
    CASET      = 0x2A,
    PASET      = 0x2B,
    RAMWR      = 0x2C,
    RAMRD      = 0x2E,
    PTLAR      = 0x30,
    VSCRDEF    = 0x33,
    MADCTL     = 0x36,
    VSCRSADD   = 0x37,
    PIXFMT     = 0x3A,
    WRMEMCONT  = 0x3C,
    RDMEMCONT  = 0x3E,
    SETTEST    = 0x44,
    GETSCAN    = 0x45,
    WRDISBV    = 0x51,
    RDDISBV    = 0x52,
    WRCTRLD    = 0x53,
    RDCTRLD    = 0x54,
    WRCABC     = 0x55,
    RDCABC     = 0x56,
    WRCABCMB   = 0x5E,
    RDCABCMB   = 0x5F,
    RDID1      = 0xDA,
    RDID2      = 0xDB,
    RDID3      = 0xDC,
    IFMODE     = 0xB0,
    FRMCTR1    = 0xB1,
    FRMCTR2    = 0xB2,
    FRMCTR3    = 0xB3,
    INVCTR     = 0xB4,
    PRCTR      = 0xB5,
    DISCTRL    = 0xB6,
    ETMOD      = 0xB7,
    PWCTRL1    = 0xC0,
    PWCTRL2    = 0xC1,
    PWCTRL3    = 0xC2,
    VMCTRL1    = 0xC5,
    VMCTRL2    = 0xC7,
    PGAMCTRL   = 0xE0,
    NGAMCTRL   = 0xE1,
    DGAMCTRL   = 0xE2,
    DTCTRLA    = 0xE8,
    DTCTRLB    = 0xEA,
    PWRSEQ     = 0xED,
    GAMMA3ENA  = 0xF2,
    TIMCTRLA   = 0xF8,
    TIMCTRLB   = 0xF9,
    PWCTRLSEQ  = 0xFC,
    PRCTRL     = 0xFE
};

class ILI9486_Driver : public Driver, public CommandExecutor {
public:
    explicit ILI9486_Driver(const Configuration& cfg);
    ~ILI9486_Driver() override = default;
    
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
    
    // ILI9486-specific validation
    static bool validateConfig(const Configuration& config) {
        // First check common driver validation
        if (!Driver::validateConfig(config)) {
            return false;
        }
        
        // ILI9486 is typically 480x320
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

#endif // _ILI9486_DRIVER_H_
