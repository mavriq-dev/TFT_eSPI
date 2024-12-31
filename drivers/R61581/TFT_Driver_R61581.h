#ifndef _TFT_DRIVER_R61581_H_
#define _TFT_DRIVER_R61581_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// R61581 Commands
#define R61581_NOP         0x00
#define R61581_SWRESET     0x01
#define R61581_RDDID       0x04
#define R61581_RDDST       0x09
#define R61581_RDMODE      0x0A
#define R61581_RDMADCTL    0x0B
#define R61581_RDPIXFMT    0x0C
#define R61581_RDIMGFMT    0x0D
#define R61581_RDSELFDIAG  0x0F

#define R61581_SLPIN       0x10
#define R61581_SLPOUT      0x11
#define R61581_PTLON       0x12
#define R61581_NORON       0x13

#define R61581_INVOFF      0x20
#define R61581_INVON       0x21
#define R61581_DISPOFF     0x28
#define R61581_DISPON      0x29
#define R61581_CASET       0x2A
#define R61581_PASET       0x2B
#define R61581_RAMWR       0x2C
#define R61581_RAMRD       0x2E

#define R61581_PTLAR       0x30
#define R61581_SCRLAR      0x33
#define R61581_TEOFF       0x34
#define R61581_TEON        0x35
#define R61581_MADCTL      0x36
#define R61581_VSCSAD      0x37
#define R61581_IDMOFF      0x38
#define R61581_IDMON       0x39
#define R61581_COLMOD      0x3A

#define R61581_RGBCTRL     0xB0
#define R61581_FRMCTR1     0xB1
#define R61581_FRMCTR2     0xB2
#define R61581_FRMCTR3     0xB3
#define R61581_INVCTR      0xB4
#define R61581_DISCTRL     0xB6

#define R61581_PWCTR1      0xC0
#define R61581_PWCTR2      0xC1
#define R61581_PWCTR3      0xC2
#define R61581_PWCTR4      0xC3
#define R61581_PWCTR5      0xC4
#define R61581_VMCTR1      0xC5
#define R61581_VMCTR2      0xC7

#define R61581_GMCTRP1     0xE0
#define R61581_GMCTRN1     0xE1
#define R61581_GAMSET      0xF2

// Display dimensions - R61581 typically 320x480
#define R61581_TFTWIDTH    320
#define R61581_TFTHEIGHT   480

class TFT_Driver_R61581 : public TFT_Driver_Base {
public:
    TFT_Driver_R61581(Config& config);
    ~TFT_Driver_R61581() override = default;

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

    // R61581 specific functions
    void setFrameRate(uint8_t divider, uint8_t rtna);
    void setRGBInterface(uint8_t mode);
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

#endif // _TFT_DRIVER_R61581_H_
