#ifndef _TFT_COMMANDSET_HX8357B_H_
#define _TFT_COMMANDSET_HX8357B_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_HX8357B : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("HX8357B Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(150);
        
        // Set Power
        seq.addCommand(0xB0, "Set Power");
        seq.addDataList({0x00, 0x04, 0x0C, 0x0C, 0x04});
        
        // Set Display Control
        seq.addCommand(0xB4, "Set Display Control");
        seq.addDataList({0x00, 0x40, 0x00, 0x2A, 0x2A, 0x0D, 0x4F});
        
        // Set VCOM Voltage
        seq.addCommand(0xB6, "Set VCOM Voltage");
        seq.addData(0x02);
        
        // Set Internal Oscillator
        seq.addCommand(0xB7, "Set Internal Oscillator");
        seq.addData(0x02);
        
        // Set RGB Interface
        seq.addCommand(0xB8, "Set RGB Interface");
        seq.addData(0x0C);
        
        // Set Panel Characteristics
        seq.addCommand(0xBA, "Set Panel Characteristics");
        seq.addData(0x07);
        
        // Set Panel Drive
        seq.addCommand(0xBD, "Set Panel Drive");
        seq.addDataList({0x00, 0x07});
        
        // Set Gamma
        seq.addCommand(0xE0, "Set Gamma");
        seq.addDataList({0x00, 0x15, 0x1D, 0x0B, 0x0E, 0x09, 0x48, 0x99,
                        0x38, 0x09, 0x11, 0x03, 0x14, 0x10, 0x00});
        
        // Set Image Function
        seq.addCommand(0xF2, "Set Image Function");
        seq.addData(0x18);
        
        // Set Memory Access
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(0x00);
        
        // Set Pixel Format
        seq.addCommand(0x3A, "Interface Pixel Format");
        seq.addData(0x55);  // 16-bit color
        
        // Exit Sleep
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(150);
        
        // Display On
        seq.addCommand(0x29, "Display On");
        seq.addDelay(50);
        
        seq.addEnd();
        return seq;
    }

    CommandSequence createSleepSequence() override {
        CommandSequence seq("HX8357B Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(50);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(150);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("HX8357B Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(150);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(50);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("HX8357B Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(50);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("HX8357B Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(50);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("HX8357B Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("HX8357B Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    // Additional HX8357B-specific sequences
    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("HX8357B Set Rotation");
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
        CommandSequence seq("HX8357B Set Window");
        
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

    CommandSequence createSetPowerSequence(uint8_t mode) {
        CommandSequence seq("HX8357B Set Power");
        seq.addCommand(0xB0, "Set Power");
        switch (mode) {
            case 0:  // Normal mode
                seq.addDataList({0x00, 0x04, 0x0C, 0x0C, 0x04});
                break;
            case 1:  // Low power mode
                seq.addDataList({0x00, 0x02, 0x08, 0x08, 0x02});
                break;
            default:
                seq.addDataList({0x00, 0x04, 0x0C, 0x0C, 0x04});
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetGammaSequence(const std::vector<uint8_t>& gammaValues) {
        CommandSequence seq("HX8357B Set Gamma");
        if (gammaValues.size() == 15) {
            seq.addCommand(0xE0, "Set Gamma");
            seq.addDataList(gammaValues);
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetVCOMSequence(uint8_t vcom) {
        CommandSequence seq("HX8357B Set VCOM");
        seq.addCommand(0xB6, "Set VCOM Voltage");
        seq.addData(vcom);
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_HX8357B_H_
