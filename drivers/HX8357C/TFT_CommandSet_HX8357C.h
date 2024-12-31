#ifndef _TFT_COMMANDSET_HX8357C_H_
#define _TFT_COMMANDSET_HX8357C_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_HX8357C : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("HX8357C Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(150);
        
        // Enable extension command
        seq.addCommand(0xB9, "Enable Extension Command");
        seq.addDataList({0xFF, 0x83, 0x57});
        
        // Set VCOM voltage
        seq.addCommand(0xB6, "VCOM Voltage");
        seq.addData(0x2C);
        
        // Set Power Control
        seq.addCommand(0xB3, "Set Power Control");
        seq.addDataList({0x43, 0x00, 0x06, 0x06});
        
        // Set Display Cycle
        seq.addCommand(0xB4, "Set Display Cycle");
        seq.addDataList({0x00, 0x60, 0x00, 0x2A, 0x2A, 0x0D, 0x4F});
        
        // Set BGP
        seq.addCommand(0xB5, "Set BGP");
        seq.addDataList({0x0B, 0x0B, 0x45});
        
        // Set Panel Characteristics
        seq.addCommand(0xC0, "Set Panel Characteristics");
        seq.addDataList({0x24, 0x24, 0x00, 0x3C, 0x00, 0x3C});
        
        // Set Panel Driving
        seq.addCommand(0xC2, "Set Panel Driving");
        seq.addDataList({0x01, 0x03});
        
        // Set Gamma
        seq.addCommand(0xE0, "Set Gamma");
        seq.addDataList({0x00, 0x15, 0x1D, 0x0B, 0x0E, 0x09, 0x48, 0x99,
                        0x38, 0x09, 0x11, 0x03, 0x14, 0x10, 0x00});
        
        // Set Memory Access
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(0x00);
        
        // Set Pixel Format
        seq.addCommand(0x3A, "Interface Pixel Format");
        seq.addData(0x55);  // 16-bit color
        
        // Exit Sleep
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(150);
        
        // Display On
        seq.addCommand(0x29, "Display On");
        seq.addDelay(50);
        
        seq.addEnd();
        return seq;
    }

    CommandSequence createSleepSequence() override {
        CommandSequence seq("HX8357C Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(50);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(150);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("HX8357C Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(150);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(50);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("HX8357C Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(50);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("HX8357C Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(50);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("HX8357C Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("HX8357C Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    // Additional HX8357C-specific sequences
    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("HX8357C Set Rotation");
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
        CommandSequence seq("HX8357C Set Window");
        
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

    CommandSequence createSetPowerControlSequence(uint8_t vcix2, uint8_t vcix2_clk, uint8_t bt) {
        CommandSequence seq("HX8357C Set Power Control");
        seq.addCommand(0xB3, "Set Power Control");
        seq.addDataList({vcix2, vcix2_clk, bt, bt});
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetDisplayCycleSequence(const std::vector<uint8_t>& cycleParams) {
        CommandSequence seq("HX8357C Set Display Cycle");
        if (cycleParams.size() == 7) {
            seq.addCommand(0xB4, "Set Display Cycle");
            seq.addDataList(cycleParams);
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetVCOMSequence(uint8_t vcom) {
        CommandSequence seq("HX8357C Set VCOM");
        seq.addCommand(0xB6, "VCOM Voltage");
        seq.addData(vcom);
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetGammaSequence(const std::vector<uint8_t>& gammaValues) {
        CommandSequence seq("HX8357C Set Gamma");
        if (gammaValues.size() == 15) {
            seq.addCommand(0xE0, "Set Gamma");
            seq.addDataList(gammaValues);
        }
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_HX8357C_H_
