#ifndef _TFT_COMMANDSET_ILI9488_H_
#define _TFT_COMMANDSET_ILI9488_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_ILI9488 : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("ILI9488 Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(120);
        
        // Positive Gamma Control
        seq.addCommand(0xE0, "Positive Gamma Control");
        seq.addDataList({0x00, 0x03, 0x09, 0x08, 0x16, 0x0A, 0x3F, 0x78,
                        0x4C, 0x09, 0x0A, 0x08, 0x16, 0x1A, 0x0F});
        
        // Negative Gamma Control
        seq.addCommand(0xE1, "Negative Gamma Control");
        seq.addDataList({0x00, 0x16, 0x19, 0x03, 0x0F, 0x05, 0x32, 0x45,
                        0x46, 0x04, 0x0E, 0x0D, 0x35, 0x37, 0x0F});
        
        // Power Control 1
        seq.addCommand(0xC0, "Power Control 1");
        seq.addDataList({0x17, 0x15});
        
        // Power Control 2
        seq.addCommand(0xC1, "Power Control 2");
        seq.addData(0x41);
        
        // VCOM Control
        seq.addCommand(0xC5, "VCOM Control");
        seq.addDataList({0x00, 0x12, 0x80});
        
        // Memory Access Control
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(0x48);
        
        // Interface Pixel Format
        seq.addCommand(0x3A, "Interface Pixel Format");
        seq.addData(0x66);  // 18-bit color format
        
        // Interface Mode Control
        seq.addCommand(0xB0, "Interface Mode Control");
        seq.addData(0x80);  // SDO NOT USE
        
        // Frame Rate Control
        seq.addCommand(0xB1, "Frame Rate Control");
        seq.addDataList({0xA0, 0x11});
        
        // Display Inversion Control
        seq.addCommand(0xB4, "Display Inversion Control");
        seq.addData(0x02);
        
        // Display Function Control
        seq.addCommand(0xB6, "Display Function Control");
        seq.addDataList({0x02, 0x02, 0x3B});
        
        // Entry Mode Set
        seq.addCommand(0xB7, "Entry Mode Set");
        seq.addData(0xC6);
        
        // Adjust Control 3
        seq.addCommand(0xF7, "Adjust Control 3");
        seq.addDataList({0xA9, 0x51, 0x2C, 0x82});
        
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
        CommandSequence seq("ILI9488 Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("ILI9488 Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("ILI9488 Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("ILI9488 Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("ILI9488 Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("ILI9488 Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    // Additional ILI9488-specific sequences
    CommandSequence createSetColorModeSequence(uint8_t colorMode) {
        CommandSequence seq("ILI9488 Set Color Mode");
        seq.addCommand(0x3A, "Interface Pixel Format");
        seq.addData(colorMode);  // 0x66 for 18-bit, 0x55 for 16-bit
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetBrightnessSequence(uint8_t brightness) {
        CommandSequence seq("ILI9488 Set Brightness");
        seq.addCommand(0x51, "Write Display Brightness");
        seq.addData(brightness);
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetScrollAreaSequence(uint16_t topFixed, uint16_t scrollArea, uint16_t bottomFixed) {
        CommandSequence seq("ILI9488 Set Scroll Area");
        seq.addCommand(0x33, "Vertical Scrolling Definition");
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

    CommandSequence createAllPixelsOnSequence(bool enable) {
        CommandSequence seq("ILI9488 All Pixels");
        seq.addCommand(enable ? 0x23 : 0x22, enable ? "All Pixels ON" : "All Pixels OFF");
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_ILI9488_H_
