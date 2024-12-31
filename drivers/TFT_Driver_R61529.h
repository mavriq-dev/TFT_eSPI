#ifndef _TFT_DRIVER_R61529_H_
#define _TFT_DRIVER_R61529_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// R61529 specific commands
#define R61529_NOP        0x00
#define R61529_SWRESET    0x01
#define R61529_RDDID      0x04
#define R61529_RDDST      0x09

#define R61529_SLPIN      0x10
#define R61529_SLPOUT     0x11
#define R61529_PTLON      0x12
#define R61529_NORON      0x13

#define R61529_RDMODE     0x0A
#define R61529_RDMADCTL   0x0B
#define R61529_RDPIXFMT   0x0C
#define R61529_RDIMGFMT   0x0D
#define R61529_RDSELFDIAG 0x0F

#define R61529_INVOFF     0x20
#define R61529_INVON      0x21
#define R61529_DISPOFF    0x28
#define R61529_DISPON     0x29

#define R61529_CASET      0x2A
#define R61529_PASET      0x2B
#define R61529_RAMWR      0x2C
#define R61529_RAMRD      0x2E

#define R61529_PTLAR      0x30
#define R61529_VSCRDEF    0x33
#define R61529_MADCTL     0x36
#define R61529_VSCRSADD   0x37
#define R61529_PIXFMT     0x3A

#define R61529_WRDISBV    0x51
#define R61529_RDDISBV    0x52
#define R61529_WRCTRLD    0x53
#define R61529_RDCTRLD    0x54
#define R61529_WRCABC     0x55
#define R61529_RDCABC     0x56

#define R61529_GAMMA1     0xE0
#define R61529_GAMMA2     0xE1
#define R61529_GAMMA3     0xE2
#define R61529_GAMMA4     0xE3

#define R61529_INTERFACE  0xB0
#define R61529_FRAMERATE  0xB1
#define R61529_DPIPLL     0xB3
#define R61529_EQTIMING   0xB4
#define R61529_PWRCTRL1   0xC0
#define R61529_PWRCTRL2   0xC1
#define R61529_VMCTRL1    0xC5
#define R61529_VMCTRL2    0xC7

// Display dimensions - R61529 is typically 320x480
#define R61529_TFTWIDTH   320
#define R61529_TFTHEIGHT  480

class TFT_Driver_R61529 : public TFT_Driver_Base {
public:
    TFT_Driver_R61529(Config& config);
    ~TFT_Driver_R61529() override = default;

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

#endif // _TFT_DRIVER_R61529_H_
