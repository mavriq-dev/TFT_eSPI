#ifndef _TFT_DRIVER_NT35510_H_
#define _TFT_DRIVER_NT35510_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// NT35510 specific commands
#define NT35510_NOP         0x00
#define NT35510_SWRESET     0x01
#define NT35510_RDDID       0x04
#define NT35510_RDDST       0x09

#define NT35510_SLPIN       0x10
#define NT35510_SLPOUT      0x11
#define NT35510_PTLON       0x12
#define NT35510_NORON       0x13

#define NT35510_RDMODE      0x0A
#define NT35510_RDMADCTL    0x0B
#define NT35510_RDPIXFMT    0x0C
#define NT35510_RDIMGFMT    0x0D
#define NT35510_RDSELFDIAG  0x0F

#define NT35510_INVOFF      0x20
#define NT35510_INVON       0x21
#define NT35510_DISPOFF     0x28
#define NT35510_DISPON      0x29
#define NT35510_CASET       0x2A
#define NT35510_PASET       0x2B
#define NT35510_RAMWR       0x2C
#define NT35510_RAMRD       0x2E

#define NT35510_PTLAR       0x30
#define NT35510_MADCTL      0x36
#define NT35510_COLMOD      0x3A
#define NT35510_SETPWD      0xB1
#define NT35510_SETDISPLAY  0xB2
#define NT35510_SETRGB      0xB3
#define NT35510_SETCYC      0xB4
#define NT35510_SETBGP      0xB5
#define NT35510_SETVCOM     0xB6
#define NT35510_SETEXTC     0xB9
#define NT35510_SETMIPI     0xBA

#define NT35510_SETGAMMA    0xC1
#define NT35510_SETPOWER    0xD0
#define NT35510_SETVCOM1    0xD1
#define NT35510_SETVCOM2    0xD2
#define NT35510_SETPANEL    0xD3
#define NT35510_SETCABC     0xD4
#define NT35510_SETCABCMB   0xD5

// Display dimensions - NT35510 supports up to 480x800
#define NT35510_TFTWIDTH    480
#define NT35510_TFTHEIGHT   800

class TFT_Driver_NT35510 : public TFT_Driver_Base {
public:
    TFT_Driver_NT35510(Config& config);
    ~TFT_Driver_NT35510() override = default;

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

    // NT35510 specific functions
    void setVCOM(uint8_t value);
    void setPanelSettings(uint8_t mode);
    void setGamma(uint8_t gamma);
    void setCABC(uint8_t mode);
    void setMIPIControl(bool enable);
    void setDisplayMode(uint8_t mode);
    void setBacklightControl(uint8_t level);

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

#endif // _TFT_DRIVER_NT35510_H_
