#ifndef _TFT_DRIVER_NT35516_H_
#define _TFT_DRIVER_NT35516_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// NT35516 Commands
#define NT35516_NOP        0x00
#define NT35516_SWRESET    0x01
#define NT35516_RDDID      0x04
#define NT35516_RDDST      0x09
#define NT35516_RDMODE     0x0A
#define NT35516_RDMADCTL   0x0B
#define NT35516_RDPIXFMT   0x0C
#define NT35516_RDIMGFMT   0x0D
#define NT35516_RDSELFDIAG 0x0F

#define NT35516_SLPIN      0x10
#define NT35516_SLPOUT     0x11
#define NT35516_PTLON      0x12
#define NT35516_NORON      0x13

#define NT35516_INVOFF     0x20
#define NT35516_INVON      0x21
#define NT35516_DISPOFF    0x28
#define NT35516_DISPON     0x29
#define NT35516_CASET      0x2A
#define NT35516_PASET      0x2B
#define NT35516_RAMWR      0x2C
#define NT35516_RAMRD      0x2E

#define NT35516_PTLAR      0x30
#define NT35516_TEOFF      0x34
#define NT35516_TEON       0x35
#define NT35516_MADCTL     0x36
#define NT35516_IDMOFF     0x38
#define NT35516_IDMON      0x39
#define NT35516_COLMOD     0x3A

#define NT35516_SETOSC     0xB0
#define NT35516_SETPOWER   0xB1
#define NT35516_SETDISP    0xB2
#define NT35516_SETRGB     0xB3
#define NT35516_SETCYC     0xB4
#define NT35516_SETBGP     0xB5
#define NT35516_SETVCOM    0xB6
#define NT35516_SETEXTC    0xB9
#define NT35516_SETMIPI    0xBA
#define NT35516_SETVDD     0xBC

#define NT35516_SETGAMMA   0xC0
#define NT35516_SETPANEL   0xCC
#define NT35516_SETOFFSET  0xD2
#define NT35516_SETCOM     0xD3

// Display dimensions - NT35516 typically 480x800
#define NT35516_TFTWIDTH   480
#define NT35516_TFTHEIGHT  800

class TFT_Driver_NT35516 : public TFT_Driver_Base {
public:
    TFT_Driver_NT35516(Config& config);
    ~TFT_Driver_NT35516() override = default;

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

    // NT35516 specific functions
    void setOscillator(uint8_t osc);
    void setPowerControl(uint8_t vc, uint8_t bt, uint8_t dc);
    void setDisplayTiming(uint8_t rtna, uint8_t fpa, uint8_t bpa);
    void setRGBInterface(uint8_t rgb_if);
    void setCycle(uint8_t cycle);
    void setBacklightControl(uint8_t bl);
    void setVCOMVoltage(uint8_t vcm);
    void setExtendedCommands(bool enable);
    void setMIPIControl(uint8_t mipi);
    void setVDDSetting(uint8_t vdd);
    void setGamma(uint8_t gamma);
    void setPanelCharacteristics(uint8_t panel);
    void setDisplayOffset(uint16_t x, uint16_t y);
    void setCOMControl(uint8_t com);
    void setIdleMode(bool enable);
    void setInterface(uint8_t mode);
    void setDisplayMode(uint8_t mode);
    void setTearingEffect(bool enable, bool mode);
    void setPartialArea(uint16_t start, uint16_t end);
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

#endif // _TFT_DRIVER_NT35516_H_
