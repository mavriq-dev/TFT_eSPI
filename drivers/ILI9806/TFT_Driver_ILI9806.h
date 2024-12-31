#ifndef _TFT_DRIVER_ILI9806_H_
#define _TFT_DRIVER_ILI9806_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// ILI9806 Commands
#define ILI9806_NOP        0x00
#define ILI9806_SWRESET    0x01
#define ILI9806_RDDID      0x04
#define ILI9806_RDDST      0x09
#define ILI9806_RDMODE     0x0A
#define ILI9806_RDMADCTL   0x0B
#define ILI9806_RDPIXFMT   0x0C
#define ILI9806_RDIMGFMT   0x0D
#define ILI9806_RDSELFDIAG 0x0F

#define ILI9806_SLPIN      0x10
#define ILI9806_SLPOUT     0x11
#define ILI9806_PTLON      0x12
#define ILI9806_NORON      0x13

#define ILI9806_INVOFF     0x20
#define ILI9806_INVON      0x21
#define ILI9806_GAMSET     0x26
#define ILI9806_DISPOFF    0x28
#define ILI9806_DISPON     0x29
#define ILI9806_CASET      0x2A
#define ILI9806_PASET      0x2B
#define ILI9806_RAMWR      0x2C
#define ILI9806_RAMRD      0x2E

#define ILI9806_PTLAR      0x30
#define ILI9806_TEOFF      0x34
#define ILI9806_TEON       0x35
#define ILI9806_MADCTL     0x36
#define ILI9806_IDMOFF     0x38
#define ILI9806_IDMON      0x39
#define ILI9806_COLMOD     0x3A

#define ILI9806_SETOSC     0xB0
#define ILI9806_SETPOWER   0xB1
#define ILI9806_SETDISP    0xB2
#define ILI9806_SETRGB     0xB3
#define ILI9806_SETCYC     0xB4
#define ILI9806_SETBGP     0xB5
#define ILI9806_SETVCOM    0xB6
#define ILI9806_SETEXTC    0xB9
#define ILI9806_SETMIPI    0xBA
#define ILI9806_SETVDD     0xBC

#define ILI9806_SETGAMMA   0xC0
#define ILI9806_SETPANEL   0xCC
#define ILI9806_SETCABC    0xC9
#define ILI9806_SETCABCMB  0xCA
#define ILI9806_SETOFFSET  0xD2
#define ILI9806_SETCOM     0xD3

// Display dimensions
#define ILI9806_TFTWIDTH   480
#define ILI9806_TFTHEIGHT  800

class TFT_Driver_ILI9806 : public TFT_Driver_Base {
public:
    TFT_Driver_ILI9806(Config& config);
    ~TFT_Driver_ILI9806() override = default;

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

    // ILI9806 specific functions
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
    void setCABC(uint8_t cabc);
    void setCABCMinBrightness(uint8_t min_bright);
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
    void setGammaSet(uint8_t gamma_curve);

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

#endif // _TFT_DRIVER_ILI9806_H_
