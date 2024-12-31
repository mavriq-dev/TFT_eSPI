#ifndef _TFT_DRIVER_ST7262_H_
#define _TFT_DRIVER_ST7262_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// ST7262 Commands
#define ST7262_NOP         0x00
#define ST7262_SWRESET     0x01
#define ST7262_RDDID       0x04
#define ST7262_RDDST       0x09
#define ST7262_RDDPM       0x0A
#define ST7262_RDDMADCTL   0x0B
#define ST7262_RDDCOLMOD   0x0C
#define ST7262_RDDIM       0x0D
#define ST7262_RDDSM       0x0E
#define ST7262_RDDSDR      0x0F

#define ST7262_SLPIN       0x10
#define ST7262_SLPOUT      0x11
#define ST7262_PTLON       0x12
#define ST7262_NORON       0x13

#define ST7262_INVOFF      0x20
#define ST7262_INVON       0x21
#define ST7262_GAMSET      0x26
#define ST7262_DISPOFF     0x28
#define ST7262_DISPON      0x29
#define ST7262_CASET       0x2A
#define ST7262_PASET       0x2B
#define ST7262_RAMWR       0x2C
#define ST7262_RAMRD       0x2E

#define ST7262_PTLAR       0x30
#define ST7262_TEOFF       0x34
#define ST7262_TEON        0x35
#define ST7262_MADCTL      0x36
#define ST7262_IDMOFF      0x38
#define ST7262_IDMON       0x39
#define ST7262_COLMOD      0x3A

#define ST7262_RDID1       0xDA
#define ST7262_RDID2       0xDB
#define ST7262_RDID3       0xDC

#define ST7262_SETOTP      0xE0
#define ST7262_SETGAMMA    0xE1
#define ST7262_SETPWCTR1   0xC0
#define ST7262_SETPWCTR2   0xC1
#define ST7262_SETPWCTR3   0xC2
#define ST7262_SETPWCTR4   0xC3
#define ST7262_SETPWCTR5   0xC4
#define ST7262_SETVCOM1    0xC5
#define ST7262_SETVCOM2    0xC6
#define ST7262_SETPGAMMA   0xE0
#define ST7262_SETNGAMMA   0xE1
#define ST7262_SET_PUMP    0xF0

// Display dimensions
#define ST7262_TFTWIDTH    240
#define ST7262_TFTHEIGHT   320

class TFT_Driver_ST7262 : public TFT_Driver_Base {
public:
    TFT_Driver_ST7262(Config& config);
    ~TFT_Driver_ST7262() override = default;

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

    // ST7262 specific functions
    void setPumpControl(uint8_t control);
    void setPowerControl1(uint8_t vrh);
    void setPowerControl2(uint8_t bt);
    void setPowerControl3(uint8_t mode);
    void setPowerControl4(uint8_t mode);
    void setPowerControl5(uint8_t mode);
    void setVCOM1(uint8_t vcm);
    void setVCOM2(uint8_t vcm);
    void setMemoryAccessControl(uint8_t mode);
    void setPixelFormat(uint8_t format);
    void setGamma(uint8_t gamma);
    void setPositiveGamma(const uint8_t* gamma);
    void setNegativeGamma(const uint8_t* gamma);
    void setOTP(uint8_t otp);
    void setColumnAddress(uint16_t start, uint16_t end);
    void setPageAddress(uint16_t start, uint16_t end);
    void setPartialArea(uint16_t start, uint16_t end);
    void setTearingEffect(bool enable, bool mode);
    void setIdleMode(bool enable);
    uint32_t readID();

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

#endif // _TFT_DRIVER_ST7262_H_
