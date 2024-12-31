#ifndef _TFT_DRIVER_ST7789V3_H_
#define _TFT_DRIVER_ST7789V3_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// ST7789V3 Commands
#define ST7789V3_NOP       0x00
#define ST7789V3_SWRESET   0x01
#define ST7789V3_RDDID     0x04
#define ST7789V3_RDDST     0x09
#define ST7789V3_RDMODE    0x0A
#define ST7789V3_RDMADCTL  0x0B
#define ST7789V3_RDPIXFMT  0x0C
#define ST7789V3_RDIMGFMT  0x0D
#define ST7789V3_RDSELFDIAG 0x0F

#define ST7789V3_SLPIN     0x10
#define ST7789V3_SLPOUT    0x11
#define ST7789V3_PTLON     0x12
#define ST7789V3_NORON     0x13

#define ST7789V3_INVOFF    0x20
#define ST7789V3_INVON     0x21
#define ST7789V3_GAMSET    0x26
#define ST7789V3_DISPOFF   0x28
#define ST7789V3_DISPON    0x29
#define ST7789V3_CASET     0x2A
#define ST7789V3_RASET     0x2B
#define ST7789V3_RAMWR     0x2C
#define ST7789V3_RAMRD     0x2E

#define ST7789V3_PTLAR     0x30
#define ST7789V3_VSCRDEF   0x33
#define ST7789V3_TEOFF     0x34
#define ST7789V3_TEON      0x35
#define ST7789V3_MADCTL    0x36
#define ST7789V3_VSCRSADD  0x37
#define ST7789V3_IDMOFF    0x38
#define ST7789V3_IDMON     0x39
#define ST7789V3_COLMOD    0x3A
#define ST7789V3_RAMWRC    0x3C
#define ST7789V3_RAMRDC    0x3E

#define ST7789V3_PORCTRL   0xB2
#define ST7789V3_GCTRL     0xB7
#define ST7789V3_VCOMS     0xBB
#define ST7789V3_LCMCTRL   0xC0
#define ST7789V3_VDVVRHEN  0xC2
#define ST7789V3_VRHS      0xC3
#define ST7789V3_VDVS      0xC4
#define ST7789V3_VCMOFSET  0xC5
#define ST7789V3_FRCTRL2   0xC6
#define ST7789V3_PWCTRL1   0xD0
#define ST7789V3_PVGAMCTRL 0xE0
#define ST7789V3_NVGAMCTRL 0xE1
#define ST7789V3_DGMLUTR   0xE2
#define ST7789V3_DGMLUTB   0xE3
#define ST7789V3_GATECTRL  0xE4
#define ST7789V3_SPI2EN    0xE7
#define ST7789V3_PWCTRL2   0xE8
#define ST7789V3_EQCTRL    0xE9
#define ST7789V3_PROMCTRL  0xEC
#define ST7789V3_PROMEN    0xFA
#define ST7789V3_NVMSET    0xFC
#define ST7789V3_PROMACT   0xFE

// Display dimensions - ST7789V3 supports multiple resolutions
#define ST7789V3_TFTWIDTH  240
#define ST7789V3_TFTHEIGHT 320

class TFT_Driver_ST7789V3 : public TFT_Driver_Base {
public:
    TFT_Driver_ST7789V3(Config& config);
    ~TFT_Driver_ST7789V3() override = default;

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

    // ST7789V3 specific functions
    void setVCOM(uint8_t value);
    void setVDV(uint8_t value);
    void setVRHS(uint8_t value);
    void enableSPI2(bool enable);
    void setEqualizer(uint8_t eq1, uint8_t eq2, uint8_t eq3);
    void setGamma(uint8_t gamma);
    void setDigitalGammaLUT(bool enable);
    void setIdleMode(bool enable);
    void setGateControl(uint8_t lines, uint8_t fine);
    void setScrollArea(uint16_t topFixed, uint16_t scrollArea, uint16_t bottomFixed);
    void setScrollStart(uint16_t start);
    void setPartialArea(uint16_t start, uint16_t end);
    void setPowerControl(uint8_t mode);
    void setDisplayControl(uint8_t mode);
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

#endif // _TFT_DRIVER_ST7789V3_H_
