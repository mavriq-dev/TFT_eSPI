#ifndef _TFT_DRIVER_ILI9488_H_
#define _TFT_DRIVER_ILI9488_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// ILI9488 specific commands
#define ILI9488_NOP        0x00
#define ILI9488_SWRESET    0x01
#define ILI9488_RDDID      0x04
#define ILI9488_RDDST      0x09

#define ILI9488_SLPIN      0x10
#define ILI9488_SLPOUT     0x11
#define ILI9488_PTLON      0x12
#define ILI9488_NORON      0x13

#define ILI9488_RDMODE     0x0A
#define ILI9488_RDMADCTL   0x0B
#define ILI9488_RDPIXFMT   0x0C
#define ILI9488_RDIMGFMT   0x0D
#define ILI9488_RDSELFDIAG 0x0F

#define ILI9488_INVOFF     0x20
#define ILI9488_INVON      0x21
#define ILI9488_GAMMASET   0x26
#define ILI9488_DISPOFF    0x28
#define ILI9488_DISPON     0x29

#define ILI9488_CASET      0x2A
#define ILI9488_PASET      0x2B
#define ILI9488_RAMWR      0x2C
#define ILI9488_RAMRD      0x2E

#define ILI9488_PTLAR      0x30
#define ILI9488_VSCRDEF    0x33
#define ILI9488_MADCTL     0x36
#define ILI9488_VSCRSADD   0x37
#define ILI9488_PIXFMT     0x3A

#define ILI9488_FRMCTR1    0xB1
#define ILI9488_FRMCTR2    0xB2
#define ILI9488_FRMCTR3    0xB3
#define ILI9488_INVCTR     0xB4
#define ILI9488_DFUNCTR    0xB6
#define ILI9488_ENTRYMODE  0xB7

#define ILI9488_PWCTR1     0xC0
#define ILI9488_PWCTR2     0xC1
#define ILI9488_PWCTR3     0xC2
#define ILI9488_PWCTR4     0xC3
#define ILI9488_PWCTR5     0xC4
#define ILI9488_VMCTR1     0xC5
#define ILI9488_VMCTR2     0xC7

#define ILI9488_PGAMMAC    0xE0
#define ILI9488_NGAMMAC    0xE1
#define ILI9488_IMGFUNCT   0xE9

// Display dimensions
#define ILI9488_TFTWIDTH   320
#define ILI9488_TFTHEIGHT  480

class TFT_Driver_ILI9488 : public TFT_Driver_Base {
public:
    TFT_Driver_ILI9488(Config& config);
    ~TFT_Driver_ILI9488() override = default;

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

private:
    uint16_t _width;
    uint16_t _height;
    uint8_t _rotation;
    bool _invert;
};

} // namespace TFT_Runtime

#endif // _TFT_DRIVER_ILI9488_H_
