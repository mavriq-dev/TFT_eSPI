#ifndef _TFT_DRIVER_SSD1351_H_
#define _TFT_DRIVER_SSD1351_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// SSD1351 Commands
#define SSD1351_CMD_SETCOLUMN       0x15
#define SSD1351_CMD_SETROW          0x75
#define SSD1351_CMD_WRITERAM        0x5C
#define SSD1351_CMD_READRAM         0x5D
#define SSD1351_CMD_SETREMAP        0xA0
#define SSD1351_CMD_STARTLINE       0xA1
#define SSD1351_CMD_DISPLAYOFFSET   0xA2
#define SSD1351_CMD_DISPLAYALLOFF   0xA4
#define SSD1351_CMD_DISPLAYALLON    0xA5
#define SSD1351_CMD_NORMALDISPLAY   0xA6
#define SSD1351_CMD_INVERTDISPLAY   0xA7
#define SSD1351_CMD_FUNCTIONSELECT  0xAB
#define SSD1351_CMD_DISPLAYOFF      0xAE
#define SSD1351_CMD_DISPLAYON       0xAF
#define SSD1351_CMD_PRECHARGE       0xB1
#define SSD1351_CMD_DISPLAYENHANCE  0xB2
#define SSD1351_CMD_CLOCKDIV        0xB3
#define SSD1351_CMD_SETVSL          0xB4
#define SSD1351_CMD_SETGPIO         0xB5
#define SSD1351_CMD_PRECHARGE2      0xB6
#define SSD1351_CMD_SETGRAY        0xB8
#define SSD1351_CMD_USELUT          0xB9
#define SSD1351_CMD_PRECHARGELEVEL  0xBB
#define SSD1351_CMD_VCOMH           0xBE
#define SSD1351_CMD_CONTRASTABC     0xC1
#define SSD1351_CMD_CONTRASTMASTER  0xC7
#define SSD1351_CMD_MUXRATIO        0xCA
#define SSD1351_CMD_COMMANDLOCK     0xFD
#define SSD1351_CMD_HORIZSCROLL     0x96
#define SSD1351_CMD_STOPSCROLL      0x9E
#define SSD1351_CMD_STARTSCROLL     0x9F

// Display dimensions - SSD1351 typically 128x128
#define SSD1351_TFTWIDTH            128
#define SSD1351_TFTHEIGHT           128

class TFT_Driver_SSD1351 : public TFT_Driver_Base {
public:
    TFT_Driver_SSD1351(Config& config);
    ~TFT_Driver_SSD1351() override = default;

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

    // SSD1351 specific functions
    void setContrast(uint8_t contrast);
    void setContrastColor(uint8_t r, uint8_t g, uint8_t b);
    void setDisplayEnhancement(bool enable);
    void setPrecharge(uint8_t phase1, uint8_t phase2);
    void setVSL(uint8_t vsl);
    void setCommandLock(bool lock);
    void startScrolling(uint8_t direction, uint8_t start, uint8_t stop, uint8_t speed);
    void stopScrolling();
    void enableGrayScale(bool enable);
    void setMuxRatio(uint8_t ratio);
    void setDisplayOffset(uint8_t offset);
    void setStartLine(uint8_t line);

private:
    uint16_t _width;
    uint16_t _height;
    uint8_t _rotation;
    bool _invert;
    
    void initPowerSettings();
    void initDisplaySettings();
    void setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
    void setRemapFormat(uint8_t format);
};

} // namespace TFT_Runtime

#endif // _TFT_DRIVER_SSD1351_H_
