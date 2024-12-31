#ifndef _TFT_DRIVER_HX8357B_H_
#define _TFT_DRIVER_HX8357B_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// HX8357B specific commands
#define HX8357B_NOP         0x00
#define HX8357B_SWRESET     0x01
#define HX8357B_RDDID       0x04
#define HX8357B_RDDST       0x09

#define HX8357B_SLPIN       0x10
#define HX8357B_SLPOUT      0x11
#define HX8357B_PTLON       0x12
#define HX8357B_NORON       0x13

#define HX8357B_RDMODE      0x0A
#define HX8357B_RDMADCTL    0x0B
#define HX8357B_RDPIXFMT    0x0C
#define HX8357B_RDIMGFMT    0x0D
#define HX8357B_RDSELFDIAG  0x0F

#define HX8357B_INVOFF      0x20
#define HX8357B_INVON       0x21
#define HX8357B_DISPOFF     0x28
#define HX8357B_DISPON      0x29
#define HX8357B_CASET       0x2A
#define HX8357B_PASET       0x2B
#define HX8357B_RAMWR       0x2C
#define HX8357B_RAMRD       0x2E

#define HX8357B_PTLAR       0x30
#define HX8357B_TEON        0x35
#define HX8357B_MADCTL      0x36
#define HX8357B_COLMOD      0x3A
#define HX8357B_SETOSC      0xB0
#define HX8357B_SETPWR1     0xB1
#define HX8357B_SETRGB      0xB3
#define HX8357B_SETCYC      0xB4
#define HX8357B_SETCOM      0xB6
#define HX8357B_SETEXTC     0xB9
#define HX8357B_SETGAMMA    0xC0
#define HX8357B_SETSTBA     0xC0
#define HX8357B_SETPANEL    0xCC
#define HX8357B_SETPOWER    0xD0
#define HX8357B_SETVCOM     0xD1
#define HX8357B_SETID       0xD3

// Display dimensions - HX8357B supports 480x320
#define HX8357B_TFTWIDTH    480
#define HX8357B_TFTHEIGHT   320

class TFT_Driver_HX8357B : public TFT_Driver_Base {
public:
    TFT_Driver_HX8357B(Config& config);
    ~TFT_Driver_HX8357B() override = default;

    // Implementation of pure virtual functions
    void init() override;
    void writeCommand(uint8_t cmd) override;
    void writeData(uint8_t data) override;
    void writeBlock(uint16_t* data, uint32_t len) override;
    void setRotation(uint8_t rotation) override;
    void invertDisplay(bool invert) override;

    // Display specific parameters
    uint16_t width() const override { return _width; }
    uint16_t height() const override { return _height; }

    // HX8357B specific functions
    void setOscillator(uint8_t osc);
    void setPowerControl(uint8_t mode);
    void setRGBInterface(uint8_t mode);
    void setCycle(uint8_t mode);
    void setDisplayMode(uint8_t mode);
    void setStandbyMode(bool enable);
    void setPanelCharacteristics(uint8_t mode);
    void setVCOM(uint8_t value);
    void enableExtendedCommands(bool enable);

private:
    uint16_t _width;
    uint16_t _height;
    uint8_t _rotation;
    bool _invert;
    
    void initPowerSettings();
    void initDisplaySettings();
    void initGamma();
    void setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
};

} // namespace TFT_Runtime

#endif // _TFT_DRIVER_HX8357B_H_
