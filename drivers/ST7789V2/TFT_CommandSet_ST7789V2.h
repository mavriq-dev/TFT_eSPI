#ifndef _TFT_COMMANDSET_ST7789V2_H_
#define _TFT_COMMANDSET_ST7789V2_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_ST7789V2 : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("ST7789V2 Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(120);
        
        // Sleep Out
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        
        // Memory Data Access Control
        seq.addCommand(0x36, "Memory Data Access Control");
        seq.addData(0x00);
        
        // Interface Pixel Format
        seq.addCommand(0x3A, "Interface Pixel Format");
        seq.addData(0x55);  // 16-bit color
        
        // PORCH Setting
        seq.addCommand(0xB2, "Porch Setting");
        seq.addDataList({0x0C, 0x0C, 0x00, 0x33, 0x33});
        
        // Gate Control
        seq.addCommand(0xB7, "Gate Control");
        seq.addData(0x35);
        
        // VCOM Setting
        seq.addCommand(0xBB, "VCOM Setting");
        seq.addData(0x2B);
        
        // LCM Control
        seq.addCommand(0xC0, "LCM Control");
        seq.addData(0x2C);
        
        // VDV and VRH Command Enable
        seq.addCommand(0xC2, "VDV and VRH Command Enable");
        seq.addDataList({0x01, 0xFF});
        
        // VRH Set
        seq.addCommand(0xC3, "VRH Set");
        seq.addData(0x11);
        
        // VDV Set
        seq.addCommand(0xC4, "VDV Set");
        seq.addData(0x20);
        
        // Frame Rate Control in Normal Mode
        seq.addCommand(0xC6, "Frame Rate Control");
        seq.addData(0x0F);
        
        // Power Control 1
        seq.addCommand(0xD0, "Power Control 1");
        seq.addDataList({0xA4, 0xA1});
        
        // Positive Voltage Gamma Control
        seq.addCommand(0xE0, "Positive Voltage Gamma Control");
        seq.addDataList({0xD0, 0x00, 0x05, 0x0E, 0x15, 0x0D, 0x37, 0x43,
                        0x47, 0x09, 0x15, 0x12, 0x16, 0x19});
        
        // Negative Voltage Gamma Control
        seq.addCommand(0xE1, "Negative Voltage Gamma Control");
        seq.addDataList({0xD0, 0x00, 0x05, 0x0D, 0x0C, 0x06, 0x2D, 0x44,
                        0x40, 0x0E, 0x1C, 0x18, 0x16, 0x19});
        
        // Display Inversion On
        seq.addCommand(0x21, "Display Inversion On");
        
        // Display On
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        
        seq.addEnd();
        return seq;
    }

    CommandSequence createSleepSequence() override {
        CommandSequence seq("ST7789V2 Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("ST7789V2 Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("ST7789V2 Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("ST7789V2 Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("ST7789V2 Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("ST7789V2 Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("ST7789V2 Set Rotation");
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
        CommandSequence seq("ST7789V2 Set Window");
        
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
        CommandSequence seq("ST7789V2 Set Gamma");
        if (gammaValues.size() == 14) {
            seq.addCommand(positive ? 0xE0 : 0xE1, 
                         positive ? "Positive Gamma" : "Negative Gamma");
            seq.addDataList(gammaValues);
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetVCOMSequence(uint8_t vcom) {
        CommandSequence seq("ST7789V2 Set VCOM");
        seq.addCommand(0xBB, "VCOM Setting");
        seq.addData(vcom);
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetPowerControlSequence(const std::vector<uint8_t>& powerParams) {
        CommandSequence seq("ST7789V2 Set Power Control");
        if (powerParams.size() >= 2) {
            seq.addCommand(0xD0, "Power Control 1");
            seq.addDataList(std::vector<uint8_t>(powerParams.begin(), powerParams.begin() + 2));
        }
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_ST7789V2_H_
