#ifndef _TFT_COMMANDSET_ST7701_H_
#define _TFT_COMMANDSET_ST7701_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_ST7701 : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("ST7701 Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(120);
        
        // Command2 Enable
        seq.addCommand(0xFF, "Command2 Enable");
        seq.addDataList({0x77, 0x01, 0x00, 0x00, 0x10});
        
        // VCOM Control
        seq.addCommand(0xC0, "VCOM Control");
        seq.addDataList({0x3B, 0x00});
        
        // Power Control 6
        seq.addCommand(0xC6, "Power Control 6");
        seq.addData(0x08);
        
        // Exit Command2
        seq.addCommand(0xFF, "Exit Command2");
        seq.addDataList({0x77, 0x01, 0x00, 0x00, 0x00});
        
        // Command3 Enable
        seq.addCommand(0xFF, "Command3 Enable");
        seq.addDataList({0x77, 0x01, 0x00, 0x00, 0x11});
        
        // Power Control Setting
        seq.addCommand(0xB0, "Power Control Setting");
        seq.addDataList({0x6D});
        
        // VCOM Setting
        seq.addCommand(0xB1, "VCOM Setting");
        seq.addData(0x37);
        
        // Exit Command3
        seq.addCommand(0xFF, "Exit Command3");
        seq.addDataList({0x77, 0x01, 0x00, 0x00, 0x00});
        
        // Memory Access Control
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(0x00);
        
        // Interface Pixel Format
        seq.addCommand(0x3A, "Interface Pixel Format");
        seq.addData(0x70);  // 24-bit
        
        // Display Function Control
        seq.addCommand(0xB6, "Display Function Control");
        seq.addDataList({0x00, 0x62, 0x3B});
        
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
        CommandSequence seq("ST7701 Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("ST7701 Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("ST7701 Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("ST7701 Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("ST7701 Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("ST7701 Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    // Additional ST7701-specific sequences
    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("ST7701 Set Rotation");
        uint8_t madctl = 0;
        
        switch (rotation) {
            case 0:
                madctl = 0x00;
                break;
            case 1:
                madctl = 0x60;
                break;
            case 2:
                madctl = 0xC0;
                break;
            case 3:
                madctl = 0xA0;
                break;
        }
        
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(madctl);
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetWindowSequence(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
        CommandSequence seq("ST7701 Set Window");
        
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

    CommandSequence createSetVCOMSequence(uint8_t vcom) {
        CommandSequence seq("ST7701 Set VCOM");
        
        // Enter Command2
        seq.addCommand(0xFF, "Enter Command2");
        seq.addDataList({0x77, 0x01, 0x00, 0x00, 0x10});
        
        // Set VCOM
        seq.addCommand(0xC0, "VCOM Control");
        seq.addDataList({vcom, 0x00});
        
        // Exit Command2
        seq.addCommand(0xFF, "Exit Command2");
        seq.addDataList({0x77, 0x01, 0x00, 0x00, 0x00});
        
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetColorModeSequence(uint8_t colorMode) {
        CommandSequence seq("ST7701 Set Color Mode");
        seq.addCommand(0x3A, "Interface Pixel Format");
        seq.addData(colorMode);  // 0x70 = 24-bit, 0x60 = 18-bit, 0x50 = 16-bit
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_ST7701_H_
