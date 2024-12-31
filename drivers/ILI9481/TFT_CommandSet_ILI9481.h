#ifndef _TFT_COMMANDSET_ILI9481_H_
#define _TFT_COMMANDSET_ILI9481_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_ILI9481 : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("ILI9481 Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(120);
        
        // Exit Sleep Mode
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(20);
        
        // Power Setting
        seq.addCommand(0xD0, "Power Setting");
        seq.addDataList({0x07, 0x42, 0x18});
        
        // VCOM Control
        seq.addCommand(0xD1, "VCOM Control");
        seq.addDataList({0x00, 0x07, 0x10});
        
        // Power Setting for Normal Mode
        seq.addCommand(0xD2, "Power Setting for Normal Mode");
        seq.addDataList({0x01, 0x02});
        
        // Panel Driving Setting
        seq.addCommand(0xC0, "Panel Driving Setting");
        seq.addDataList({0x10, 0x3B, 0x00, 0x02, 0x11});
        
        // Frame Rate and Inversion Control
        seq.addCommand(0xC5, "Frame Rate and Inversion Control");
        seq.addData(0x03);  // 72Hz
        
        // Interface Control
        seq.addCommand(0xC6, "Interface Control");
        seq.addData(0x83);
        
        // Gamma Setting
        seq.addCommand(0xC8, "Gamma Setting");
        seq.addDataList({0x00, 0x32, 0x36, 0x45, 0x06, 0x16,
                        0x37, 0x75, 0x77, 0x54, 0x0C, 0x00});
        
        // Memory Access Control
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(0x0A);
        
        // Set Pixel Format
        seq.addCommand(0x3A, "Set Pixel Format");
        seq.addData(0x55);  // 16-bit color
        
        // Display On
        seq.addCommand(0x29, "Display On");
        seq.addDelay(120);
        
        seq.addEnd();
        return seq;
    }

    CommandSequence createSleepSequence() override {
        CommandSequence seq("ILI9481 Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("ILI9481 Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("ILI9481 Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("ILI9481 Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("ILI9481 Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("ILI9481 Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    // Additional ILI9481-specific sequences
    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("ILI9481 Set Rotation");
        uint8_t madctl = 0;
        
        switch (rotation) {
            case 0:  // Portrait
                madctl = 0x0A;
                break;
            case 1:  // Landscape
                madctl = 0x28;
                break;
            case 2:  // Portrait inverted
                madctl = 0x09;
                break;
            case 3:  // Landscape inverted
                madctl = 0x2B;
                break;
        }
        
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(madctl);
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetWindowSequence(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
        CommandSequence seq("ILI9481 Set Window");
        
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

    CommandSequence createSetFrameRateSequence(uint8_t rate) {
        CommandSequence seq("ILI9481 Set Frame Rate");
        seq.addCommand(0xC5, "Frame Rate Control");
        seq.addData(rate);  // 0x03 = 72Hz (default)
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetGammaSequence(const std::vector<uint8_t>& gammaValues) {
        CommandSequence seq("ILI9481 Set Gamma");
        if (gammaValues.size() == 12) {
            seq.addCommand(0xC8, "Gamma Setting");
            seq.addDataList(gammaValues);
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetPowerControlSequence(uint8_t vc, uint8_t bl, uint8_t dc) {
        CommandSequence seq("ILI9481 Set Power Control");
        seq.addCommand(0xD0, "Power Setting");
        seq.addDataList({vc, bl, dc});
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetVCOMControlSequence(uint8_t vcomh, uint8_t vcoml) {
        CommandSequence seq("ILI9481 Set VCOM Control");
        seq.addCommand(0xD1, "VCOM Control");
        seq.addDataList({0x00, vcomh, vcoml});
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_ILI9481_H_
