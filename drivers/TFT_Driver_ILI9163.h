#ifndef _TFT_DRIVER_ILI9163_H_
#define _TFT_DRIVER_ILI9163_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// ILI9163 Commands
#define ILI9163_NOP        0x00
#define ILI9163_SWRESET    0x01
#define ILI9163_RDDID      0x04
#define ILI9163_RDDST      0x09
#define ILI9163_RDMODE     0x0A
#define ILI9163_RDMADCTL   0x0B
#define ILI9163_RDPIXFMT   0x0C
#define ILI9163_RDIMGFMT   0x0D
#define ILI9163_RDSELFDIAG 0x0F

#define ILI9163_SLPIN      0x10
#define ILI9163_SLPOUT     0x11
#define ILI9163_PTLON      0x12
#define ILI9163_NORON      0x13

#define ILI9163_INVOFF     0x20
#define ILI9163_INVON      0x21
#define ILI9163_GAMSET     0x26
#define ILI9163_DISPOFF    0x28
#define ILI9163_DISPON     0x29
#define ILI9163_CASET      0x2A
#define ILI9163_PASET      0x2B
#define ILI9163_RAMWR      0x2C
#define ILI9163_RAMRD      0x2E

#define ILI9163_PTLAR      0x30
#define ILI9163_VSCRDEF    0x33
#define ILI9163_TEOFF      0x34
#define ILI9163_TEON       0x35
#define ILI9163_MADCTL     0x36
#define ILI9163_VSCRSADD   0x37
#define ILI9163_IDMOFF     0x38
#define ILI9163_IDMON      0x39
#define ILI9163_COLMOD     0x3A

#define ILI9163_FRMCTR1    0xB1
#define ILI9163_FRMCTR2    0xB2
#define ILI9163_FRMCTR3    0xB3
#define ILI9163_INVCTR     0xB4
#define ILI9163_DISSET5    0xB6

#define ILI9163_PWCTR1     0xC0
#define ILI9163_PWCTR2     0xC1
#define ILI9163_PWCTR3     0xC2
#define ILI9163_PWCTR4     0xC3
#define ILI9163_PWCTR5     0xC4
#define ILI9163_VMCTR1     0xC5
#define ILI9163_VMCTR2     0xC7

#define ILI9163_GMCTRP1    0xE0
#define ILI9163_GMCTRN1    0xE1
#define ILI9163_PWCTR6     0xFC

// Display dimensions - ILI9163 typically 128x128
#define ILI9163_TFTWIDTH   128
#define ILI9163_TFTHEIGHT  128

class TFT_Driver_ILI9163 : public TFT_Driver_Base {
public:
    TFT_Driver_ILI9163(Config& config);
    ~TFT_Driver_ILI9163() override = default;

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

    // ILI9163 specific functions
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
    uint8_t _colstart;  // Offset for partial display
    uint8_t _rowstart;  // Offset for partial display
    
    void initPowerSettings();
    void initDisplaySettings();
    void initGamma();
    void setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
};

} // namespace TFT_Runtime

#endif // _TFT_DRIVER_ILI9163_H_
