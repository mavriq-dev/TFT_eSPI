#ifndef _TFT_DRIVER_ST7735R_H_
#define _TFT_DRIVER_ST7735R_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// ST7735R specific commands
#define ST7735R_NOP       0x00
#define ST7735R_SWRESET   0x01
#define ST7735R_RDDID     0x04
#define ST7735R_RDDST     0x09

#define ST7735R_SLPIN     0x10
#define ST7735R_SLPOUT    0x11
#define ST7735R_PTLON     0x12
#define ST7735R_NORON     0x13

#define ST7735R_INVOFF    0x20
#define ST7735R_INVON     0x21
#define ST7735R_DISPOFF   0x28
#define ST7735R_DISPON    0x29
#define ST7735R_CASET     0x2A
#define ST7735R_RASET     0x2B
#define ST7735R_RAMWR     0x2C
#define ST7735R_RAMRD     0x2E

#define ST7735R_PTLAR     0x30
#define ST7735R_COLMOD    0x3A
#define ST7735R_MADCTL    0x36

#define ST7735R_FRMCTR1   0xB1
#define ST7735R_FRMCTR2   0xB2
#define ST7735R_FRMCTR3   0xB3
#define ST7735R_INVCTR    0xB4
#define ST7735R_DISSET5   0xB6

#define ST7735R_PWCTR1    0xC0
#define ST7735R_PWCTR2    0xC1
#define ST7735R_PWCTR3    0xC2
#define ST7735R_PWCTR4    0xC3
#define ST7735R_PWCTR5    0xC4
#define ST7735R_VMCTR1    0xC5

#define ST7735R_RDID1     0xDA
#define ST7735R_RDID2     0xDB
#define ST7735R_RDID3     0xDC
#define ST7735R_RDID4     0xDD

#define ST7735R_PWCTR6    0xFC

#define ST7735R_GMCTRP1   0xE0
#define ST7735R_GMCTRN1   0xE1

// Display dimensions - ST7735R is typically 128x160
#define ST7735R_TFTWIDTH  128
#define ST7735R_TFTHEIGHT 160

// Color definitions for default init
#define ST7735R_INITR_GREENTAB   0x0
#define ST7735R_INITR_REDTAB     0x1
#define ST7735R_INITR_BLACKTAB   0x2
#define ST7735R_INITR_144GREENTAB 0x3

class TFT_Driver_ST7735R : public TFT_Driver_Base {
public:
    TFT_Driver_ST7735R(Config& config);
    ~TFT_Driver_ST7735R() override = default;

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

    // ST7735R specific functions
    void setTabColor(uint8_t option);
    void setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

private:
    uint16_t _width;
    uint16_t _height;
    uint8_t _rotation;
    bool _invert;
    uint8_t _tabcolor;

    void commonInit();
    void initR(uint8_t options);
    void initRGreenTab();
    void initRRedTab();
    void initRBlackTab();
    void initR144();
    void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
};

} // namespace TFT_Runtime

#endif // _TFT_DRIVER_ST7735R_H_
