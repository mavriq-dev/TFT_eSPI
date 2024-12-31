#ifndef _TFT_DRIVER_ST7796S_H_
#define _TFT_DRIVER_ST7796S_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// ST7796S Commands
#define ST7796S_NOP       0x00
#define ST7796S_SWRESET   0x01
#define ST7796S_RDDID     0x04
#define ST7796S_RDDST     0x09
#define ST7796S_RDMODE    0x0A
#define ST7796S_RDMADCTL  0x0B
#define ST7796S_RDPIXFMT  0x0C
#define ST7796S_RDIMGFMT  0x0D
#define ST7796S_RDSELFDIAG 0x0F

#define ST7796S_SLPIN     0x10
#define ST7796S_SLPOUT    0x11
#define ST7796S_PTLON     0x12
#define ST7796S_NORON     0x13

#define ST7796S_INVOFF    0x20
#define ST7796S_INVON     0x21
#define ST7796S_GAMSET    0x26
#define ST7796S_DISPOFF   0x28
#define ST7796S_DISPON    0x29
#define ST7796S_CASET     0x2A
#define ST7796S_PASET     0x2B
#define ST7796S_RAMWR     0x2C
#define ST7796S_RAMRD     0x2E

#define ST7796S_PTLAR     0x30
#define ST7796S_VSCRDEF   0x33
#define ST7796S_TEOFF     0x34
#define ST7796S_TEON      0x35
#define ST7796S_MADCTL    0x36
#define ST7796S_VSCRSADD  0x37
#define ST7796S_IDMOFF    0x38
#define ST7796S_IDMON     0x39
#define ST7796S_COLMOD    0x3A
#define ST7796S_RAMWRC    0x3C
#define ST7796S_RAMRDC    0x3E

#define ST7796S_SETIMAGE  0xB0
#define ST7796S_FRMCTR1   0xB1
#define ST7796S_FRMCTR2   0xB2
#define ST7796S_FRMCTR3   0xB3
#define ST7796S_INVCTR    0xB4
#define ST7796S_DFUNCTR   0xB6
#define ST7796S_PWCTR1    0xC0
#define ST7796S_PWCTR2    0xC1
#define ST7796S_PWCTR3    0xC2
#define ST7796S_PWCTR4    0xC3
#define ST7796S_PWCTR5    0xC4
#define ST7796S_VMCTR     0xC5
#define ST7796S_GMCTRP1   0xE0
#define ST7796S_GMCTRN1   0xE1
#define ST7796S_DOCA      0xE8
#define ST7796S_CSCON     0xF0

// Display dimensions
#define ST7796S_TFTWIDTH  320
#define ST7796S_TFTHEIGHT 480

class TFT_Driver_ST7796S : public TFT_Driver_Base {
public:
    TFT_Driver_ST7796S(Config& config);
    ~TFT_Driver_ST7796S() override = default;

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

    // ST7796S specific functions
    void setFrameRate(uint8_t divider, uint8_t rtna);
    void setImageFormat(uint8_t format);
    void setInversion(uint8_t mode);
    void setDisplayFunction(uint8_t mode);
    void setPowerControl1(uint8_t vrh);
    void setPowerControl2(uint8_t bt);
    void setPowerControl3(uint8_t mode);
    void setPowerControl4(uint8_t mode);
    void setPowerControl5(uint8_t mode);
    void setVCOMControl(uint8_t vcm);
    void setMemoryAccessControl(uint8_t mode);
    void setPixelFormat(uint8_t format);
    void setColumnAddress(uint16_t start, uint16_t end);
    void setPageAddress(uint16_t start, uint16_t end);
    void setScrollArea(uint16_t tfa, uint16_t vsa, uint16_t bfa);
    void setScrollStart(uint16_t start);
    void setTearingEffect(bool enable, bool mode);
    void setPartialArea(uint16_t start, uint16_t end);
    void setIdleMode(bool enable);
    void setColorEnhancement(uint8_t mode);
    void setCommandAccess(uint8_t mode);

private:
    uint16_t _width;
    uint16_t _height;
    uint8_t _rotation;
    bool _invert;
    
    void initPowerSettings();
    void initDisplaySettings();
    void initGamma();
    void setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
};

} // namespace TFT_Runtime

#endif // _TFT_DRIVER_ST7796S_H_
