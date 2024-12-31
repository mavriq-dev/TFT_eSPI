#ifndef _TFT_COMMANDSET_ST7796_H_
#define _TFT_COMMANDSET_ST7796_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_ST7796 : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("ST7796 Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(120);
        
        // Command Set Control
        seq.addCommand(0xF0, "Command Set Control");
        seq.addData(0xC3);
        
        seq.addCommand(0xF0, "Command Set Control");
        seq.addData(0x96);
        
        // Memory Access Control
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(0x48);
        
        // Interface Pixel Format
        seq.addCommand(0x3A, "Interface Pixel Format");
        seq.addData(0x55);  // 16-bit color
        
        // Display Function Control
        seq.addCommand(0xB4, "Display Function Control");
        seq.addData(0x01);
        
        // VCOM Control 1
        seq.addCommand(0xB7, "VCOM Control 1");
        seq.addData(0x00);
        
        // Frame Rate Control
        seq.addCommand(0xB5, "Frame Rate Control");
        seq.addDataList({0x00, 0x18});  // 79Hz
        
        // Display Inversion Control
        seq.addCommand(0xB6, "Display Function Control");
        seq.addDataList({0x80, 0x02, 0x3B});
        
        // Power Control 1
        seq.addCommand(0xC0, "Power Control 1");
        seq.addDataList({0x80, 0x64});
        
        // Power Control 2
        seq.addCommand(0xC1, "Power Control 2");
        seq.addData(0x0D);
        
        // Power Control 3
        seq.addCommand(0xC2, "Power Control 3");
        seq.addData(0xA7);
        
        // Positive Voltage Gamma Control
        seq.addCommand(0xE0, "Positive Voltage Gamma Control");
        seq.addDataList({0xF0, 0x00, 0x02, 0x0A, 0x0D, 0x1D, 0x35, 0x55,
                        0x45, 0x3C, 0x17, 0x17, 0x35, 0x3D});
        
        // Negative Voltage Gamma Control
        seq.addCommand(0xE1, "Negative Voltage Gamma Control");
        seq.addDataList({0xF0, 0x00, 0x02, 0x07, 0x06, 0x04, 0x2E, 0x44,
                        0x45, 0x0B, 0x17, 0x16, 0x2B, 0x33});
        
        // Enable 3 Gamma Control
        seq.addCommand(0xF2, "Enable 3 Gamma Control");
        seq.addData(0x00);
        
        // Sleep Out
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        
        // Display On
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        
        seq.addEnd();
        return seq;
    }

    CommandSequence createSleepSequence() override {
        CommandSequence seq("ST7796 Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("ST7796 Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("ST7796 Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("ST7796 Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("ST7796 Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("ST7796 Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    // Additional ST7796-specific sequences
    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("ST7796 Set Rotation");
        uint8_t madctl = 0;
        
        switch (rotation) {
            case 0:  // Portrait
                madctl = 0x48;
                break;
            case 1:  // Landscape
                madctl = 0x28;
                break;
            case 2:  // Portrait inverted
                madctl = 0x88;
                break;
            case 3:  // Landscape inverted
                madctl = 0xE8;
                break;
        }
        
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(madctl);
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetWindowSequence(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
        CommandSequence seq("ST7796 Set Window");
        
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

    CommandSequence createSetScrollAreaSequence(uint16_t topFixed, uint16_t scrollArea, uint16_t bottomFixed) {
        CommandSequence seq("ST7796 Set Scroll Area");
        
        seq.addCommand(0x33, "Vertical Scroll Definition");
        seq.addDataList({
            static_cast<uint8_t>(topFixed >> 8),
            static_cast<uint8_t>(topFixed & 0xFF),
            static_cast<uint8_t>(scrollArea >> 8),
            static_cast<uint8_t>(scrollArea & 0xFF),
            static_cast<uint8_t>(bottomFixed >> 8),
            static_cast<uint8_t>(bottomFixed & 0xFF)
        });
        
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetScrollStartSequence(uint16_t startLine) {
        CommandSequence seq("ST7796 Set Scroll Start");
        seq.addCommand(0x37, "Vertical Scrolling Start Address");
        seq.addDataList({
            static_cast<uint8_t>(startLine >> 8),
            static_cast<uint8_t>(startLine & 0xFF)
        });
        seq.addEnd();
        return seq;
    }

    CommandSequence createIdleModeSequence(bool enable) {
        CommandSequence seq("ST7796 Idle Mode");
        seq.addCommand(enable ? 0x39 : 0x38, enable ? "Idle Mode ON" : "Idle Mode OFF");
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_ST7796_H_
