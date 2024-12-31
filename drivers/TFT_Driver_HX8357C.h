#ifndef _TFT_DRIVER_HX8357C_H_
#define _TFT_DRIVER_HX8357C_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// HX8357C Commands
#define HX8357C_NOP         0x00
#define HX8357C_SWRESET     0x01
#define HX8357C_RDDID       0x04
#define HX8357C_RDDST       0x09
#define HX8357C_RDPWR       0x0A
#define HX8357C_RDMADCTL    0x0B
#define HX8357C_RDCOLMOD    0x0C
#define HX8357C_RDDIM       0x0D
#define HX8357C_RDDSDR      0x0F

#define HX8357C_SLPIN       0x10
#define HX8357C_SLPOUT      0x11
#define HX8357C_PTLON       0x12
#define HX8357C_NORON       0x13

#define HX8357C_INVOFF      0x20
#define HX8357C_INVON       0x21
#define HX8357C_GAMSET      0x26
#define HX8357C_DISPOFF     0x28
#define HX8357C_DISPON      0x29
#define HX8357C_CASET       0x2A
#define HX8357C_PASET       0x2B
#define HX8357C_RAMWR       0x2C
#define HX8357C_RAMRD       0x2E

#define HX8357C_PTLAR       0x30
#define HX8357C_TEON        0x35
#define HX8357C_MADCTL      0x36
#define HX8357C_COLMOD      0x3A
#define HX8357C_SETOSC      0xB0
#define HX8357C_SETPWR1     0xB1
#define HX8357C_SETRGB      0xB3
#define HX8357C_SETCYC      0xB4
#define HX8357C_SETCOM      0xB6
#define HX8357C_SETEXTC     0xB9
#define HX8357C_SETGAMMA    0xC0
#define HX8357C_SETPANEL    0xCC
#define HX8357C_SETPOWER    0xD0
#define HX8357C_SETVCOM     0xD1
#define HX8357C_SETNOR      0xD2
#define HX8357C_SETPAR      0xD3
#define HX8357C_SETPANELREL 0xD4
#define HX8357C_SETSTBA     0xD5
#define HX8357C_SETDGC      0xD6

// Display dimensions - HX8357C typically 320x480
#define HX8357C_TFTWIDTH    320
#define HX8357C_TFTHEIGHT   480

class TFT_Driver_HX8357C : public TFT_Driver_Base {
public:
    TFT_Driver_HX8357C(Config& config);
    ~TFT_Driver_HX8357C() override = default;

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

    // HX8357C specific functions
    void setOscillator(uint8_t osc);
    void setPowerControl(uint8_t vc, uint8_t bt, uint8_t dc);
    void setRGBInterface(uint8_t rgb_if);
    void setCycle(uint8_t cycle);
    void setDisplayControl(uint8_t mode);
    void setGamma(uint8_t gamma);
    void setPanelCharacteristics(uint8_t panel);
    void setPower(uint8_t power);
    void setVCOM(uint8_t vcom);
    void setNormalMode(uint8_t normal);
    void setPartialMode(uint8_t partial);
    void setPanelRelated(uint8_t related);
    void setStandbyMode(uint8_t standby);
    void setDigitalGammaControl(bool enable);
    void setExtendedCommands(bool enable);
    void setTearingEffect(bool enable);
    void setMemoryAccessControl(uint8_t mode);
    void setPixelFormat(uint8_t format);

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

#endif // _TFT_DRIVER_HX8357C_H_
