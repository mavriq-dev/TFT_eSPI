#ifndef _TFT_DRIVER_RM68140_H_
#define _TFT_DRIVER_RM68140_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// RM68140 specific commands
#define RM68140_NOP        0x00
#define RM68140_SWRESET    0x01
#define RM68140_RDDID      0x04
#define RM68140_RDDST      0x09

#define RM68140_SLPIN      0x10
#define RM68140_SLPOUT     0x11
#define RM68140_PTLON      0x12
#define RM68140_NORON      0x13

#define RM68140_RDMODE     0x0A
#define RM68140_RDMADCTL   0x0B
#define RM68140_RDPIXFMT   0x0C
#define RM68140_RDIMGFMT   0x0D
#define RM68140_RDSELFDIAG 0x0F

#define RM68140_INVOFF     0x20
#define RM68140_INVON      0x21
#define RM68140_DISPOFF    0x28
#define RM68140_DISPON     0x29

#define RM68140_CASET      0x2A
#define RM68140_PASET      0x2B
#define RM68140_RAMWR      0x2C
#define RM68140_RAMRD      0x2E

#define RM68140_PTLAR      0x30
#define RM68140_VSCRDEF    0x33
#define RM68140_MADCTL     0x36
#define RM68140_VSCRSADD   0x37
#define RM68140_PIXFMT     0x3A

#define RM68140_FRMCTR1    0xB1
#define RM68140_FRMCTR2    0xB2
#define RM68140_FRMCTR3    0xB3
#define RM68140_INVCTR     0xB4
#define RM68140_DFUNCTR    0xB6

#define RM68140_PWCTR1     0xC0
#define RM68140_PWCTR2     0xC1
#define RM68140_PWCTR3     0xC2
#define RM68140_PWCTR4     0xC3
#define RM68140_PWCTR5     0xC4
#define RM68140_VMCTR1     0xC5
#define RM68140_VMCTR2     0xC7

#define RM68140_GMCTRP1    0xE0
#define RM68140_GMCTRN1    0xE1
#define RM68140_GAMSET     0xF2

// Display dimensions
#define RM68140_TFTWIDTH   320
#define RM68140_TFTHEIGHT  480

class TFT_Driver_RM68140 : public TFT_Driver_Base {
public:
    TFT_Driver_RM68140(Config& config);
    ~TFT_Driver_RM68140() override = default;

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

#endif // _TFT_DRIVER_RM68140_H_
