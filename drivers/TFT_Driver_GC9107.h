#ifndef _TFT_DRIVER_GC9107_H_
#define _TFT_DRIVER_GC9107_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// GC9107 Commands
#define GC9107_NOP       0x00
#define GC9107_SWRESET   0x01
#define GC9107_RDDID     0x04
#define GC9107_RDDST     0x09
#define GC9107_RDMODE    0x0A
#define GC9107_RDMADCTL  0x0B
#define GC9107_RDPIXFMT  0x0C
#define GC9107_RDIMGFMT  0x0D
#define GC9107_RDSELFDIAG 0x0F

#define GC9107_SLPIN     0x10
#define GC9107_SLPOUT    0x11
#define GC9107_PTLON     0x12
#define GC9107_NORON     0x13

#define GC9107_INVOFF    0x20
#define GC9107_INVON     0x21
#define GC9107_GAMSET    0x26
#define GC9107_DISPOFF   0x28
#define GC9107_DISPON    0x29
#define GC9107_CASET     0x2A
#define GC9107_RASET     0x2B
#define GC9107_RAMWR     0x2C
#define GC9107_RAMRD     0x2E

#define GC9107_PTLAR     0x30
#define GC9107_VSCRDEF   0x33
#define GC9107_TEOFF     0x34
#define GC9107_TEON      0x35
#define GC9107_MADCTL    0x36
#define GC9107_VSCRSADD  0x37
#define GC9107_IDMOFF    0x38
#define GC9107_IDMON     0x39
#define GC9107_COLMOD    0x3A
#define GC9107_RAMWRC    0x3C
#define GC9107_RAMRDC    0x3E

#define GC9107_SETRGB    0xB1
#define GC9107_PORCTRL   0xB2
#define GC9107_FRCTRL1   0xB3
#define GC9107_PARCTRL   0xB5
#define GC9107_GCTRL     0xB7
#define GC9107_GTADJ     0xB8
#define GC9107_DGMEN     0xBA
#define GC9107_VCOMS     0xBB
#define GC9107_LCMCTRL   0xC0
#define GC9107_IDSET     0xC1
#define GC9107_VDVVRHEN  0xC2
#define GC9107_VRHS      0xC3
#define GC9107_VDVS      0xC4
#define GC9107_VCMOFSET  0xC5
#define GC9107_FRCTRL2   0xC6
#define GC9107_CABCCTRL  0xC7
#define GC9107_REGSEL1   0xC8
#define GC9107_REGSEL2   0xCA
#define GC9107_PWMFRSEL  0xCC
#define GC9107_PWCTRL1   0xD0
#define GC9107_VAPVANEN  0xD2
#define GC9107_CMD2EN    0xDF
#define GC9107_PVGAMCTRL 0xE0
#define GC9107_NVGAMCTRL 0xE1
#define GC9107_DGMLUTR   0xE2
#define GC9107_DGMLUTB   0xE3
#define GC9107_GATECTRL  0xE4
#define GC9107_SPI2EN    0xE7
#define GC9107_PWCTRL2   0xE8
#define GC9107_EQCTRL    0xE9
#define GC9107_PROMCTRL  0xEC
#define GC9107_PROMEN    0xFA
#define GC9107_NVMSET    0xFC
#define GC9107_PROMACT   0xFE

// Display dimensions
#define GC9107_TFTWIDTH  128
#define GC9107_TFTHEIGHT 128

class TFT_Driver_GC9107 : public TFT_Driver_Base {
public:
    TFT_Driver_GC9107(Config& config);
    ~TFT_Driver_GC9107() override = default;

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

    // GC9107 specific functions
    void setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
    void setScrollArea(uint16_t topFixed, uint16_t scrollArea, uint16_t bottomFixed);
    void setScrollStart(uint16_t start);
    void setIdleMode(bool enable);
    void setPowerControl(uint8_t mode);
    void setVCOM(uint8_t value);
    void setMemoryAccessControl(uint8_t mode);
    void setPixelFormat(uint8_t format);
    void setGamma(uint8_t gamma);
    void setPositiveGamma(const uint8_t* gamma);
    void setNegativeGamma(const uint8_t* gamma);
    void setFrameRate(uint8_t divider, uint8_t rtna);
    void setPartialArea(uint16_t start, uint16_t end);
    void setTearingEffect(bool enable, bool mode);
    void enablePowerSave(bool enable);
    uint32_t readID();

private:
    uint16_t _width;
    uint16_t _height;
    uint8_t _rotation;
    bool _invert;

    void initPowerSettings();
    void initDisplaySettings();
    void initGamma();
    void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
};

} // namespace TFT_Runtime

#endif // _TFT_DRIVER_GC9107_H_
