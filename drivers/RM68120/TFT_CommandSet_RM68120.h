#ifndef _TFT_COMMANDSET_RM68120_H_
#define _TFT_COMMANDSET_RM68120_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_RM68120 : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("RM68120 Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(120);
        
        // Power Setting
        seq.addCommand(0xB0, "Power Control");
        seq.addDataList({0x00, 0x00});
        
        seq.addCommand(0xB1, "VCOM Control");
        seq.addDataList({0x00, 0x00});
        
        seq.addCommand(0xB3, "Power Control 2");
        seq.addDataList({0x02, 0x00, 0x00, 0x00});
        
        // Display Control
        seq.addCommand(0xB4, "Display Mode Control");
        seq.addDataList({0x00});
        
        seq.addCommand(0xB7, "Entry Mode Set");
        seq.addDataList({0x06});
        
        seq.addCommand(0xB8, "Color Enhancement");
        seq.addDataList({0x00, 0x00});
        
        seq.addCommand(0xBA, "Panel Driving Setting");
        seq.addDataList({0x07, 0x00, 0x00});
        
        // Gamma Setting
        seq.addCommand(0xE0, "Positive Gamma Control");
        seq.addDataList({0x00, 0x04, 0x0E, 0x08, 0x17, 0x0A, 0x40, 0x79,
                        0x4D, 0x07, 0x0E, 0x0A, 0x1A, 0x1D, 0x0F});
        
        seq.addCommand(0xE1, "Negative Gamma Control");
        seq.addDataList({0x00, 0x1B, 0x1B, 0x02, 0x0E, 0x04, 0x39, 0x24,
                        0x4B, 0x03, 0x0A, 0x09, 0x30, 0x36, 0x0F});
        
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
        CommandSequence seq("RM68120 Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("RM68120 Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("RM68120 Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("RM68120 Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("RM68120 Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("RM68120 Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("RM68120 Set Rotation");
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
        CommandSequence seq("RM68120 Set Window");
        
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
        CommandSequence seq("RM68120 Set Gamma");
        if (gammaValues.size() == 15) {
            seq.addCommand(positive ? 0xE0 : 0xE1, 
                         positive ? "Positive Gamma" : "Negative Gamma");
            seq.addDataList(gammaValues);
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetVCOMSequence(uint8_t vcom1, uint8_t vcom2) {
        CommandSequence seq("RM68120 Set VCOM");
        seq.addCommand(0xB1, "VCOM Control");
        seq.addDataList({vcom1, vcom2});
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetPowerControlSequence(const std::vector<uint8_t>& powerParams) {
        CommandSequence seq("RM68120 Set Power Control");
        if (powerParams.size() >= 4) {
            seq.addCommand(0xB3, "Power Control 2");
            seq.addDataList(std::vector<uint8_t>(powerParams.begin(), powerParams.begin() + 4));
        }
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_RM68120_H_
