#ifndef _TFT_DRIVER_RM68120_H_
#define _TFT_DRIVER_RM68120_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// RM68120 Commands
#define RM68120_NOP        0x00
#define RM68120_SWRESET    0x01
#define RM68120_RDDID      0x04
#define RM68120_RDDST      0x09
#define RM68120_RDMODE     0x0A
#define RM68120_RDMADCTL   0x0B
#define RM68120_RDPIXFMT   0x0C
#define RM68120_RDIMGFMT   0x0D
#define RM68120_RDSELFDIAG 0x0F

#define RM68120_SLPIN      0x10
#define RM68120_SLPOUT     0x11
#define RM68120_PTLON      0x12
#define RM68120_NORON      0x13

#define RM68120_INVOFF     0x20
#define RM68120_INVON      0x21
#define RM68120_ALLPOFF    0x22
#define RM68120_ALLPON     0x23
#define RM68120_DISPOFF    0x28
#define RM68120_DISPON     0x29
#define RM68120_CASET      0x2A
#define RM68120_RASET      0x2B
#define RM68120_RAMWR      0x2C
#define RM68120_RAMRD      0x2E

#define RM68120_PTLAR      0x30
#define RM68120_TEOFF      0x34
#define RM68120_TEON       0x35
#define RM68120_MADCTL     0x36
#define RM68120_IDMOFF     0x38
#define RM68120_IDMON      0x39
#define RM68120_COLMOD     0x3A

#define RM68120_FRMCTR1    0xB1
#define RM68120_FRMCTR2    0xB2
#define RM68120_FRMCTR3    0xB3
#define RM68120_INVCTR     0xB4
#define RM68120_DISSET5    0xB6

#define RM68120_PWCTR1     0xC0
#define RM68120_PWCTR2     0xC1
#define RM68120_PWCTR3     0xC2
#define RM68120_PWCTR4     0xC3
#define RM68120_PWCTR5     0xC4
#define RM68120_VMCTR1     0xC5
#define RM68120_VMCTR2     0xC7

#define RM68120_GMCTRP1    0xE0
#define RM68120_GMCTRN1    0xE1
#define RM68120_GAMSET     0xF2

// Display dimensions - RM68120 typically 480x800
#define RM68120_TFTWIDTH   480
#define RM68120_TFTHEIGHT  800

class TFT_Driver_RM68120 : public TFT_Driver_Base {
public:
    TFT_Driver_RM68120(Config& config);
    ~TFT_Driver_RM68120() override = default;

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

    // RM68120 specific functions
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
    void setPixelFormat(uint8_t format);
    void setMemoryAccessControl(uint8_t mode);
    void setColumnAddress(uint16_t start, uint16_t end);
    void setRowAddress(uint16_t start, uint16_t end);

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

#endif // _TFT_DRIVER_RM68120_H_
