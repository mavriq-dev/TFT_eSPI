#ifndef _TFT_COMMANDSET_GC9107_H_
#define _TFT_COMMANDSET_GC9107_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_GC9107 : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("GC9107 Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(120);
        
        // Sleep Out
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(20);
        
        // Memory Data Access Control
        seq.addCommand(0x36, "Memory Data Access Control");
        seq.addData(0x00);
        
        // Interface Pixel Format
        seq.addCommand(0x3A, "Interface Pixel Format");
        seq.addData(0x05);  // 16-bit color
        
        // Display Function Control
        seq.addCommand(0xB6, "Display Function Control");
        seq.addDataList({0x0A, 0x82, 0x27});
        
        // Power Control 1
        seq.addCommand(0xC0, "Power Control 1");
        seq.addDataList({0x0C, 0x0C});
        
        // Power Control 2
        seq.addCommand(0xC1, "Power Control 2");
        seq.addData(0x43);
        
        // VCOM Control
        seq.addCommand(0xC5, "VCOM Control");
        seq.addData(0x2B);
        
        // Frame Rate Control
        seq.addCommand(0xB1, "Frame Rate Control");
        seq.addDataList({0x00, 0x1B});  // 70Hz
        
        // Gamma Setting
        seq.addCommand(0x26, "Gamma Set");
        seq.addData(0x01);
        
        // Positive Gamma Correction
        seq.addCommand(0xE0, "Positive Gamma Correction");
        seq.addDataList({0x0F, 0x24, 0x21, 0x0C, 0x0F, 0x08, 0x43, 0x88,
                        0x32, 0x0F, 0x10, 0x06, 0x0F, 0x07, 0x00});
        
        // Negative Gamma Correction
        seq.addCommand(0xE1, "Negative Gamma Correction");
        seq.addDataList({0x0F, 0x38, 0x30, 0x09, 0x0F, 0x0B, 0x33, 0x37,
                        0x2F, 0x0B, 0x0C, 0x04, 0x22, 0x1B, 0x00});
        
        // Display Inversion Control
        seq.addCommand(0xB4, "Display Inversion Control");
        seq.addData(0x07);
        
        // Display On
        seq.addCommand(0x29, "Display On");
        seq.addDelay(120);
        
        seq.addEnd();
        return seq;
    }

    CommandSequence createSleepSequence() override {
        CommandSequence seq("GC9107 Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("GC9107 Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("GC9107 Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("GC9107 Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("GC9107 Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("GC9107 Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    // Additional GC9107-specific sequences
    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("GC9107 Set Rotation");
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
        CommandSequence seq("GC9107 Set Window");
        
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
        CommandSequence seq("GC9107 Set Frame Rate");
        seq.addCommand(0xB1, "Frame Rate Control");
        seq.addDataList({0x00, rate});  // Default is 0x1B (70Hz)
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetGammaSequence(bool positive, const std::vector<uint8_t>& gammaValues) {
        CommandSequence seq("GC9107 Set Gamma");
        if (gammaValues.size() == 15) {
            seq.addCommand(positive ? 0xE0 : 0xE1, 
                         positive ? "Positive Gamma" : "Negative Gamma");
            seq.addDataList(gammaValues);
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetDisplayInversionSequence(uint8_t mode) {
        CommandSequence seq("GC9107 Set Display Inversion");
        seq.addCommand(0xB4, "Display Inversion Control");
        seq.addData(mode);  // Default is 0x07
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_GC9107_H_
