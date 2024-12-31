#ifndef _TFT_DRIVER_S6D02A1_H_
#define _TFT_DRIVER_S6D02A1_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// S6D02A1 Commands
#define S6D02A1_NOP        0x00
#define S6D02A1_SWRESET    0x01
#define S6D02A1_RDDID      0x04
#define S6D02A1_RDDST      0x09
#define S6D02A1_RDMODE     0x0A
#define S6D02A1_RDMADCTL   0x0B
#define S6D02A1_RDPIXFMT   0x0C
#define S6D02A1_RDIMGFMT   0x0D
#define S6D02A1_RDSELFDIAG 0x0F

#define S6D02A1_SLPIN      0x10
#define S6D02A1_SLPOUT     0x11
#define S6D02A1_PTLON      0x12
#define S6D02A1_NORON      0x13

#define S6D02A1_INVOFF     0x20
#define S6D02A1_INVON      0x21
#define S6D02A1_GAMSET     0x26
#define S6D02A1_DISPOFF    0x28
#define S6D02A1_DISPON     0x29
#define S6D02A1_CASET      0x2A
#define S6D02A1_PASET      0x2B
#define S6D02A1_RAMWR      0x2C
#define S6D02A1_RAMRD      0x2E

#define S6D02A1_PTLAR      0x30
#define S6D02A1_VSCRDEF    0x33
#define S6D02A1_TEOFF      0x34
#define S6D02A1_TEON       0x35
#define S6D02A1_MADCTL     0x36
#define S6D02A1_VSCRSADD   0x37
#define S6D02A1_IDMOFF     0x38
#define S6D02A1_IDMON      0x39
#define S6D02A1_COLMOD     0x3A

#define S6D02A1_SETOSC     0xB0
#define S6D02A1_SETPWR1    0xB1
#define S6D02A1_SETRGB     0xB3
#define S6D02A1_SETCYC     0xB4
#define S6D02A1_SETCOM     0xB6
#define S6D02A1_SETEXTC    0xB9
#define S6D02A1_SETGAMMA   0xBA

#define S6D02A1_PWCTR1     0xC0
#define S6D02A1_PWCTR2     0xC1
#define S6D02A1_PWCTR3     0xC2
#define S6D02A1_PWCTR4     0xC3
#define S6D02A1_PWCTR5     0xC4
#define S6D02A1_VMCTR1     0xC5
#define S6D02A1_VMCTR2     0xC7

#define S6D02A1_GMCTRP1    0xE0
#define S6D02A1_GMCTRN1    0xE1
#define S6D02A1_GAMRSEL    0xF2

// Display dimensions - S6D02A1 typically 128x160
#define S6D02A1_TFTWIDTH   128
#define S6D02A1_TFTHEIGHT  160

class TFT_Driver_S6D02A1 : public TFT_Driver_Base {
public:
    TFT_Driver_S6D02A1(Config& config);
    ~TFT_Driver_S6D02A1() override = default;

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

    // S6D02A1 specific functions
    void setOscillator(uint8_t osc);
    void setPowerControl(uint8_t vc, uint8_t bt, uint8_t dc);
    void setRGBInterface(uint8_t rgb_if);
    void setCycle(uint8_t cycle);
    void setDisplayControl(uint8_t mode);
    void setExtendedCommands(bool enable);
    void setGamma(uint8_t gamma);
    void setVCOMVoltage(uint8_t vcm);
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

#endif // _TFT_DRIVER_S6D02A1_H_
