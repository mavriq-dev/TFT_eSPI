#ifndef _TFT_COMMANDSET_ILI9881C_H_
#define _TFT_COMMANDSET_ILI9881C_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_ILI9881C : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("ILI9881C Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(120);
        
        // Command 2 Enable
        seq.addCommand(0xFF, "Command2 Enable");
        seq.addDataList({0x98, 0x81, 0x03});
        
        // GIP 1
        seq.addCommand(0x01, "GIP 1");
        seq.addDataList({0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
        
        seq.addCommand(0x02, "GIP 2");
        seq.addDataList({0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
        
        seq.addCommand(0x03, "GIP 3");
        seq.addDataList({0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
        
        // Power Control
        seq.addCommand(0xB0, "Power Control");
        seq.addDataList({0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
        
        seq.addCommand(0xB1, "VCOM");
        seq.addDataList({0x00, 0x00});
        
        seq.addCommand(0xB2, "VGH Voltage");
        seq.addDataList({0x00, 0x00});
        
        // Panel Driving Setting
        seq.addCommand(0xB3, "Panel Driving Setting");
        seq.addDataList({0x00, 0x00, 0x00, 0x00});
        
        // Display Inversion Control
        seq.addCommand(0xB4, "Display Inversion Control");
        seq.addDataList({0x00});
        
        // GIP Setting 1
        seq.addCommand(0xC0, "GIP Setting 1");
        seq.addDataList({0x00, 0x00, 0x00, 0x00});
        
        // GIP Setting 2
        seq.addCommand(0xC1, "GIP Setting 2");
        seq.addDataList({0x00, 0x00, 0x00, 0x00});
        
        // Power Control 1
        seq.addCommand(0xD0, "Power Control 1");
        seq.addDataList({0x00, 0x00, 0x00});
        
        // Power Control 2
        seq.addCommand(0xD1, "Power Control 2");
        seq.addDataList({0x00, 0x00, 0x00});
        
        // VCOM Control
        seq.addCommand(0xD3, "VCOM Control");
        seq.addDataList({0x00, 0x00});
        
        // Exit Command 2
        seq.addCommand(0xFF, "Exit Command2");
        seq.addDataList({0x98, 0x81, 0x04});
        
        // Source Timing Setting
        seq.addCommand(0x00, "Source Timing Setting");
        seq.addDataList({0x00});
        
        // MIPI Control
        seq.addCommand(0xFF, "MIPI Control");
        seq.addDataList({0x98, 0x81, 0x01});
        
        // Display Mode Control
        seq.addCommand(0x20, "Display Mode Control");
        seq.addDataList({0x00});
        
        // Power Control
        seq.addCommand(0x24, "Power Control");
        seq.addDataList({0x00});
        
        // VCOM
        seq.addCommand(0x25, "VCOM");
        seq.addDataList({0x00});
        
        // Exit Command Mode
        seq.addCommand(0xFF, "Exit Command Mode");
        seq.addDataList({0x98, 0x81, 0x00});
        
        // Sleep Out
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        
        // Memory Access Control
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(0x00);
        
        // Interface Pixel Format
        seq.addCommand(0x3A, "Interface Pixel Format");
        seq.addData(0x55);  // 16-bit color
        
        // Display On
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        
        seq.addEnd();
        return seq;
    }

    CommandSequence createSleepSequence() override {
        CommandSequence seq("ILI9881C Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("ILI9881C Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("ILI9881C Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("ILI9881C Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("ILI9881C Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("ILI9881C Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("ILI9881C Set Rotation");
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
        CommandSequence seq("ILI9881C Set Window");
        
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
        CommandSequence seq("ILI9881C Set Gamma");
        // Enter Command 2
        seq.addCommand(0xFF, "Enter Command2");
        seq.addDataList({0x98, 0x81, 0x03});
        
        if (gammaValues.size() >= 16) {
            seq.addCommand(0xE0, "Gamma Setting");
            seq.addDataList(std::vector<uint8_t>(gammaValues.begin(), gammaValues.begin() + 16));
        }
        
        // Exit Command 2
        seq.addCommand(0xFF, "Exit Command2");
        seq.addDataList({0x98, 0x81, 0x00});
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetVCOMSequence(uint8_t vcom1, uint8_t vcom2) {
        CommandSequence seq("ILI9881C Set VCOM");
        // Enter Command 2
        seq.addCommand(0xFF, "Enter Command2");
        seq.addDataList({0x98, 0x81, 0x03});
        
        seq.addCommand(0xB1, "VCOM Setting");
        seq.addDataList({vcom1, vcom2});
        
        // Exit Command 2
        seq.addCommand(0xFF, "Exit Command2");
        seq.addDataList({0x98, 0x81, 0x00});
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetPowerControlSequence(const std::vector<uint8_t>& powerParams) {
        CommandSequence seq("ILI9881C Set Power Control");
        // Enter Command 2
        seq.addCommand(0xFF, "Enter Command2");
        seq.addDataList({0x98, 0x81, 0x03});
        
        if (powerParams.size() >= 6) {
            seq.addCommand(0xB0, "Power Control");
            seq.addDataList(std::vector<uint8_t>(powerParams.begin(), powerParams.begin() + 6));
        }
        
        // Exit Command 2
        seq.addCommand(0xFF, "Exit Command2");
        seq.addDataList({0x98, 0x81, 0x00});
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_ILI9881C_H_
