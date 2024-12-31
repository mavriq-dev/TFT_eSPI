#ifndef _TFT_DRIVER_ILI9225_H_
#define _TFT_DRIVER_ILI9225_H_

#include "TFT_Driver_Base.h"

namespace TFT_Runtime {

// ILI9225 specific commands
#define ILI9225_DRIVER_OUTPUT_CTRL      0x01  // Driver Output Control
#define ILI9225_LCD_AC_DRIVING_CTRL     0x02  // LCD AC Driving Control
#define ILI9225_ENTRY_MODE              0x03  // Entry Mode
#define ILI9225_DISP_CTRL1             0x07  // Display Control 1
#define ILI9225_BLANK_PERIOD_CTRL1     0x08  // Blank Period Control
#define ILI9225_FRAME_CYCLE_CTRL       0x0B  // Frame Cycle Control
#define ILI9225_INTERFACE_CTRL         0x0C  // Interface Control
#define ILI9225_OSC_CTRL               0x0F  // Oscillator Control
#define ILI9225_POWER_CTRL1            0x10  // Power Control 1
#define ILI9225_POWER_CTRL2            0x11  // Power Control 2
#define ILI9225_POWER_CTRL3            0x12  // Power Control 3
#define ILI9225_POWER_CTRL4            0x13  // Power Control 4
#define ILI9225_POWER_CTRL5            0x14  // Power Control 5
#define ILI9225_VCI_RECYCLING          0x15  // VCI Recycling
#define ILI9225_RAM_ADDR_SET1          0x20  // Horizontal GRAM Address Set
#define ILI9225_RAM_ADDR_SET2          0x21  // Vertical GRAM Address Set
#define ILI9225_GRAM_DATA_REG          0x22  // GRAM Data Register
#define ILI9225_GATE_SCAN_CTRL         0x30  // Gate Scan Control Register
#define ILI9225_VERTICAL_SCROLL_CTRL1  0x31  // Vertical Scroll Control 1 Register
#define ILI9225_VERTICAL_SCROLL_CTRL2  0x32  // Vertical Scroll Control 2 Register
#define ILI9225_VERTICAL_SCROLL_CTRL3  0x33  // Vertical Scroll Control 3 Register
#define ILI9225_PARTIAL_DRIVING_POS1   0x34  // Partial Driving Position 1 Register
#define ILI9225_PARTIAL_DRIVING_POS2   0x35  // Partial Driving Position 2 Register
#define ILI9225_HORIZONTAL_WINDOW_ADDR1 0x36  // Horizontal Address Start Position
#define ILI9225_HORIZONTAL_WINDOW_ADDR2 0x37  // Horizontal Address End Position
#define ILI9225_VERTICAL_WINDOW_ADDR1  0x38  // Vertical Address Start Position
#define ILI9225_VERTICAL_WINDOW_ADDR2  0x39  // Vertical Address End Position
#define ILI9225_GAMMA_CTRL1            0x50  // Gamma Control 1
#define ILI9225_GAMMA_CTRL2            0x51  // Gamma Control 2
#define ILI9225_GAMMA_CTRL3            0x52  // Gamma Control 3
#define ILI9225_GAMMA_CTRL4            0x53  // Gamma Control 4
#define ILI9225_GAMMA_CTRL5            0x54  // Gamma Control 5
#define ILI9225_GAMMA_CTRL6            0x55  // Gamma Control 6
#define ILI9225_GAMMA_CTRL7            0x56  // Gamma Control 7
#define ILI9225_GAMMA_CTRL8            0x57  // Gamma Control 8
#define ILI9225_GAMMA_CTRL9            0x58  // Gamma Control 9
#define ILI9225_GAMMA_CTRL10           0x59  // Gamma Control 10

// Display dimensions - ILI9225 supports 176x220
#define ILI9225_TFTWIDTH               176
#define ILI9225_TFTHEIGHT              220

class TFT_Driver_ILI9225 : public TFT_Driver_Base {
public:
    TFT_Driver_ILI9225(Config& config);
    ~TFT_Driver_ILI9225() override = default;

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

    // ILI9225 specific functions
    void setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
    void setDisplayMode(uint8_t mode);
    void setOscillator(uint8_t osc);
    void setPowerControl(uint8_t ctrl1, uint8_t ctrl2, uint8_t ctrl3);
    void setDrivingControl(uint8_t mode);
    void setGamma(uint8_t gamma1, uint8_t gamma2, uint8_t gamma3);
    void setPartialArea(uint16_t start, uint16_t end);
    void setScrollArea(uint16_t top, uint16_t scroll, uint16_t bottom);

private:
    uint16_t _width;
    uint16_t _height;
    uint8_t _rotation;
    bool _invert;
    
    void writeRegister(uint8_t reg, uint16_t data);
    void initPowerSettings();
    void initDisplaySettings();
    void initGamma();
    void setGramArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
};

} // namespace TFT_Runtime

#endif // _TFT_DRIVER_ILI9225_H_
