#ifndef _TFT_COMMANDSET_R61581_H_
#define _TFT_COMMANDSET_R61581_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_R61581 : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("R61581 Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(120);
        
        // Power Setting
        seq.addCommand(0xB0, "Power Setting");
        seq.addDataList({0x00});
        
        seq.addCommand(0xB3, "Power Control");
        seq.addDataList({0x02, 0x00, 0x00, 0x00});
        
        seq.addCommand(0xB4, "VCOM Control");
        seq.addDataList({0x00});
        
        // Display Control
        seq.addCommand(0xC0, "Panel Driving Setting");
        seq.addDataList({0x03, 0x3B, 0x00, 0x02, 0x11});
        
        seq.addCommand(0xC1, "Display Timing Setting for Normal Mode");
        seq.addDataList({0x10, 0x10, 0x88});
        
        seq.addCommand(0xC4, "Source/Gate Driving Timing Setting");
        seq.addDataList({0x01, 0x01, 0x10, 0x10});
        
        seq.addCommand(0xC5, "Gate-On Pulse Width Setting");
        seq.addDataList({0x03});
        
        seq.addCommand(0xC6, "Non-Display Area Setting");
        seq.addDataList({0x02});
        
        seq.addCommand(0xC8, "Gamma Setting");
        seq.addDataList({0x00, 0x32, 0x36, 0x45, 0x06, 0x16, 0x37, 0x75,
                        0x77, 0x54, 0x0C, 0x00});
        
        // Memory Access Control
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(0x0A);
        
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
        CommandSequence seq("R61581 Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("R61581 Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("R61581 Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("R61581 Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("R61581 Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("R61581 Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("R61581 Set Rotation");
        uint8_t madctl = 0;
        
        switch (rotation) {
            case 0:  // Portrait
                madctl = 0x0A;
                break;
            case 1:  // Landscape
                madctl = 0x6A;
                break;
            case 2:  // Portrait inverted
                madctl = 0xCA;
                break;
            case 3:  // Landscape inverted
                madctl = 0xAA;
                break;
        }
        
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(madctl);
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetWindowSequence(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
        CommandSequence seq("R61581 Set Window");
        
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
        CommandSequence seq("R61581 Set Gamma");
        if (gammaValues.size() == 12) {
            seq.addCommand(0xC8, "Gamma Setting");
            seq.addDataList(gammaValues);
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetVCOMSequence(uint8_t vcom) {
        CommandSequence seq("R61581 Set VCOM");
        seq.addCommand(0xB4, "VCOM Control");
        seq.addData(vcom);
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetPowerControlSequence(const std::vector<uint8_t>& powerParams) {
        CommandSequence seq("R61581 Set Power Control");
        if (powerParams.size() >= 4) {
            seq.addCommand(0xB3, "Power Control");
            seq.addDataList(std::vector<uint8_t>(powerParams.begin(), powerParams.begin() + 4));
        }
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_R61581_H_
