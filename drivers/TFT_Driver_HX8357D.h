#ifndef _TFT_DRIVER_HX8357D_H_
#define _TFT_DRIVER_HX8357D_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// HX8357D specific commands
#define HX8357D_NOP        0x00
#define HX8357D_SWRESET    0x01
#define HX8357D_RDDID      0x04
#define HX8357D_RDDST      0x09

#define HX8357D_SLPIN      0x10
#define HX8357D_SLPOUT     0x11
#define HX8357D_PTLON      0x12
#define HX8357D_NORON      0x13

#define HX8357D_INVOFF     0x20
#define HX8357D_INVON      0x21
#define HX8357D_DISPOFF    0x28
#define HX8357D_DISPON     0x29

#define HX8357D_CASET      0x2A
#define HX8357D_PASET      0x2B
#define HX8357D_RAMWR      0x2C
#define HX8357D_RAMRD      0x2E

#define HX8357D_TEON       0x35
#define HX8357D_MADCTL     0x36
#define HX8357D_COLMOD     0x3A
#define HX8357D_TEARLINE   0x44

#define HX8357D_SETOSC     0xB0
#define HX8357D_SETPWR1    0xB1
#define HX8357D_SETRGB     0xB3
#define HX8357D_SETCOM     0xB6

#define HX8357D_SETCYC     0xB4
#define HX8357D_SETC       0xB9
#define HX8357D_SETSTBA    0xC0
#define HX8357D_SETPANEL   0xCC

#define HX8357D_SETGAMMA   0xE0

// Display dimensions
#define HX8357D_TFTWIDTH   320
#define HX8357D_TFTHEIGHT  480

class TFT_Driver_HX8357D : public TFT_Driver_Base {
public:
    TFT_Driver_HX8357D(Config& config);
    ~TFT_Driver_HX8357D() override = default;

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

private:
    uint16_t _width;
    uint16_t _height;
    uint8_t _rotation;
    bool _invert;
};

} // namespace TFT_Runtime

#endif // _TFT_DRIVER_HX8357D_H_
