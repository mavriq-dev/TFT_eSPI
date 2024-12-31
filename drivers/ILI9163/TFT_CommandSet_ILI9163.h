#ifndef _TFT_COMMANDSET_ILI9163_H_
#define _TFT_COMMANDSET_ILI9163_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_ILI9163 : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("ILI9163 Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(150);
        
        // Exit Sleep
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(50);
        
        // Frame Rate Control
        seq.addCommand(0xB1, "Frame Rate Control");
        seq.addDataList({0x08, 0x08});
        
        // Display Function Control
        seq.addCommand(0xB6, "Display Function Control");
        seq.addDataList({0x00, 0x82, 0x27});
        
        // Power Control 1
        seq.addCommand(0xC0, "Power Control 1");
        seq.addDataList({0x0C, 0x0C});
        
        // Power Control 2
        seq.addCommand(0xC1, "Power Control 2");
        seq.addData(0x03);
        
        // VCOM Control 1
        seq.addCommand(0xC5, "VCOM Control 1");
        seq.addDataList({0x50, 0x5B});
        
        // Set Gamma
        seq.addCommand(0x26, "Gamma Set");
        seq.addData(0x01);
        
        // Positive Gamma Correction
        seq.addCommand(0xE0, "Positive Gamma Correction");
        seq.addDataList({0x0F, 0x1B, 0x18, 0x0B, 0x0E, 0x09, 0x47, 0x94,
                        0x35, 0x0A, 0x13, 0x05, 0x08, 0x03, 0x00});
        
        // Negative Gamma Correction
        seq.addCommand(0xE1, "Negative Gamma Correction");
        seq.addDataList({0x00, 0x24, 0x27, 0x04, 0x11, 0x06, 0x38, 0x6B,
                        0x4A, 0x05, 0x0C, 0x0A, 0x37, 0x3C, 0x0F});
        
        // Memory Access Control
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(0x00);
        
        // Pixel Format
        seq.addCommand(0x3A, "Interface Pixel Format");
        seq.addData(0x05);  // 16-bit color
        
        // Display On
        seq.addCommand(0x29, "Display On");
        seq.addDelay(50);
        
        seq.addEnd();
        return seq;
    }

    CommandSequence createSleepSequence() override {
        CommandSequence seq("ILI9163 Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(50);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(150);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("ILI9163 Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(150);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(50);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("ILI9163 Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(50);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("ILI9163 Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(50);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("ILI9163 Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("ILI9163 Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    // Additional ILI9163-specific sequences
    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("ILI9163 Set Rotation");
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
        CommandSequence seq("ILI9163 Set Window");
        
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
        CommandSequence seq("ILI9163 Set Frame Rate");
        seq.addCommand(0xB1, "Frame Rate Control");
        seq.addDataList({rate, rate});  // Default is 0x08 (119Hz)
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetVCOMSequence(uint8_t vcom1, uint8_t vcom2) {
        CommandSequence seq("ILI9163 Set VCOM");
        seq.addCommand(0xC5, "VCOM Control 1");
        seq.addDataList({vcom1, vcom2});
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetGammaSequence(bool positive, const std::vector<uint8_t>& gammaValues) {
        CommandSequence seq("ILI9163 Set Gamma");
        if (gammaValues.size() == 15) {
            seq.addCommand(positive ? 0xE0 : 0xE1, 
                         positive ? "Positive Gamma" : "Negative Gamma");
            seq.addDataList(gammaValues);
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetPowerControlSequence(uint8_t vc, uint8_t vci1, uint8_t vci2) {
        CommandSequence seq("ILI9163 Set Power Control");
        seq.addCommand(0xC0, "Power Control 1");
        seq.addDataList({vc, vc});
        seq.addCommand(0xC1, "Power Control 2");
        seq.addData(vci1);
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_ILI9163_H_
