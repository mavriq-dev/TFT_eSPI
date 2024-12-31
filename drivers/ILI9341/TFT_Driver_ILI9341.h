#ifndef _TFT_DRIVER_ILI9341_H_
#define _TFT_DRIVER_ILI9341_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

class TFT_Driver_ILI9341 : public TFT_Driver_Base {
public:
    TFT_Driver_ILI9341(Config& config);
    ~TFT_Driver_ILI9341() override = default;

    // Implementation of pure virtual functions
    void init() override;
    void writeCommand(uint8_t cmd) override;
    void writeData(uint8_t data) override;
    void writeBlock(uint16_t* data, uint32_t len) override;
    
    // Display specific parameters
    uint16_t width() const override { return _width; }
    uint16_t height() const override { return _height; }
    uint8_t getRotation() const override { return _rotation; }

    // Override optional functions
    void setRotation(uint8_t rotation) override;
    void invertDisplay(bool invert) override;

private:
    static constexpr uint16_t ILI9341_TFTWIDTH = 240;
    static constexpr uint16_t ILI9341_TFTHEIGHT = 320;

    // ILI9341 specific commands
    static constexpr uint8_t ILI9341_NOP = 0x00;
    static constexpr uint8_t ILI9341_SWRESET = 0x01;
    static constexpr uint8_t ILI9341_RDDID = 0x04;
    static constexpr uint8_t ILI9341_RDDST = 0x09;
    static constexpr uint8_t ILI9341_SLPIN = 0x10;
    static constexpr uint8_t ILI9341_SLPOUT = 0x11;
    static constexpr uint8_t ILI9341_PTLON = 0x12;
    static constexpr uint8_t ILI9341_NORON = 0x13;
    static constexpr uint8_t ILI9341_RDMODE = 0x0A;
    static constexpr uint8_t ILI9341_RDMADCTL = 0x0B;
    static constexpr uint8_t ILI9341_RDPIXFMT = 0x0C;
    static constexpr uint8_t ILI9341_RDIMGFMT = 0x0A;
    static constexpr uint8_t ILI9341_RDSELFDIAG = 0x0F;
    static constexpr uint8_t ILI9341_INVOFF = 0x20;
    static constexpr uint8_t ILI9341_INVON = 0x21;
    static constexpr uint8_t ILI9341_GAMMASET = 0x26;
    static constexpr uint8_t ILI9341_DISPOFF = 0x28;
    static constexpr uint8_t ILI9341_DISPON = 0x29;
    static constexpr uint8_t ILI9341_CASET = 0x2A;
    static constexpr uint8_t ILI9341_PASET = 0x2B;
    static constexpr uint8_t ILI9341_RAMWR = 0x2C;
    static constexpr uint8_t ILI9341_RAMRD = 0x2E;
    static constexpr uint8_t ILI9341_MADCTL = 0x36;
    static constexpr uint8_t ILI9341_PIXFMT = 0x3A;

    // Add missing command definitions
    static constexpr uint8_t ILI9341_PWCTR1 = 0xC0;    // Power Control 1
    static constexpr uint8_t ILI9341_PWCTR2 = 0xC1;    // Power Control 2
    static constexpr uint8_t ILI9341_VMCTR1 = 0xC5;    // VCOM Control 1
    static constexpr uint8_t ILI9341_VMCTR2 = 0xC7;    // VCOM Control 2
    static constexpr uint8_t ILI9341_FRMCTR1 = 0xB1;   // Frame Rate Control (normal mode)
    static constexpr uint8_t ILI9341_DFUNCTR = 0xB6;   // Display Function Control
};

} // namespace TFT_Runtime

#endif // _TFT_DRIVER_ILI9341_H_
