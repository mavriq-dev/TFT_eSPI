#ifndef _TFT_DRIVER_ILI9481_H_
#define _TFT_DRIVER_ILI9481_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// ILI9481 specific commands
#define ILI9481_NOP           0x00
#define ILI9481_SWRESET       0x01
#define ILI9481_RDDID         0x04
#define ILI9481_RDDST         0x09

#define ILI9481_SLPIN         0x10
#define ILI9481_SLPOUT        0x11
#define ILI9481_PTLON         0x12
#define ILI9481_NORON         0x13

#define ILI9481_RDMODE        0x0A
#define ILI9481_RDMADCTL      0x0B
#define ILI9481_RDPIXFMT      0x0C
#define ILI9481_RDIMGFMT      0x0D
#define ILI9481_RDSELFDIAG    0x0F

#define ILI9481_INVOFF        0x20
#define ILI9481_INVON         0x21
#define ILI9481_GAMMASET      0x26
#define ILI9481_DISPOFF       0x28
#define ILI9481_DISPON        0x29

#define ILI9481_CASET         0x2A
#define ILI9481_PASET         0x2B
#define ILI9481_RAMWR         0x2C
#define ILI9481_RAMRD         0x2E

#define ILI9481_PTLAR         0x30
#define ILI9481_VSCRDEF       0x33
#define ILI9481_MADCTL        0x36
#define ILI9481_VSCRSADD      0x37
#define ILI9481_PIXFMT        0x3A

#define ILI9481_RGBCTRL       0xB0
#define ILI9481_FRMCTR1       0xB1
#define ILI9481_FRMCTR2       0xB2
#define ILI9481_FRMCTR3       0xB3
#define ILI9481_INVCTR        0xB4
#define ILI9481_DFUNCTR       0xB6

#define ILI9481_PWCTR1        0xC0
#define ILI9481_PWCTR2        0xC1
#define ILI9481_PWCTR3        0xC2
#define ILI9481_PWCTR4        0xC3
#define ILI9481_PWCTR5        0xC4
#define ILI9481_VMCTR1        0xC5
#define ILI9481_VMCTR2        0xC7

#define ILI9481_GMCTRP1       0xE0
#define ILI9481_GMCTRN1       0xE1
#define ILI9481_PWCTR6        0xFC

// Display dimensions - ILI9481 is typically 320x480 or 480x320
#define ILI9481_TFTWIDTH      320
#define ILI9481_TFTHEIGHT     480

class TFT_Driver_ILI9481 : public TFT_Driver_Base {
public:
    TFT_Driver_ILI9481(Config& config);
    ~TFT_Driver_ILI9481() override = default;

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

    // ILI9481 specific functions
    void setGamma(uint8_t gamma);
    void setVCOM(uint8_t value);
    void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

private:
    uint16_t _width;
    uint16_t _height;
    uint8_t _rotation;
    bool _invert;
    
    void initGamma();
    void initPowerSequence();
};

} // namespace TFT_Runtime

#endif // _TFT_DRIVER_ILI9481_H_
