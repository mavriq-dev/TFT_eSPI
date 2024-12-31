#ifndef _TFT_DRIVER_ILI9327_H_
#define _TFT_DRIVER_ILI9327_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// ILI9327 Commands
#define ILI9327_NOP        0x00
#define ILI9327_SWRESET    0x01
#define ILI9327_RDDID      0x04
#define ILI9327_RDDST      0x09
#define ILI9327_RDMODE     0x0A
#define ILI9327_RDMADCTL   0x0B
#define ILI9327_RDPIXFMT   0x0C
#define ILI9327_RDIMGFMT   0x0D
#define ILI9327_RDSELFDIAG 0x0F

#define ILI9327_SLPIN      0x10
#define ILI9327_SLPOUT     0x11
#define ILI9327_PTLON      0x12
#define ILI9327_NORON      0x13

#define ILI9327_INVOFF     0x20
#define ILI9327_INVON      0x21
#define ILI9327_GAMSET     0x26
#define ILI9327_DISPOFF    0x28
#define ILI9327_DISPON     0x29
#define ILI9327_CASET      0x2A
#define ILI9327_PASET      0x2B
#define ILI9327_RAMWR      0x2C
#define ILI9327_RAMRD      0x2E

#define ILI9327_PTLAR      0x30
#define ILI9327_TEOFF      0x34
#define ILI9327_TEON       0x35
#define ILI9327_MADCTL     0x36
#define ILI9327_IDMOFF     0x38
#define ILI9327_IDMON      0x39
#define ILI9327_COLMOD     0x3A

#define ILI9327_SETOSC     0xB0
#define ILI9327_SETPWR1    0xB1
#define ILI9327_SETRGB     0xB3
#define ILI9327_SETCYC     0xB4
#define ILI9327_SETCOM     0xB6
#define ILI9327_SETEXTC    0xB9
#define ILI9327_SETGIP     0xC6
#define ILI9327_SETVCOM    0xBE

#define ILI9327_SETGAMMA   0xC0
#define ILI9327_SETPANEL   0xCC
#define ILI9327_SETPWR2    0xC1
#define ILI9327_SETPWR3    0xC2
#define ILI9327_SETPWR4    0xC3
#define ILI9327_SETPWR5    0xC4

// Display dimensions
#define ILI9327_TFTWIDTH   320
#define ILI9327_TFTHEIGHT  480

class TFT_Driver_ILI9327 : public TFT_Driver_Base {
public:
    TFT_Driver_ILI9327(Config& config);
    ~TFT_Driver_ILI9327() override = default;

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

    // ILI9327 specific functions
    void setOscillator(uint8_t osc);
    void setPowerControl1(uint8_t vrh, uint8_t vc);
    void setPowerControl2(uint8_t bt, uint8_t apo);
    void setPowerControl3(uint8_t dc, uint8_t ap);
    void setPowerControl4(uint8_t gvdd);
    void setPowerControl5(uint8_t vgh, uint8_t vgl);
    void setRGBInterface(uint8_t rgb_if);
    void setCycle(uint8_t cycle);
    void setDisplayControl(uint8_t mode);
    void setVCOMVoltage(uint8_t vcm);
    void setGamma(uint8_t gamma);
    void setPanelCharacteristics(uint8_t panel);
    void setIdleMode(bool enable);
    void setInterface(uint8_t mode);
    void setDisplayMode(uint8_t mode);
    void setTearingEffect(bool enable, bool mode);
    void setPartialArea(uint16_t start, uint16_t end);
    void setPixelFormat(uint8_t format);
    void setMemoryAccessControl(uint8_t mode);
    void setColumnAddress(uint16_t start, uint16_t end);
    void setPageAddress(uint16_t start, uint16_t end);
    void setGammaSet(uint8_t gamma_curve);
    void setExtendedCommands(bool enable);
    void setGIPControl(uint8_t gip);

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

#endif // _TFT_DRIVER_ILI9327_H_
