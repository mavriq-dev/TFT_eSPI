#ifndef _TFT_COMMANDSET_S6D02A1_H_
#define _TFT_COMMANDSET_S6D02A1_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_S6D02A1 : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("S6D02A1 Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(120);
        
        // Power Control
        seq.addCommand(0xF2, "Power Control 3");
        seq.addDataList({0x1C, 0x1C, 0x0C, 0x0C});
        
        seq.addCommand(0xF3, "Power Control 4");
        seq.addDataList({0x08, 0x32, 0x00, 0x08});
        
        seq.addCommand(0xF4, "Power Control 5");
        seq.addDataList({0x8F, 0x20});
        
        seq.addCommand(0xF5, "VCOM Control");
        seq.addDataList({0x40, 0x50});
        
        // Display Control
        seq.addCommand(0xF6, "Source Control");
        seq.addDataList({0x04, 0x00, 0x08, 0x03, 0x01});
        
        seq.addCommand(0xF7, "Gate Control");
        seq.addDataList({0x04, 0x02});
        
        seq.addCommand(0xF8, "Display Control 1");
        seq.addDataList({0x11, 0x00});
        
        seq.addCommand(0xF9, "Display Control 2");
        seq.addDataList({0x08, 0x08, 0x08, 0x08});
        
        // Gamma Settings
        seq.addCommand(0xFA, "Positive Gamma Control");
        seq.addDataList({0x0B, 0x0B, 0x0B, 0x17, 0x1F, 0x25, 0x2B, 0x31,
                        0x36, 0x3B, 0x41, 0x47, 0x4D, 0x53, 0x59, 0x5F});
        
        seq.addCommand(0xFB, "Negative Gamma Control");
        seq.addDataList({0x0B, 0x0B, 0x0B, 0x17, 0x1F, 0x25, 0x2B, 0x31,
                        0x36, 0x3B, 0x41, 0x47, 0x4D, 0x53, 0x59, 0x5F});
        
        // Memory Access Control
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(0x00);
        
        // Interface Pixel Format
        seq.addCommand(0x3A, "Interface Pixel Format");
        seq.addData(0x55);  // 16-bit color
        
        // Exit Sleep Mode
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        
        // Display On
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        
        seq.addEnd();
        return seq;
    }

    CommandSequence createSleepSequence() override {
        CommandSequence seq("S6D02A1 Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("S6D02A1 Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("S6D02A1 Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("S6D02A1 Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("S6D02A1 Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("S6D02A1 Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("S6D02A1 Set Rotation");
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
        CommandSequence seq("S6D02A1 Set Window");
        
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

    CommandSequence createSetGammaSequence(bool positive, const std::vector<uint8_t>& gammaValues) {
        CommandSequence seq("S6D02A1 Set Gamma");
        if (gammaValues.size() == 16) {
            seq.addCommand(positive ? 0xFA : 0xFB, 
                         positive ? "Positive Gamma" : "Negative Gamma");
            seq.addDataList(gammaValues);
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetVCOMSequence(uint8_t vcom1, uint8_t vcom2) {
        CommandSequence seq("S6D02A1 Set VCOM");
        seq.addCommand(0xF5, "VCOM Control");
        seq.addDataList({vcom1, vcom2});
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetPowerControlSequence(const std::vector<uint8_t>& powerParams) {
        CommandSequence seq("S6D02A1 Set Power Control");
        if (powerParams.size() >= 4) {
            seq.addCommand(0xF2, "Power Control 3");
            seq.addDataList(std::vector<uint8_t>(powerParams.begin(), powerParams.begin() + 4));
        }
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_S6D02A1_H_
