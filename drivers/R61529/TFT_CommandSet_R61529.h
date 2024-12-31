#ifndef _TFT_COMMANDSET_R61529_H_
#define _TFT_COMMANDSET_R61529_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_R61529 : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("R61529 Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(120);
        
        // Power Setting
        seq.addCommand(0xB0, "Power Setting");
        seq.addDataList({0x00, 0x00, 0x03, 0x3B, 0x00, 0x0D, 0x00, 0x00});
        
        // Driver Output Control
        seq.addCommand(0xB1, "Driver Output Control");
        seq.addDataList({0x00, 0x16, 0x06, 0x00});
        
        // LCD Drive AC Control
        seq.addCommand(0xB4, "LCD Drive AC Control");
        seq.addDataList({0x11, 0x00, 0x00});
        
        // Entry Mode
        seq.addCommand(0xB5, "Entry Mode");
        seq.addData(0x00);
        
        // Display Control 1
        seq.addCommand(0xB6, "Display Control 1");
        seq.addDataList({0x02, 0x22});
        
        // Display Control 2
        seq.addCommand(0xB7, "Display Control 2");
        seq.addDataList({0x46, 0x00, 0x00});
        
        // Display Mode Control
        seq.addCommand(0xB8, "Display Mode Control");
        seq.addData(0x00);
        
        // External Interface Control
        seq.addCommand(0xBA, "External Interface Control");
        seq.addData(0x00);
        
        // Panel Interface Control 1
        seq.addCommand(0xBB, "Panel Interface Control 1");
        seq.addDataList({0x00, 0x00});
        
        // Panel Interface Control 2
        seq.addCommand(0xBC, "Panel Interface Control 2");
        seq.addDataList({0x00, 0x00, 0x00});
        
        // Panel Interface Control 3
        seq.addCommand(0xBD, "Panel Interface Control 3");
        seq.addDataList({0x00, 0x00, 0x00});
        
        // Panel Interface Control 4
        seq.addCommand(0xBE, "Panel Interface Control 4");
        seq.addDataList({0x00, 0x00, 0x00});
        
        // Panel Interface Control 5
        seq.addCommand(0xBF, "Panel Interface Control 5");
        seq.addData(0x00);
        
        // Gamma Setting
        seq.addCommand(0xC0, "Gamma Setting");
        seq.addDataList({0x10, 0x00, 0x01, 0x00, 0x00, 0x33});
        
        // Gamma Setting for Blue
        seq.addCommand(0xC1, "Gamma Setting Blue");
        seq.addDataList({0x01, 0x02, 0x0A, 0x0D, 0x1C, 0x26, 0x2F, 0x32,
                        0x36, 0x3F, 0x47, 0x50, 0x59, 0x60, 0x68, 0x71});
        
        // Gamma Setting for Green
        seq.addCommand(0xC2, "Gamma Setting Green");
        seq.addDataList({0x01, 0x02, 0x09, 0x0D, 0x1C, 0x26, 0x2F, 0x32,
                        0x36, 0x3F, 0x47, 0x50, 0x59, 0x60, 0x68, 0x71});
        
        // Gamma Setting for Red
        seq.addCommand(0xC3, "Gamma Setting Red");
        seq.addDataList({0x01, 0x02, 0x09, 0x0D, 0x1C, 0x26, 0x2F, 0x32,
                        0x36, 0x3F, 0x47, 0x50, 0x59, 0x60, 0x68, 0x71});
        
        // Interface Control
        seq.addCommand(0xC4, "Interface Control");
        seq.addDataList({0x00, 0x00});
        
        // Memory Access Control
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(0x00);
        
        // Interface Pixel Format
        seq.addCommand(0x3A, "Interface Pixel Format");
        seq.addData(0x55);  // 16-bit color
        
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
        CommandSequence seq("R61529 Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("R61529 Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("R61529 Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("R61529 Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("R61529 Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("R61529 Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    // Additional R61529-specific sequences
    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("R61529 Set Rotation");
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
        CommandSequence seq("R61529 Set Window");
        
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

    CommandSequence createSetGammaSequence(const std::vector<uint8_t>& gammaValues) {
        CommandSequence seq("R61529 Set Gamma");
        if (gammaValues.size() == 16) {
            seq.addCommand(0xC1, "Set Blue Gamma");
            seq.addDataList(gammaValues);
            seq.addCommand(0xC2, "Set Green Gamma");
            seq.addDataList(gammaValues);
            seq.addCommand(0xC3, "Set Red Gamma");
            seq.addDataList(gammaValues);
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetDisplayModeSequence(uint8_t mode) {
        CommandSequence seq("R61529 Set Display Mode");
        seq.addCommand(0xB8, "Display Mode Control");
        seq.addData(mode);  // 0x00 = Normal, 0x04 = 8-color, etc.
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetPowerSettingSequence(uint8_t vcix2, uint8_t bt, uint8_t apon, uint8_t pon) {
        CommandSequence seq("R61529 Set Power Setting");
        seq.addCommand(0xB0, "Power Setting");
        seq.addDataList({0x00, 0x00, vcix2, bt, 0x00, apon, pon, 0x00});
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_R61529_H_
