#ifndef _TFT_DRIVER_ILI9342_H_
#define _TFT_DRIVER_ILI9342_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// ILI9342 specific commands
#define ILI9342_NOP         0x00
#define ILI9342_SWRESET     0x01
#define ILI9342_RDDID       0x04
#define ILI9342_RDDST       0x09

#define ILI9342_SLPIN       0x10
#define ILI9342_SLPOUT      0x11
#define ILI9342_PTLON       0x12
#define ILI9342_NORON       0x13

#define ILI9342_RDMODE      0x0A
#define ILI9342_RDMADCTL    0x0B
#define ILI9342_RDPIXFMT    0x0C
#define ILI9342_RDIMGFMT    0x0D
#define ILI9342_RDSELFDIAG  0x0F

#define ILI9342_INVOFF      0x20
#define ILI9342_INVON       0x21
#define ILI9342_GAMMASET    0x26
#define ILI9342_DISPOFF     0x28
#define ILI9342_DISPON      0x29

#define ILI9342_CASET       0x2A
#define ILI9342_PASET       0x2B
#define ILI9342_RAMWR       0x2C
#define ILI9342_RAMRD       0x2E

#define ILI9342_PTLAR       0x30
#define ILI9342_VSCRDEF     0x33
#define ILI9342_MADCTL      0x36
#define ILI9342_VSCRSADD    0x37
#define ILI9342_PIXFMT      0x3A

#define ILI9342_FRMCTR1     0xB1
#define ILI9342_FRMCTR2     0xB2
#define ILI9342_FRMCTR3     0xB3
#define ILI9342_INVCTR      0xB4
#define ILI9342_DFUNCTR     0xB6

#define ILI9342_PWCTR1      0xC0
#define ILI9342_PWCTR2      0xC1
#define ILI9342_PWCTR3      0xC2
#define ILI9342_PWCTR4      0xC3
#define ILI9342_PWCTR5      0xC4
#define ILI9342_VMCTR1      0xC5
#define ILI9342_VMCTR2      0xC7

#define ILI9342_RDID1       0xDA
#define ILI9342_RDID2       0xDB
#define ILI9342_RDID3       0xDC
#define ILI9342_RDID4       0xDD

#define ILI9342_GMCTRP1     0xE0
#define ILI9342_GMCTRN1     0xE1

#define ILI9342_IFCTL       0xF6

// Display dimensions - ILI9342 is typically 320x240
#define ILI9342_TFTWIDTH    320
#define ILI9342_TFTHEIGHT   240

class TFT_Driver_ILI9342 : public TFT_Driver_Base {
public:
    TFT_Driver_ILI9342(Config& config);
    ~TFT_Driver_ILI9342() override = default;

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

    // ILI9342 specific functions
    void setInterfaceControl(bool mdt, bool epf);
    void setDisplayFunction(uint8_t div, bool vrh, bool vgl);
    void setGamma(uint8_t gamma);
    void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

private:
    uint16_t _width;
    uint16_t _height;
    uint8_t _rotation;
    bool _invert;
    
    void initPowerSequence();
    void initGamma();
    void initDisplay();
};

} // namespace TFT_Runtime

#endif // _TFT_DRIVER_ILI9342_H_
