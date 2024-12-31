#ifndef _TFT_DRIVER_ILI9486_H_
#define _TFT_DRIVER_ILI9486_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// ILI9486 specific commands
#define ILI9486_NOP        0x00
#define ILI9486_SWRESET    0x01
#define ILI9486_RDDID      0x04
#define ILI9486_RDDST      0x09

#define ILI9486_SLPIN      0x10
#define ILI9486_SLPOUT     0x11
#define ILI9486_PTLON      0x12
#define ILI9486_NORON      0x13

#define ILI9486_RDMODE     0x0A
#define ILI9486_RDMADCTL   0x0B
#define ILI9486_RDPIXFMT   0x0C
#define ILI9486_RDIMGFMT   0x0D
#define ILI9486_RDSELFDIAG 0x0F

#define ILI9486_INVOFF     0x20
#define ILI9486_INVON      0x21
#define ILI9486_GAMMASET   0x26
#define ILI9486_DISPOFF    0x28
#define ILI9486_DISPON     0x29

#define ILI9486_CASET      0x2A
#define ILI9486_PASET      0x2B
#define ILI9486_RAMWR      0x2C
#define ILI9486_RAMRD      0x2E

#define ILI9486_PTLAR      0x30
#define ILI9486_MADCTL     0x36
#define ILI9486_PIXFMT     0x3A

#define ILI9486_FRMCTR1    0xB1
#define ILI9486_FRMCTR2    0xB2
#define ILI9486_FRMCTR3    0xB3
#define ILI9486_INVCTR     0xB4
#define ILI9486_DFUNCTR    0xB6

#define ILI9486_PWCTR1     0xC0
#define ILI9486_PWCTR2     0xC1
#define ILI9486_PWCTR3     0xC2
#define ILI9486_PWCTR4     0xC3
#define ILI9486_PWCTR5     0xC4
#define ILI9486_VMCTR1     0xC5
#define ILI9486_VMCTR2     0xC7

#define ILI9486_PGAMMAC    0xE0
#define ILI9486_NGAMMAC    0xE1

// Display dimensions
#define ILI9486_TFTWIDTH   320
#define ILI9486_TFTHEIGHT  480

class TFT_Driver_ILI9486 : public TFT_Driver_Base {
public:
    TFT_Driver_ILI9486(Config& config);
    ~TFT_Driver_ILI9486() override = default;

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

#endif // _TFT_DRIVER_ILI9486_H_
