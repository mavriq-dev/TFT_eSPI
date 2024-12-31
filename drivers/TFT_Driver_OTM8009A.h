#ifndef _TFT_DRIVER_OTM8009A_H_
#define _TFT_DRIVER_OTM8009A_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// OTM8009A specific commands
#define OTM8009A_NOP        0x00
#define OTM8009A_SWRESET    0x01
#define OTM8009A_RDDID      0x04
#define OTM8009A_RDDST      0x09

#define OTM8009A_SLPIN      0x10
#define OTM8009A_SLPOUT     0x11
#define OTM8009A_PTLON      0x12
#define OTM8009A_NORON      0x13

#define OTM8009A_INVOFF     0x20
#define OTM8009A_INVON      0x21
#define OTM8009A_DISPOFF    0x28
#define OTM8009A_DISPON     0x29
#define OTM8009A_CASET      0x2A
#define OTM8009A_PASET      0x2B
#define OTM8009A_RAMWR      0x2C
#define OTM8009A_RAMRD      0x2E

#define OTM8009A_PTLAR      0x30
#define OTM8009A_MADCTL     0x36
#define OTM8009A_COLMOD     0x3A
#define OTM8009A_WRDISBV    0x51
#define OTM8009A_WRCTRLD    0x53
#define OTM8009A_WRCABC     0x55
#define OTM8009A_WRCABCMB   0x5E

// Extended commands
#define OTM8009A_RDID1      0xDA
#define OTM8009A_RDID2      0xDB
#define OTM8009A_RDID3      0xDC

#define OTM8009A_SETEXTC    0xFF
#define OTM8009A_SETGIP1    0xC0
#define OTM8009A_SETGIP2    0xC1
#define OTM8009A_SETPANEL   0xCC
#define OTM8009A_SETPOWER   0xD0
#define OTM8009A_SETVCOM    0xD1
#define OTM8009A_SETMIPI    0xE0
#define OTM8009A_SETGAMMA   0xE1

// Display dimensions - OTM8009A supports up to 480x800
#define OTM8009A_TFTWIDTH   480
#define OTM8009A_TFTHEIGHT  800

class TFT_Driver_OTM8009A : public TFT_Driver_Base {
public:
    TFT_Driver_OTM8009A(Config& config);
    ~TFT_Driver_OTM8009A() override = default;

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

    // OTM8009A specific functions
    void setBrightness(uint8_t brightness);
    void setCABC(uint8_t mode);
    void setVCOM(uint8_t value);
    void setMIPILanes(uint8_t lanes);
    void setPanelCharacteristics(uint8_t mode);
    void setGIPTiming(uint8_t mode);
    void enableExtendedCommands(bool enable);

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

#endif // _TFT_DRIVER_OTM8009A_H_
