#ifndef _TFT_DRIVER_SSD1963_H_
#define _TFT_DRIVER_SSD1963_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// SSD1963 specific commands
#define SSD1963_NOP         0x00
#define SSD1963_SWRESET     0x01
#define SSD1963_RDDID       0x04
#define SSD1963_RDDST       0x09

#define SSD1963_SLPIN       0x10
#define SSD1963_SLPOUT      0x11
#define SSD1963_PTLON       0x12
#define SSD1963_NORON       0x13

#define SSD1963_INVOFF      0x20
#define SSD1963_INVON       0x21
#define SSD1963_DISPOFF     0x28
#define SSD1963_DISPON      0x29

#define SSD1963_CASET       0x2A
#define SSD1963_PASET       0x2B
#define SSD1963_RAMWR       0x2C
#define SSD1963_RAMRD       0x2E

#define SSD1963_PTLAR       0x30
#define SSD1963_MADCTL      0x36
#define SSD1963_PIXFMT      0x3A

// SSD1963 Extended Commands
#define SSD1963_SETPLL      0xE0
#define SSD1963_SETPLLMN    0xE2
#define SSD1963_SETLCDMODE  0xB0
#define SSD1963_SETGPIO     0xB8
#define SSD1963_SETPWM      0xBE
#define SSD1963_SETDITHER   0xC0
#define SSD1963_SETVCOM     0xC4
#define SSD1963_SETPANEL    0xC6
#define SSD1963_SETGAMMA    0xC8
#define SSD1963_SETPRECHARGE 0xD0
#define SSD1963_SETVCOMH    0xD1
#define SSD1963_SETRGB      0xD4

// Display dimensions - SSD1963 supports multiple resolutions, this is for 800x480
#define SSD1963_TFTWIDTH    800
#define SSD1963_TFTHEIGHT   480

class TFT_Driver_SSD1963 : public TFT_Driver_Base {
public:
    TFT_Driver_SSD1963(Config& config);
    ~TFT_Driver_SSD1963() override = default;

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

    // SSD1963 specific functions
    void setPLL(uint8_t mult, uint8_t div);
    void setLCDMode(uint8_t hori, uint8_t vert, uint8_t pol);
    void setBacklight(uint8_t value);

private:
    uint16_t _width;
    uint16_t _height;
    uint8_t _rotation;
    bool _invert;
};

} // namespace TFT_Runtime

#endif // _TFT_DRIVER_SSD1963_H_
