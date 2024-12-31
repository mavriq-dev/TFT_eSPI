/***************************************************
  SSD1963 Display Driver
  
  This file implements the SSD1963 specific driver functionality
  for the TFT_eSPI Runtime Configuration system.
  
  The SSD1963 is a sophisticated display controller with:
  - Built-in display buffer
  - Support for multiple resolutions up to 864×480
  - Hardware acceleration
  - Advanced power management
  
  Created: December 2024
 ****************************************************/

#ifndef _SSD1963_DRIVER_H_
#define _SSD1963_DRIVER_H_

#include "../TFT_Runtime.h"
#include "../TFT_Commands.h"

namespace TFT_Runtime {

// SSD1963 Command definitions
enum SSD1963_CMD : uint16_t {
    NOP         = 0x0000,
    SOFT_RESET  = 0x0001,
    GET_PWR_MODE = 0x000A,
    GET_ADDR_MODE = 0x000B,
    GET_PIXEL_FMT = 0x000C,
    GET_DISPLAY_MODE = 0x000D,
    GET_SIGNAL_MODE = 0x000E,
    GET_DIAGNOSTIC = 0x000F,
    ENT_SLEEP   = 0x0010,
    EXIT_SLEEP  = 0x0011,
    ENT_PARTIAL = 0x0012,
    ENT_NORMAL  = 0x0013,
    EXIT_INVERT = 0x0020,
    ENTER_INVERT = 0x0021,
    SET_GAMMA   = 0x0026,
    BLANK_DISPLAY = 0x0028,
    ON_DISPLAY  = 0x0029,
    SET_COLUMN  = 0x002A,
    SET_PAGE    = 0x002B,
    WR_MEMSTART = 0x002C,
    RD_MEMSTART = 0x002E,
    SET_PARTIAL = 0x0030,
    SET_SCROLL  = 0x0033,
    SET_TEAR_OFF = 0x0034,
    SET_TEAR_ON = 0x0035,
    SET_ADDR_MODE = 0x0036,
    SET_SCROLL_START = 0x0037,
    EXIT_IDLE   = 0x0038,
    ENT_IDLE    = 0x0039,
    SET_PIXEL_FMT = 0x003A,
    WR_MEM_AUTO = 0x003C,
    RD_MEM_AUTO = 0x003E,
    SET_TEAR_SCANLINE = 0x0044,
    GET_SCANLINE = 0x0045,
    RD_DDB_START = 0x00A1,
    RD_DDB_AUTO  = 0x00A8,
    SET_LCD_MODE = 0x00B0,
    GET_LCD_MODE = 0x00B1,
    SET_HORI_PERIOD = 0x00B4,
    GET_HORI_PERIOD = 0x00B5,
    SET_VERT_PERIOD = 0x00B6,
    GET_VERT_PERIOD = 0x00B7,
    SET_GPIO_CONF = 0x00B8,
    GET_GPIO_CONF = 0x00B9,
    SET_GPIO_VAL = 0x00BA,
    GET_GPIO_STATUS = 0x00BB,
    SET_POST_PROC = 0x00BC,
    GET_POST_PROC = 0x00BD,
    SET_PWM_CONF = 0x00BE,
    GET_PWM_CONF = 0x00BF,
    SET_LCD_GEN0 = 0x00C0,
    GET_LCD_GEN0 = 0x00C1,
    SET_LCD_GEN1 = 0x00C2,
    GET_LCD_GEN1 = 0x00C3,
    SET_LCD_GEN2 = 0x00C4,
    GET_LCD_GEN2 = 0x00C5,
    SET_LCD_GEN3 = 0x00C6,
    GET_LCD_GEN3 = 0x00C7,
    SET_GPIO0_ROP = 0x00C8,
    GET_GPIO0_ROP = 0x00C9,
    SET_GPIO1_ROP = 0x00CA,
    GET_GPIO1_ROP = 0x00CB,
    SET_GPIO2_ROP = 0x00CC,
    GET_GPIO2_ROP = 0x00CD,
    SET_GPIO3_ROP = 0x00CE,
    GET_GPIO3_ROP = 0x00CF,
    SET_DBC_CONF = 0x00D0,
    GET_DBC_CONF = 0x00D1,
    SET_DBC_TH   = 0x00D4,
    GET_DBC_TH   = 0x00D5,
    SET_PLL      = 0x00E0,
    SET_PLL_MN   = 0x00E2,
    GET_PLL_MN   = 0x00E3,
    GET_PLL_STATUS = 0x00E4,
    SET_DEEP_SLEEP = 0x00E5,
    SET_LSHIFT_FREQ = 0x00E6,
    GET_LSHIFT_FREQ = 0x00E7,
    SET_PIXEL_DATA_IF = 0x00F0,
    GET_PIXEL_DATA_IF = 0x00F1
};

// LCD timing structure
struct LCDTiming {
    uint16_t horizontalTotal;
    uint16_t horizontalDisplay;
    uint16_t horizontalSyncStart;
    uint16_t horizontalSyncWidth;
    uint16_t verticalTotal;
    uint16_t verticalDisplay;
    uint16_t verticalSyncStart;
    uint16_t verticalSyncWidth;
};

class SSD1963_Driver : public Driver, public CommandExecutor {
public:
    explicit SSD1963_Driver(const Configuration& cfg);
    ~SSD1963_Driver() override = default;
    
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
    
    // SSD1963-specific validation
    static bool validateConfig(const Configuration& config) {
        // First check common driver validation
        if (!Driver::validateConfig(config)) {
            return false;
        }
        
        // SSD1963 supports various resolutions up to 864×480
        if (config.geometry.width > 864 || config.geometry.height > 480) {
            return false;
        }
        
        return true;
    }
    
    // SSD1963-specific features
    void setPLL(uint8_t mult, uint8_t div);
    void setLCDMode(uint8_t mode);
    void setPixelClock(uint32_t clock);
    void setLCDTiming(const LCDTiming& timing);
    void setGPIO(uint8_t gpio, bool value);
    void setPWM(uint8_t prescaler, uint8_t compare, uint8_t period);
    void setBacklight(uint8_t level);
    void enableTearingEffect(bool enable, bool vblank = true);

private:
    // Initialization helpers
    void initBus();
    void initDisplay();
    void initPLL();
    void initLCD();
    void initPWM();
    
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
    LCDTiming _timing;
    uint32_t _pixelClock;
};

} // namespace TFT_Runtime

#endif // _SSD1963_DRIVER_H_
