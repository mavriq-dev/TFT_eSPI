#ifndef _TFT_COMMANDSET_ILI9327_H_
#define _TFT_COMMANDSET_ILI9327_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_ILI9327 : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("ILI9327 Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(120);
        
        // Power Control
        seq.addCommand(0xE9, "Set Panel Related");
        seq.addData(0x20);
        
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(50);
        
        seq.addCommand(0xD1, "VCOM Control");
        seq.addDataList({0x00, 0x71, 0x19});
        
        seq.addCommand(0xD0, "Power Setting");
        seq.addDataList({0x07, 0x01, 0x08});
        
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(0x48);
        
        seq.addCommand(0x3A, "Interface Pixel Format");
        seq.addData(0x05);  // 16-bit color
        
        seq.addCommand(0xC1, "Display Timing Setting for Normal Mode");
        seq.addDataList({0x10, 0x10, 0x02, 0x02});
        
        seq.addCommand(0xC0, "Panel Driving Setting");
        seq.addDataList({0x00, 0x35, 0x00, 0x00, 0x01, 0x02});
        
        seq.addCommand(0xC5, "Frame Rate Control");
        seq.addData(0x04);  // 72Hz
        
        // Gamma Setting
        seq.addCommand(0xC8, "Gamma Setting");
        seq.addDataList({0x00, 0x15, 0x00, 0x22, 0x00, 0x08, 0x77, 0x26,
                        0x77, 0x22, 0x04, 0x00});
        
        seq.addCommand(0xF2, "Enable 3G");
        seq.addData(0x02);
        
        seq.addCommand(0x26, "Gamma Set");
        seq.addData(0x01);
        
        // Display On
        seq.addCommand(0x29, "Display ON");
        seq.addDelay(50);
        
        seq.addEnd();
        return seq;
    }

    CommandSequence createSleepSequence() override {
        CommandSequence seq("ILI9327 Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(50);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("ILI9327 Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(50);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("ILI9327 Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(50);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("ILI9327 Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(50);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("ILI9327 Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("ILI9327 Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    // Additional ILI9327-specific sequences
    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("ILI9327 Set Rotation");
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
        CommandSequence seq("ILI9327 Set Window");
        
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
        CommandSequence seq("ILI9327 Set Gamma");
        if (gammaValues.size() == 12) {
            seq.addCommand(0xC8, "Gamma Setting");
            seq.addDataList(gammaValues);
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetVCOMSequence(uint8_t vcom1, uint8_t vcom2, uint8_t vcom3) {
        CommandSequence seq("ILI9327 Set VCOM");
        seq.addCommand(0xD1, "VCOM Control");
        seq.addDataList({vcom1, vcom2, vcom3});
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetPowerSequence(uint8_t vci1, uint8_t vci2, uint8_t vci3) {
        CommandSequence seq("ILI9327 Set Power");
        seq.addCommand(0xD0, "Power Setting");
        seq.addDataList({vci1, vci2, vci3});
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetFrameRateSequence(uint8_t rate) {
        CommandSequence seq("ILI9327 Set Frame Rate");
        seq.addCommand(0xC5, "Frame Rate Control");
        seq.addData(rate);  // Default is 0x04 (72Hz)
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_ILI9327_H_
