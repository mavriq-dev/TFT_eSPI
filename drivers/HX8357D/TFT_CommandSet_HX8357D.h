#ifndef _TFT_COMMANDSET_HX8357D_H_
#define _TFT_COMMANDSET_HX8357D_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_HX8357D : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("HX8357D Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(150);
        
        // Set Power Control
        seq.addCommand(0xB1, "Power Control");
        seq.addDataList({0x00, 0x14, 0x18, 0x18, 0x0C, 0x00});
        
        // Set Display Control
        seq.addCommand(0xB2, "Display Control");
        seq.addDataList({0x00, 0x00});
        
        // Set RGB Interface
        seq.addCommand(0xB3, "RGB Interface");
        seq.addDataList({0x00, 0x00, 0x02, 0x00});
        
        // Set CYC Control
        seq.addCommand(0xB4, "CYC Control");
        seq.addDataList({0x00, 0x14, 0x18, 0x18, 0x0C, 0x00});
        
        // Set VCOM Control
        seq.addCommand(0xB6, "VCOM Control");
        seq.addDataList({0x00, 0x02, 0x3C, 0x38, 0x00, 0x00});
        
        // Set Internal OSC
        seq.addCommand(0xB9, "Internal OSC");
        seq.addDataList({0x01, 0x00});
        
        // Set Panel Driving
        seq.addCommand(0xC0, "Panel Driving");
        seq.addDataList({0x10, 0x3B, 0x00, 0x02, 0x11});
        
        // Set Frame Rate
        seq.addCommand(0xC1, "Frame Rate");
        seq.addData(0x10);
        
        // Set Interface Control
        seq.addCommand(0xC5, "Interface Control");
        seq.addDataList({0x00, 0x0A, 0x08});
        
        // Set Gamma
        seq.addCommand(0xC8, "Gamma Setting");
        seq.addDataList({0x00, 0x32, 0x36, 0x45, 0x06, 0x16,
                        0x37, 0x75, 0x77, 0x54, 0x0C, 0x00});
        
        // Set Panel Characteristics
        seq.addCommand(0xCC, "Panel Characteristics");
        seq.addData(0x00);
        
        // Set Power Control
        seq.addCommand(0xD0, "Power Control");
        seq.addDataList({0x44, 0x42, 0x06});
        
        // Set VCOM Control
        seq.addCommand(0xD1, "VCOM Control");
        seq.addDataList({0x43, 0x16});
        
        // Set Power Control
        seq.addCommand(0xD2, "Power Control");
        seq.addDataList({0x04, 0x22});
        
        // Set Power Control
        seq.addCommand(0xD3, "Power Control");
        seq.addDataList({0x04, 0x12});
        
        // Set VCOM Control
        seq.addCommand(0xD4, "VCOM Control");
        seq.addDataList({0x07, 0x12});
        
        // Set RGB Interface Control
        seq.addCommand(0xE9, "RGB Interface Control");
        seq.addData(0x00);
        
        // Set Color Mode - 16 bit per pixel
        seq.addCommand(0x3A, "Interface Pixel Format");
        seq.addData(0x55);    // 16-bit color
        
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
        CommandSequence seq("HX8357D Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("HX8357D Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("HX8357D Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("HX8357D Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("HX8357D Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("HX8357D Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    // Additional HX8357D-specific sequences
    CommandSequence createSetBrightnessSequence(uint8_t brightness) {
        CommandSequence seq("HX8357D Set Brightness");
        seq.addCommand(0x51, "Write Display Brightness");
        seq.addData(brightness);
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetTearingEffectSequence(bool enable, bool vblank = true) {
        CommandSequence seq("HX8357D Set Tearing Effect");
        if (enable) {
            seq.addCommand(0x35, "Tearing Effect Line ON");
            seq.addData(vblank ? 0x00 : 0x01);  // 0x00 for V-Blanking, 0x01 for both V and H
        } else {
            seq.addCommand(0x34, "Tearing Effect Line OFF");
        }
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_HX8357D_H_
