#ifndef _TFT_DRIVER_ST7796_H_
#define _TFT_DRIVER_ST7796_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// ST7796 specific commands
#define ST7796_NOP        0x00
#define ST7796_SWRESET    0x01
#define ST7796_RDDID      0x04
#define ST7796_RDDST      0x09

#define ST7796_SLPIN      0x10
#define ST7796_SLPOUT     0x11
#define ST7796_PTLON      0x12
#define ST7796_NORON      0x13

#define ST7796_RDMODE     0x0A
#define ST7796_RDMADCTL   0x0B
#define ST7796_RDPIXFMT   0x0C
#define ST7796_RDIMGFMT   0x0D
#define ST7796_RDSELFDIAG 0x0F

#define ST7796_INVOFF     0x20
#define ST7796_INVON      0x21
#define ST7796_GAMMASET   0x26
#define ST7796_DISPOFF    0x28
#define ST7796_DISPON     0x29

#define ST7796_CASET      0x2A
#define ST7796_PASET      0x2B
#define ST7796_RAMWR      0x2C
#define ST7796_RAMRD      0x2E

#define ST7796_PTLAR      0x30
#define ST7796_VSCRDEF    0x33
#define ST7796_MADCTL     0x36
#define ST7796_VSCRSADD   0x37
#define ST7796_PIXFMT     0x3A

#define ST7796_WRDISBV    0x51
#define ST7796_RDDISBV    0x52
#define ST7796_WRCTRLD    0x53

#define ST7796_FRMCTR1    0xB1
#define ST7796_FRMCTR2    0xB2
#define ST7796_FRMCTR3    0xB3
#define ST7796_INVCTR     0xB4
#define ST7796_DFUNCTR    0xB6
#define ST7796_ETMOD      0xB7

#define ST7796_PWCTR1     0xC0
#define ST7796_PWCTR2     0xC1
#define ST7796_PWCTR3     0xC2
#define ST7796_PWCTR4     0xC3
#define ST7796_PWCTR5     0xC4
#define ST7796_VMCTR      0xC5
#define ST7796_VMCTR2     0xC7

#define ST7796_PGAMMAC    0xE0
#define ST7796_NGAMMAC    0xE1

// Display dimensions - common for 320x480
#define ST7796_TFTWIDTH   320
#define ST7796_TFTHEIGHT  480

class TFT_Driver_ST7796 : public TFT_Driver_Base {
public:
    TFT_Driver_ST7796(Config& config);
    ~TFT_Driver_ST7796() override = default;

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

#endif // _TFT_DRIVER_ST7796_H_
