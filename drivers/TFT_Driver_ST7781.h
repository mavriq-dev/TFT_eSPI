#ifndef _TFT_DRIVER_ST7781_H_
#define _TFT_DRIVER_ST7781_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// ST7781 Commands
#define ST7781_NOP         0x00
#define ST7781_SWRESET     0x01
#define ST7781_RDDID       0x04
#define ST7781_RDDST       0x09
#define ST7781_RDMODE      0x0A
#define ST7781_RDMADCTL    0x0B
#define ST7781_RDPIXFMT    0x0C
#define ST7781_RDIMGFMT    0x0D
#define ST7781_RDSELFDIAG  0x0F

#define ST7781_SLPIN       0x10
#define ST7781_SLPOUT      0x11
#define ST7781_PTLON       0x12
#define ST7781_NORON       0x13

#define ST7781_INVOFF      0x20
#define ST7781_INVON       0x21
#define ST7781_GAMSET      0x26
#define ST7781_DISPOFF     0x28
#define ST7781_DISPON      0x29
#define ST7781_CASET       0x2A
#define ST7781_PASET       0x2B
#define ST7781_RAMWR       0x2C
#define ST7781_RAMRD       0x2E

#define ST7781_PTLAR       0x30
#define ST7781_VSCRDEF     0x33
#define ST7781_TEOFF       0x34
#define ST7781_TEON        0x35
#define ST7781_MADCTL      0x36
#define ST7781_VSCRSADD    0x37
#define ST7781_IDMOFF      0x38
#define ST7781_IDMON       0x39
#define ST7781_COLMOD      0x3A

#define ST7781_RDID1       0xDA
#define ST7781_RDID2       0xDB
#define ST7781_RDID3       0xDC

#define ST7781_FRMCTR1     0xB1
#define ST7781_FRMCTR2     0xB2
#define ST7781_FRMCTR3     0xB3
#define ST7781_INVCTR      0xB4
#define ST7781_DISSET5     0xB6

#define ST7781_PWCTR1      0xC0
#define ST7781_PWCTR2      0xC1
#define ST7781_PWCTR3      0xC2
#define ST7781_PWCTR4      0xC3
#define ST7781_PWCTR5      0xC4
#define ST7781_VMCTR1      0xC5
#define ST7781_VMCTR2      0xC7

#define ST7781_GMCTRP1     0xE0
#define ST7781_GMCTRN1     0xE1
#define ST7781_PWCTR6      0xFC

// Display dimensions - ST7781 typically 320x480
#define ST7781_TFTWIDTH    320
#define ST7781_TFTHEIGHT   480

class TFT_Driver_ST7781 : public TFT_Driver_Base {
public:
    TFT_Driver_ST7781(Config& config);
    ~TFT_Driver_ST7781() override = default;

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

    // ST7781 specific functions
    void setFrameRate(uint8_t divider, uint8_t rtna);
    void setDisplayTiming(uint8_t frs, uint8_t btt);
    void setPowerControl(uint8_t vc, uint8_t bt, uint8_t dc);
    void setVCOMVoltage(uint8_t vcm);
    void setGamma(uint8_t gamma);
    void setIdleMode(bool enable);
    void setInterface(uint8_t mode);
    void setDisplayMode(uint8_t mode);
    void setTearingEffect(bool enable, bool mode);
    void setPartialArea(uint16_t start, uint16_t end);
    void setScrollArea(uint16_t topFixed, uint16_t scrollArea, uint16_t bottomFixed);
    void setScrollStart(uint16_t start);
    void setPixelFormat(uint8_t format);
    void setMemoryAccessControl(uint8_t mode);
    void setColumnAddress(uint16_t start, uint16_t end);
    void setPageAddress(uint16_t start, uint16_t end);

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

#endif // _TFT_DRIVER_ST7781_H_
