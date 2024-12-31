#ifndef _TFT_COMMANDSET_SSD1963_H_
#define _TFT_COMMANDSET_SSD1963_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_SSD1963 : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("SSD1963 Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(120);
        
        // Set PLL
        seq.addCommand(0xE0, "Set PLL");
        seq.addDataList({0x01});  // Enable PLL
        
        seq.addCommand(0xE2, "Set PLL frequency");
        seq.addDataList({0x23, 0x02, 0x54});  // Multiply by 35 for 120MHz
        
        seq.addDelay(10);
        
        seq.addCommand(0xE0, "Start PLL");
        seq.addDataList({0x03});  // Use PLL
        
        seq.addDelay(10);
        
        // Set LCD Panel
        seq.addCommand(0xB0, "LCD Panel Characteristics");
        seq.addDataList({0x20,   // 18-bit panel
                        0x00,   // TFT mode
                        0x03,   // Horizontal size high byte (800)
                        0x20,   // Horizontal size low byte
                        0x01,   // Vertical size high byte (480)
                        0xE0,   // Vertical size low byte
                        0x00}); // RGB mode
        
        // Set Pixel Data Interface
        seq.addCommand(0xF0, "Pixel Data Interface");
        seq.addDataList({0x03});  // 16-bit (565 format)
        
        // Set Display Mode
        seq.addCommand(0xB4, "Display Mode");
        seq.addDataList({0x00,   // Reserved
                        0x00,   // Reserved
                        0x00,   // Reserved
                        0x00,   // Reserved
                        0x00,   // Reserved
                        0x00,   // Reserved
                        0x00}); // Reserved
        
        // Set Display On
        seq.addCommand(0x29, "Display On");
        
        // Set PWM
        seq.addCommand(0xBE, "Set PWM Configuration");
        seq.addDataList({0x06,   // PWM enabled
                        0xFF,   // PWM frequency
                        0x01,   // PWM polarity
                        0xFF,   // PWM duty cycle
                        0x00,   // Reserved
                        0x00}); // Reserved
        
        // Set Address Mode
        seq.addCommand(0x36, "Set Address Mode");
        seq.addDataList({0x00}); // Normal orientation
        
        // Set Pixel Format
        seq.addCommand(0x3A, "Set Pixel Format");
        seq.addDataList({0x55}); // 16-bit per pixel
        
        // Exit Sleep Mode
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        
        seq.addEnd();
        return seq;
    }

    CommandSequence createSleepSequence() override {
        CommandSequence seq("SSD1963 Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("SSD1963 Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("SSD1963 Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("SSD1963 Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("SSD1963 Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("SSD1963 Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("SSD1963 Set Rotation");
        uint8_t madctl = 0;
        
        switch (rotation) {
            case 0:  // Portrait
                madctl = 0x00;
                break;
            case 1:  // Landscape
                madctl = 0x60;
                break;
            case 2:  // Portrait inverted
                madctl = 0xC0;
                break;
            case 3:  // Landscape inverted
                madctl = 0xA0;
                break;
        }
        
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(madctl);
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetWindowSequence(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
        CommandSequence seq("SSD1963 Set Window");
        
        seq.addCommand(0x2A, "Column Address Set");
        seq.addDataList({
            static_cast<uint8_t>(x0 >> 8),
            static_cast<uint8_t>(x0 & 0xFF),
            static_cast<uint8_t>(x1 >> 8),
            static_cast<uint8_t>(x1 & 0xFF)
        });
        
        seq.addCommand(0x2B, "Page Address Set");
        seq.addDataList({
            static_cast<uint8_t>(y0 >> 8),
            static_cast<uint8_t>(y0 & 0xFF),
            static_cast<uint8_t>(y1 >> 8),
            static_cast<uint8_t>(y1 & 0xFF)
        });
        
        seq.addCommand(0x2C, "Memory Write");
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetPLLSequence(uint8_t multiplier, uint8_t divider) {
        CommandSequence seq("SSD1963 Set PLL");
        
        seq.addCommand(0xE0, "Set PLL");
        seq.addDataList({0x01});  // Enable PLL
        
        seq.addCommand(0xE2, "Set PLL frequency");
        seq.addDataList({multiplier, divider, 0x54});
        
        seq.addDelay(10);
        
        seq.addCommand(0xE0, "Start PLL");
        seq.addDataList({0x03});  // Use PLL
        
        seq.addDelay(10);
        
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetPWMSequence(uint8_t pwm_freq, uint8_t duty_cycle) {
        CommandSequence seq("SSD1963 Set PWM");
        seq.addCommand(0xBE, "Set PWM Configuration");
        seq.addDataList({0x06,       // PWM enabled
                        pwm_freq,    // PWM frequency
                        0x01,        // PWM polarity
                        duty_cycle,  // PWM duty cycle
                        0x00,        // Reserved
                        0x00});      // Reserved
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetLCDModeSequence(uint16_t horiz_size, uint16_t vert_size, uint8_t rgb_mode) {
        CommandSequence seq("SSD1963 Set LCD Mode");
        seq.addCommand(0xB0, "LCD Panel Characteristics");
        seq.addDataList({0x20,   // 18-bit panel
                        0x00,    // TFT mode
                        static_cast<uint8_t>(horiz_size >> 8),
                        static_cast<uint8_t>(horiz_size & 0xFF),
                        static_cast<uint8_t>(vert_size >> 8),
                        static_cast<uint8_t>(vert_size & 0xFF),
                        rgb_mode});
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_SSD1963_H_
