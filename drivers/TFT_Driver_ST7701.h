#ifndef _TFT_DRIVER_ST7701_H_
#define _TFT_DRIVER_ST7701_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// ST7701 specific commands
#define ST7701_NOP        0x00
#define ST7701_SWRESET    0x01
#define ST7701_RDDID      0x04
#define ST7701_RDDST      0x09

#define ST7701_SLPIN      0x10
#define ST7701_SLPOUT     0x11
#define ST7701_PTLON      0x12
#define ST7701_NORON      0x13

#define ST7701_INVOFF     0x20
#define ST7701_INVON      0x21
#define ST7701_DISPOFF    0x28
#define ST7701_DISPON     0x29

#define ST7701_CASET      0x2A
#define ST7701_RASET      0x2B
#define ST7701_RAMWR      0x2C
#define ST7701_RAMRD      0x2E

#define ST7701_PTLAR      0x30
#define ST7701_VSCRDEF    0x33
#define ST7701_TEOFF      0x34
#define ST7701_TEON       0x35
#define ST7701_MADCTL     0x36
#define ST7701_COLMOD     0x3A

// ST7701 Extended Commands
#define ST7701_CMD_ACCESS 0xFF
#define ST7701_VCOMS      0xB0
#define ST7701_VGHSS      0xB1
#define ST7701_TESTCMD    0xB2
#define ST7701_VGLS       0xB5
#define ST7701_PWCTLR1    0xB7
#define ST7701_PWCTLR2    0xB8
#define ST7701_PWCTLR3    0xB9
#define ST7701_PWCTLR4    0xBA
#define ST7701_PWCTLR5    0xBC
#define ST7701_PWCTLR6    0xBD

#define ST7701_PGAMMA     0xE0
#define ST7701_NGAMMA     0xE1
#define ST7701_DGAMMA     0xE2

// Display dimensions - ST7701 commonly used in 480x800
#define ST7701_TFTWIDTH   480
#define ST7701_TFTHEIGHT  800

class TFT_Driver_ST7701 : public TFT_Driver_Base {
public:
    TFT_Driver_ST7701(Config& config);
    ~TFT_Driver_ST7701() override = default;

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

    // ST7701 specific functions
    void setCommandPage(uint8_t page);
    void setVCOM(uint8_t value);
    void setGamma(uint8_t* pgamma, uint8_t* ngamma);

private:
    uint16_t _width;
    uint16_t _height;
    uint8_t _rotation;
    bool _invert;
    
    void initCommandPage1();
    void initCommandPage2();
    void initCommandPage3();
};

} // namespace TFT_Runtime

#endif // _TFT_DRIVER_ST7701_H_
