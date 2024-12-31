#ifndef _TFT_DRIVER_ILI9881C_H_
#define _TFT_DRIVER_ILI9881C_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// ILI9881C Commands
#define ILI9881C_NOP         0x00
#define ILI9881C_SWRESET     0x01
#define ILI9881C_RDDID       0x04
#define ILI9881C_RDDST       0x09
#define ILI9881C_RDMODE      0x0A
#define ILI9881C_RDMADCTL    0x0B
#define ILI9881C_RDPIXFMT    0x0C
#define ILI9881C_RDIMGFMT    0x0D
#define ILI9881C_RDSELFDIAG  0x0F

#define ILI9881C_SLPIN       0x10
#define ILI9881C_SLPOUT      0x11
#define ILI9881C_PTLON       0x12
#define ILI9881C_NORON       0x13

#define ILI9881C_INVOFF      0x20
#define ILI9881C_INVON       0x21
#define ILI9881C_GAMSET      0x26
#define ILI9881C_DISPOFF     0x28
#define ILI9881C_DISPON      0x29
#define ILI9881C_CASET       0x2A
#define ILI9881C_PASET       0x2B
#define ILI9881C_RAMWR       0x2C
#define ILI9881C_RAMRD       0x2E

#define ILI9881C_PTLAR       0x30
#define ILI9881C_TEOFF       0x34
#define ILI9881C_TEON        0x35
#define ILI9881C_MADCTL      0x36
#define ILI9881C_IDMOFF      0x38
#define ILI9881C_IDMON       0x39
#define ILI9881C_COLMOD      0x3A

#define ILI9881C_SETIMAGE    0xB0
#define ILI9881C_SETPANEL    0xB1
#define ILI9881C_SETPOWER    0xB2
#define ILI9881C_SETRGB      0xB3
#define ILI9881C_SETCYC      0xB4
#define ILI9881C_SETVCOM     0xB6
#define ILI9881C_SETEXTC     0xB9
#define ILI9881C_SETMIPI     0xBA
#define ILI9881C_SETOTP      0xBB
#define ILI9881C_SETPOWER2   0xBC
#define ILI9881C_SETPOWER3   0xBD
#define ILI9881C_SETDGC      0xBE
#define ILI9881C_SETVCOM2    0xBF

#define ILI9881C_SETGIP1     0xD0
#define ILI9881C_SETGIP2     0xD1
#define ILI9881C_SETGIP3     0xD2
#define ILI9881C_SETGIP4     0xD3
#define ILI9881C_SETGIP5     0xD4
#define ILI9881C_SETGIP6     0xD5
#define ILI9881C_SETGIP7     0xD6

// Display dimensions
#define ILI9881C_TFTWIDTH    720
#define ILI9881C_TFTHEIGHT   1280

class TFT_Driver_ILI9881C : public TFT_Driver_Base {
public:
    TFT_Driver_ILI9881C(Config& config);
    ~TFT_Driver_ILI9881C() override = default;

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

    // ILI9881C specific functions
    void setImageFunction(uint8_t mode);
    void setPanelCharacteristics(uint8_t panel);
    void setPowerControl(uint8_t power);
    void setRGBInterface(uint8_t rgb_if);
    void setCycle(uint8_t cycle);
    void setVCOM(uint8_t vcom);
    void setExtendedCommands(bool enable);
    void setMIPIControl(uint8_t mipi);
    void setOTPControl(uint8_t otp);
    void setPowerControl2(uint8_t power);
    void setPowerControl3(uint8_t power);
    void setDGC(uint8_t dgc);
    void setVCOM2(uint8_t vcom);
    void setGIP1(uint8_t gip);
    void setGIP2(uint8_t gip);
    void setGIP3(uint8_t gip);
    void setGIP4(uint8_t gip);
    void setGIP5(uint8_t gip);
    void setGIP6(uint8_t gip);
    void setGIP7(uint8_t gip);
    void setGamma(uint8_t gamma);
    void setMemoryAccessControl(uint8_t mode);
    void setPixelFormat(uint8_t format);
    void setColumnAddress(uint16_t start, uint16_t end);
    void setPageAddress(uint16_t start, uint16_t end);
    void setPartialArea(uint16_t start, uint16_t end);
    void setTearingEffect(bool enable, bool mode);
    void setIdleMode(bool enable);

private:
    uint16_t _width;
    uint16_t _height;
    uint8_t _rotation;
    bool _invert;
    
    void initPowerSettings();
    void initDisplaySettings();
    void initGamma();
    void initGIP();
    void setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
};

} // namespace TFT_Runtime

#endif // _TFT_DRIVER_ILI9881C_H_
