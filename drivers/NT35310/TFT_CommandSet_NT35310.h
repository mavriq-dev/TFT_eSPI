#ifndef _TFT_COMMANDSET_NT35310_H_
#define _TFT_COMMANDSET_NT35310_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_NT35310 : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("NT35310 Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(120);
        
        // Power Control A
        seq.addCommand(0xF0, "Command Set Enable");
        seq.addDataList({0x5A, 0x5A});
        
        // Power Control
        seq.addCommand(0xC3, "Power Control");
        seq.addDataList({0x40, 0x00, 0x28});
        
        seq.addCommand(0xC4, "Power Control 2");
        seq.addDataList({0x01, 0x02, 0x00, 0x00});
        
        seq.addCommand(0xC5, "VCOM Control");
        seq.addDataList({0x1C, 0x1C});
        
        seq.addCommand(0xC6, "VCOM Offset");
        seq.addData(0x1C);
        
        // Display Control
        seq.addCommand(0xF2, "Display Control");
        seq.addDataList({0x02, 0x06, 0x0A, 0x02});
        
        seq.addCommand(0xF3, "Gamma Control");
        seq.addDataList({0x10, 0x10, 0x10, 0x10});
        
        seq.addCommand(0xF4, "Panel Timing Control");
        seq.addDataList({0x0A, 0x16, 0x0A, 0x16});
        
        seq.addCommand(0xF5, "Source Timing Control");
        seq.addDataList({0x16, 0x16, 0x16, 0x16});
        
        // Gamma Settings
        seq.addCommand(0xE0, "Positive Gamma");
        seq.addDataList({0x00, 0x04, 0x0E, 0x0C, 0x0F, 0x0C, 0x0B, 0x09,
                        0x04, 0x07, 0x0D, 0x07, 0x0E, 0x15, 0x0F, 0x00});
        
        seq.addCommand(0xE1, "Negative Gamma");
        seq.addDataList({0x00, 0x0C, 0x11, 0x0C, 0x0F, 0x0C, 0x0B, 0x09,
                        0x04, 0x07, 0x0D, 0x07, 0x0E, 0x15, 0x0F, 0x00});
        
        // Memory Access Control
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(0x08);
        
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
        CommandSequence seq("NT35310 Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("NT35310 Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("NT35310 Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("NT35310 Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("NT35310 Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("NT35310 Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("NT35310 Set Rotation");
        uint8_t madctl = 0;
        
        switch (rotation) {
            case 0:  // Portrait
                madctl = 0x08;
                break;
            case 1:  // Landscape
                madctl = 0x68;
                break;
            case 2:  // Portrait inverted
                madctl = 0xC8;
                break;
            case 3:  // Landscape inverted
                madctl = 0xA8;
                break;
        }
        
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(madctl);
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetWindowSequence(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
        CommandSequence seq("NT35310 Set Window");
        
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
        CommandSequence seq("NT35310 Set Gamma");
        if (gammaValues.size() == 16) {
            seq.addCommand(positive ? 0xE0 : 0xE1, 
                         positive ? "Positive Gamma" : "Negative Gamma");
            seq.addDataList(gammaValues);
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetVCOMSequence(uint8_t vcom1, uint8_t vcom2) {
        CommandSequence seq("NT35310 Set VCOM");
        seq.addCommand(0xC5, "VCOM Control");
        seq.addDataList({vcom1, vcom2});
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetPowerControlSequence(const std::vector<uint8_t>& powerParams) {
        CommandSequence seq("NT35310 Set Power Control");
        if (powerParams.size() >= 3) {
            seq.addCommand(0xC3, "Power Control");
            seq.addDataList(std::vector<uint8_t>(powerParams.begin(), powerParams.begin() + 3));
        }
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_NT35310_H_
