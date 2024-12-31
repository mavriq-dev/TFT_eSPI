#ifndef _TFT_DRIVER_ST7789V2_H_
#define _TFT_DRIVER_ST7789V2_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// ST7789V2 specific commands
#define ST7789V2_NOP       0x00
#define ST7789V2_SWRESET   0x01
#define ST7789V2_RDDID     0x04
#define ST7789V2_RDDST     0x09

#define ST7789V2_SLPIN     0x10
#define ST7789V2_SLPOUT    0x11
#define ST7789V2_PTLON     0x12
#define ST7789V2_NORON     0x13

#define ST7789V2_INVOFF    0x20
#define ST7789V2_INVON     0x21
#define ST7789V2_DISPOFF   0x28
#define ST7789V2_DISPON    0x29
#define ST7789V2_CASET     0x2A
#define ST7789V2_RASET     0x2B
#define ST7789V2_RAMWR     0x2C
#define ST7789V2_RAMRD     0x2E

#define ST7789V2_PTLAR     0x30
#define ST7789V2_VSCRDEF   0x33
#define ST7789V2_TEOFF     0x34
#define ST7789V2_TEON      0x35
#define ST7789V2_MADCTL    0x36
#define ST7789V2_COLMOD    0x3A
#define ST7789V2_RAMWRC    0x3C
#define ST7789V2_RAMRDC    0x3E

#define ST7789V2_PORCTRL   0xB2
#define ST7789V2_GCTRL     0xB7
#define ST7789V2_VCOMS     0xBB
#define ST7789V2_LCMCTRL   0xC0
#define ST7789V2_VDVVRHEN  0xC2
#define ST7789V2_VRHS      0xC3
#define ST7789V2_VDVS      0xC4
#define ST7789V2_VCMOFSET  0xC5
#define ST7789V2_FRCTRL2   0xC6
#define ST7789V2_PWCTRL1   0xD0
#define ST7789V2_PVGAMCTRL 0xE0
#define ST7789V2_NVGAMCTRL 0xE1

// Enhanced commands in V2
#define ST7789V2_GCTRL2    0xB8
#define ST7789V2_VCMCTRL   0xC7
#define ST7789V2_PWRSEQ    0xD1
#define ST7789V2_RDID1     0xDA
#define ST7789V2_RDID2     0xDB
#define ST7789V2_RDID3     0xDC
#define ST7789V2_CMD2EN    0xDF
#define ST7789V2_PWRCTRL2  0xE8
#define ST7789V2_EQCTRL    0xE9

// Display dimensions - ST7789V2 supports multiple resolutions
#define ST7789V2_TFTWIDTH  240
#define ST7789V2_TFTHEIGHT 320

class TFT_Driver_ST7789V2 : public TFT_Driver_Base {
public:
    TFT_Driver_ST7789V2(Config& config);
    ~TFT_Driver_ST7789V2() override = default;

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

    // ST7789V2 specific functions
    void setVCOM(uint8_t value);
    void setVDV(uint8_t value);
    void setVRHS(uint8_t value);
    void enableCommandSet2(bool enable);
    void setEqualizerControl(uint8_t eq1, uint8_t eq2, uint8_t eq3);
    void setWindowWithOffset(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

private:
    uint16_t _width;
    uint16_t _height;
    uint8_t _rotation;
    bool _invert;
    uint8_t _colstart;  // Offset for partial display
    uint8_t _rowstart;  // Offset for partial display
    
    void initPowerSettings();
    void initDisplaySettings();
    void initGamma();
    void setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
};

} // namespace TFT_Runtime

#endif // _TFT_DRIVER_ST7789V2_H_
