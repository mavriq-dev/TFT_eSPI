#ifndef _TFT_COMMANDSET_ST7796S_H_
#define _TFT_COMMANDSET_ST7796S_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_ST7796S : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("ST7796S Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(120);
        
        // Command Set Control
        seq.addCommand(0xF0, "Command Set Control");
        seq.addData(0xC3);
        
        seq.addCommand(0xF0, "Command Set Control");
        seq.addData(0x96);
        
        // Power Control 1
        seq.addCommand(0xB1, "Power Control 1");
        seq.addDataList({0x80, 0x44});
        
        // Power Control 2
        seq.addCommand(0xB2, "Power Control 2");
        seq.addDataList({0x02, 0x70});
        
        // Power Control 3
        seq.addCommand(0xB3, "Power Control 3");
        seq.addDataList({0x01, 0x01});
        
        // VCOM Control
        seq.addCommand(0xB4, "VCOM Control");
        seq.addDataList({0x00, 0x11});
        
        // VCOM Control 2
        seq.addCommand(0xB6, "VCOM Control 2");
        seq.addDataList({0x20, 0x02, 0x3B});
        
        // Memory Access Control
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(0x48);
        
        // Interface Pixel Format
        seq.addCommand(0x3A, "Interface Pixel Format");
        seq.addData(0x55);  // 16-bit color
        
        // Display Function Control
        seq.addCommand(0xB6, "Display Function Control");
        seq.addDataList({0x80, 0x02, 0x3B});
        
        // Frame Rate Control
        seq.addCommand(0xE8, "Frame Rate Control");
        seq.addDataList({0x40, 0x8A, 0x00, 0x00, 0x29, 0x19, 0xA5, 0x33});
        
        // Gamma Setting
        seq.addCommand(0xE0, "Positive Gamma Control");
        seq.addDataList({0xF0, 0x09, 0x0B, 0x06, 0x04, 0x15, 0x2F, 0x54,
                        0x42, 0x3C, 0x17, 0x14, 0x18, 0x1B});
        
        seq.addCommand(0xE1, "Negative Gamma Control");
        seq.addDataList({0xE0, 0x09, 0x0B, 0x06, 0x04, 0x03, 0x2B, 0x43,
                        0x42, 0x3B, 0x16, 0x14, 0x17, 0x1B});
        
        // Sleep Out
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        
        // Display On
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        
        seq.addEnd();
        return seq;
    }

    CommandSequence createSleepSequence() override {
        CommandSequence seq("ST7796S Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("ST7796S Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("ST7796S Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("ST7796S Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("ST7796S Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("ST7796S Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("ST7796S Set Rotation");
        uint8_t madctl = 0;
        
        switch (rotation) {
            case 0:  // Portrait
                madctl = 0x48;
                break;
            case 1:  // Landscape
                madctl = 0x28;
                break;
            case 2:  // Portrait inverted
                madctl = 0x88;
                break;
            case 3:  // Landscape inverted
                madctl = 0xE8;
                break;
        }
        
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(madctl);
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetWindowSequence(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
        CommandSequence seq("ST7796S Set Window");
        
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
        CommandSequence seq("ST7796S Set Gamma");
        if (gammaValues.size() == 14) {
            seq.addCommand(positive ? 0xE0 : 0xE1, 
                         positive ? "Positive Gamma" : "Negative Gamma");
            seq.addDataList(gammaValues);
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetVCOMSequence(uint8_t vcom1, uint8_t vcom2) {
        CommandSequence seq("ST7796S Set VCOM");
        seq.addCommand(0xB4, "VCOM Control");
        seq.addDataList({vcom1, vcom2});
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetPowerControlSequence(const std::vector<uint8_t>& powerParams) {
        CommandSequence seq("ST7796S Set Power Control");
        if (powerParams.size() >= 2) {
            seq.addCommand(0xB1, "Power Control 1");
            seq.addDataList(std::vector<uint8_t>(powerParams.begin(), powerParams.begin() + 2));
        }
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_ST7796S_H_
