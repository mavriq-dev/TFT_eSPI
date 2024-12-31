#ifndef _TFT_COMMANDSET_HX8369A_H_
#define _TFT_COMMANDSET_HX8369A_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_HX8369A : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("HX8369A Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(150);
        
        // Set Password
        seq.addCommand(0xB9, "Set Password");
        seq.addDataList({0xFF, 0x83, 0x69});
        
        // Set Power
        seq.addCommand(0xB1, "Set Power");
        seq.addDataList({0x01, 0x00, 0x34, 0x07, 0x00, 0x0E, 0x0E, 0x1A,
                        0x22, 0x3F, 0x3F, 0x01, 0x23, 0x01, 0xE6, 0xE6,
                        0xE6, 0xE6, 0xE6});
        
        // Set Display
        seq.addCommand(0xB2, "Set Display");
        seq.addDataList({0x00, 0x20, 0x0A, 0x0A, 0x70, 0x00, 0xFF, 0x00,
                        0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x01});
        
        // Set Display Cycle
        seq.addCommand(0xB4, "Set Display Cycle");
        seq.addDataList({0x00, 0x0C, 0x80, 0x0C, 0x01, 0x00, 0x00, 0x37});
        
        // Set VCOM
        seq.addCommand(0xB6, "Set VCOM");
        seq.addDataList({0x00, 0x00, 0x20, 0x20, 0x00, 0x20});
        
        // Set Panel
        seq.addCommand(0xCC, "Set Panel");
        seq.addData(0x02);
        
        // Set Gamma
        seq.addCommand(0xE0, "Set Gamma");
        seq.addDataList({0x00, 0x15, 0x1D, 0x0B, 0x0E, 0x09, 0x48, 0x99,
                        0x38, 0x09, 0x11, 0x03, 0x14, 0x10, 0x00});
        
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
        CommandSequence seq("HX8369A Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(50);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(150);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("HX8369A Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(150);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(50);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("HX8369A Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(50);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("HX8369A Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(50);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("HX8369A Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("HX8369A Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    // Additional HX8369A-specific sequences
    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("HX8369A Set Rotation");
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
        CommandSequence seq("HX8369A Set Window");
        
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

    CommandSequence createSetPowerSequence(const std::vector<uint8_t>& powerParams) {
        CommandSequence seq("HX8369A Set Power");
        if (powerParams.size() == 19) {
            seq.addCommand(0xB1, "Set Power");
            seq.addDataList(powerParams);
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetDisplaySequence(const std::vector<uint8_t>& displayParams) {
        CommandSequence seq("HX8369A Set Display");
        if (displayParams.size() == 15) {
            seq.addCommand(0xB2, "Set Display");
            seq.addDataList(displayParams);
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetVCOMSequence(const std::vector<uint8_t>& vcomParams) {
        CommandSequence seq("HX8369A Set VCOM");
        if (vcomParams.size() == 6) {
            seq.addCommand(0xB6, "Set VCOM");
            seq.addDataList(vcomParams);
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetGammaSequence(const std::vector<uint8_t>& gammaValues) {
        CommandSequence seq("HX8369A Set Gamma");
        if (gammaValues.size() == 15) {
            seq.addCommand(0xE0, "Set Gamma");
            seq.addDataList(gammaValues);
        }
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_HX8369A_H_
