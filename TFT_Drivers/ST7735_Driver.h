/***************************************************
  ST7735 Display Driver Implementation
  
  This file implements the ST7735 specific driver functionality
  for the TFT_eSPI Runtime Configuration system.
  
  Created: December 2024
 ****************************************************/

#ifndef _ST7735_DRIVER_H_
#define _ST7735_DRIVER_H_

#include "../TFT_Runtime.h"
#include "../TFT_Commands.h"

namespace TFT_Runtime {

// ST7735 Command definitions
enum ST7735_CMD : uint8_t {
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
    RDID1      = 0xDA,
    RDID2      = 0xDB,
    RDID3      = 0xDC,
    RDID4      = 0xDD,
    GMCTRP1    = 0xE0,
    GMCTRN1    = 0xE1
};

class ST7735_Driver : public Driver, public CommandExecutor {
public:
    explicit ST7735_Driver(const Configuration& cfg);
    ~ST7735_Driver() override = default;
    
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
    
    // ST7735-specific validation
    static bool validateConfig(const Configuration& config) {
        // First check common driver validation
        if (!Driver::validateConfig(config)) {
            return false;
        }
        
        // Validate dimensions based on variant
        switch (config.driver_config.st7735.variant) {
            case ST7735Variant::INITR_GREENTAB:
            case ST7735Variant::INITR_REDTAB:
            case ST7735Variant::INITR_BLACKTAB:
                if (config.geometry.width != 128 || config.geometry.height != 160) {
                    return false;
                }
                break;
                
            case ST7735Variant::INITR_GREENTAB128:
                if (config.geometry.width != 128 || config.geometry.height != 128) {
                    return false;
                }
                break;
                
            case ST7735Variant::INITR_GREENTAB160x80:
            case ST7735Variant::INITR_REDTAB160x80:
                if (config.geometry.width != 160 || config.geometry.height != 80) {
                    return false;
                }
                break;
                
            default:
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
    
    // State tracking
    bool _booted = false;
    bool _suspended = false;
};

} // namespace TFT_Runtime

#endif // _ST7735_DRIVER_H_
