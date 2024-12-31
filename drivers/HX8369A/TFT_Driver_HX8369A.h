#ifndef _TFT_DRIVER_HX8369A_H_
#define _TFT_DRIVER_HX8369A_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// HX8369A Commands
#define HX8369A_NOP         0x00
#define HX8369A_SWRESET     0x01
#define HX8369A_RDDID       0x04
#define HX8369A_RDNUMED     0x05
#define HX8369A_RDDST       0x09
#define HX8369A_RDDPM       0x0A
#define HX8369A_RDDMADCTL   0x0B
#define HX8369A_RDDCOLMOD   0x0C
#define HX8369A_RDDIM       0x0D
#define HX8369A_RDDSM       0x0E
#define HX8369A_RDDSDR      0x0F

#define HX8369A_SLPIN       0x10
#define HX8369A_SLPOUT      0x11
#define HX8369A_PTLON       0x12
#define HX8369A_NORON       0x13

#define HX8369A_INVOFF      0x20
#define HX8369A_INVON       0x21
#define HX8369A_GAMSET      0x26
#define HX8369A_DISPOFF     0x28
#define HX8369A_DISPON      0x29
#define HX8369A_CASET       0x2A
#define HX8369A_PASET       0x2B
#define HX8369A_RAMWR       0x2C
#define HX8369A_RAMRD       0x2E

#define HX8369A_PTLAR       0x30
#define HX8369A_VSCRDEF     0x33
#define HX8369A_TEOFF       0x34
#define HX8369A_TEON        0x35
#define HX8369A_MADCTL      0x36
#define HX8369A_VSCRSADD    0x37
#define HX8369A_IDMOFF      0x38
#define HX8369A_IDMON       0x39
#define HX8369A_COLMOD      0x3A

#define HX8369A_SETPWD      0xB1
#define HX8369A_SETDISPLAY  0xB2
#define HX8369A_SETRGB      0xB3
#define HX8369A_SETCYC      0xB4
#define HX8369A_SETVCOM     0xB6
#define HX8369A_SETEXTC     0xB9
#define HX8369A_SETMIPI     0xBA
#define HX8369A_SETOTP      0xBB
#define HX8369A_SETPOWER    0xBC
#define HX8369A_SETCABC     0xC9

// Display dimensions
#define HX8369A_TFTWIDTH    480
#define HX8369A_TFTHEIGHT   800

class TFT_Driver_HX8369A : public TFT_Driver_Base {
public:
    TFT_Driver_HX8369A(Config& config);
    ~TFT_Driver_HX8369A() override = default;

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

    // HX8369A specific functions
    void setPassword(uint8_t pwd);
    void setDisplayMode(uint8_t mode);
    void setRGBInterface(uint8_t rgb_if);
    void setCycle(uint8_t cycle);
    void setVCOM(uint8_t vcom);
    void setExtendedCommands(bool enable);
    void setMIPIControl(uint8_t mipi);
    void setOTPControl(uint8_t otp);
    void setPowerControl(uint8_t power);
    void setCABC(uint8_t cabc);
    void setGamma(uint8_t gamma);
    void setMemoryAccessControl(uint8_t mode);
    void setPixelFormat(uint8_t format);
    void setColumnAddress(uint16_t start, uint16_t end);
    void setPageAddress(uint16_t start, uint16_t end);
    void setScrollArea(uint16_t tfa, uint16_t vsa, uint16_t bfa);
    void setScrollStart(uint16_t start);
    void setTearingEffect(bool enable, bool mode);
    void setPartialArea(uint16_t start, uint16_t end);
    void setIdleMode(bool enable);

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

#endif // _TFT_DRIVER_HX8369A_H_
