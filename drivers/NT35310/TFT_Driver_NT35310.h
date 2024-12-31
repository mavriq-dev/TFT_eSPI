#ifndef _TFT_DRIVER_NT35310_H_
#define _TFT_DRIVER_NT35310_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// NT35310 Commands
#define NT35310_NOP        0x00
#define NT35310_SWRESET    0x01
#define NT35310_RDDID      0x04
#define NT35310_RDDST      0x09
#define NT35310_RDMODE     0x0A
#define NT35310_RDMADCTL   0x0B
#define NT35310_RDPIXFMT   0x0C
#define NT35310_RDIMGFMT   0x0D
#define NT35310_RDSELFDIAG 0x0F

#define NT35310_SLPIN      0x10
#define NT35310_SLPOUT     0x11
#define NT35310_PTLON      0x12
#define NT35310_NORON      0x13

#define NT35310_INVOFF     0x20
#define NT35310_INVON      0x21
#define NT35310_GAMSET     0x26
#define NT35310_DISPOFF    0x28
#define NT35310_DISPON     0x29
#define NT35310_CASET      0x2A
#define NT35310_PASET      0x2B
#define NT35310_RAMWR      0x2C
#define NT35310_RAMRD      0x2E

#define NT35310_PTLAR      0x30
#define NT35310_TEOFF      0x34
#define NT35310_TEON       0x35
#define NT35310_MADCTL     0x36
#define NT35310_IDMOFF     0x38
#define NT35310_IDMON      0x39
#define NT35310_COLMOD     0x3A

#define NT35310_SETOSC     0xB0
#define NT35310_SETPWR1    0xB1
#define NT35310_SETRGB     0xB3
#define NT35310_SETCYC     0xB4
#define NT35310_SETCOM     0xB6
#define NT35310_SETEXTC    0xB9
#define NT35310_SETMIPI    0xBA
#define NT35310_SETVCOM    0xBE

#define NT35310_SETGAMMA   0xC0
#define NT35310_SETPANEL   0xCC
#define NT35310_SETPWR2    0xC1
#define NT35310_SETPWR3    0xC2

// Display dimensions
#define NT35310_TFTWIDTH   320
#define NT35310_TFTHEIGHT  480

class TFT_Driver_NT35310 : public TFT_Driver_Base {
public:
    TFT_Driver_NT35310(Config& config);
    ~TFT_Driver_NT35310() override = default;

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

    // NT35310 specific functions
    void setOscillator(uint8_t osc);
    void setPowerControl1(uint8_t vrh, uint8_t vc);
    void setPowerControl2(uint8_t bt, uint8_t apo);
    void setPowerControl3(uint8_t dc, uint8_t ap);
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
    void setMIPIControl(uint8_t mipi);

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

#endif // _TFT_DRIVER_NT35310_H_
