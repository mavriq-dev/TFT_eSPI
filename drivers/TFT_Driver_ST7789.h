#ifndef _TFT_DRIVER_ST7789_H_
#define _TFT_DRIVER_ST7789_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// ST7789 specific commands
#define ST7789_NOP     0x00
#define ST7789_SWRESET 0x01
#define ST7789_RDDID   0x04
#define ST7789_RDDST   0x09

#define ST7789_SLPIN   0x10
#define ST7789_SLPOUT  0x11
#define ST7789_PTLON   0x12
#define ST7789_NORON   0x13

#define ST7789_INVOFF  0x20
#define ST7789_INVON   0x21
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON  0x29
#define ST7789_CASET   0x2A
#define ST7789_RASET   0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_RAMRD   0x2E

#define ST7789_PTLAR   0x30
#define ST7789_COLMOD  0x3A
#define ST7789_MADCTL  0x36

#define ST7789_PORCTRL 0xB2
#define ST7789_GCTRL   0xB7
#define ST7789_VCOMS   0xBB

#define ST7789_PWCTR1  0xC0
#define ST7789_PWCTR2  0xC1
#define ST7789_PWCTR3  0xC2
#define ST7789_PWCTR4  0xC3
#define ST7789_PWCTR5  0xC4
#define ST7789_VMCTR1  0xC5

#define ST7789_PVGAMCTRL 0xE0
#define ST7789_NVGAMCTRL 0xE1

// Display dimensions - common for 240x240 variant
#define ST7789_TFTWIDTH  240
#define ST7789_TFTHEIGHT 240

class TFT_Driver_ST7789 : public TFT_Driver_Base {
public:
    TFT_Driver_ST7789(Config& config);
    ~TFT_Driver_ST7789() override = default;

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

#endif // _TFT_DRIVER_ST7789_H_
