#ifndef _TFT_DRIVER_GC9A01_H_
#define _TFT_DRIVER_GC9A01_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// GC9A01 specific commands
#define GC9A01_NOP        0x00
#define GC9A01_SWRESET    0x01
#define GC9A01_RDDID      0x04
#define GC9A01_RDDST      0x09

#define GC9A01_SLPIN      0x10
#define GC9A01_SLPOUT     0x11
#define GC9A01_PTLON      0x12
#define GC9A01_NORON      0x13

#define GC9A01_INVOFF     0x20
#define GC9A01_INVON      0x21
#define GC9A01_DISPOFF    0x28
#define GC9A01_DISPON     0x29

#define GC9A01_CASET      0x2A
#define GC9A01_RASET      0x2B
#define GC9A01_RAMWR      0x2C
#define GC9A01_RAMRD      0x2E

#define GC9A01_PTLAR      0x30
#define GC9A01_VSCRDEF    0x33
#define GC9A01_TEOFF      0x34
#define GC9A01_TEON       0x35
#define GC9A01_MADCTL     0x36
#define GC9A01_VSCRSADD   0x37
#define GC9A01_PIXFMT     0x3A

#define GC9A01_RAMCTRL    0xB0
#define GC9A01_RGBCTRL    0xB1
#define GC9A01_PORCTRL    0xB2
#define GC9A01_FRCTRL1    0xB3
#define GC9A01_PARCTRL    0xB5
#define GC9A01_GCTRL      0xB7
#define GC9A01_GTADJ      0xB8
#define GC9A01_DGMEN      0xBA
#define GC9A01_VCOMS      0xBB
#define GC9A01_POWSAVE    0xBC
#define GC9A01_DLPOFFSAVE 0xBD

#define GC9A01_PWCTR1     0xC1
#define GC9A01_PWCTR2     0xC2
#define GC9A01_PWCTR3     0xC3
#define GC9A01_PWCTR4     0xC4
#define GC9A01_PWCTR5     0xC5
#define GC9A01_PWCTR6     0xC6
#define GC9A01_PWCTR7     0xC7

#define GC9A01_PVGAMCTRL  0xE0
#define GC9A01_NVGAMCTRL  0xE1
#define GC9A01_DGMLUTR    0xE2
#define GC9A01_DGMLUTB    0xE3
#define GC9A01_GATECTRL   0xE4
#define GC9A01_SPI2EN     0xE7
#define GC9A01_PWCTR2_2   0xE8
#define GC9A01_EQCTRL     0xE9
#define GC9A01_PROMCTRL   0xEC

// Display dimensions - GC9A01 is typically 240x240 round display
#define GC9A01_TFTWIDTH   240
#define GC9A01_TFTHEIGHT  240

class TFT_Driver_GC9A01 : public TFT_Driver_Base {
public:
    TFT_Driver_GC9A01(Config& config);
    ~TFT_Driver_GC9A01() override = default;

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

#endif // _TFT_DRIVER_GC9A01_H_
