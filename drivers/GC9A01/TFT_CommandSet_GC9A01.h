#ifndef _TFT_COMMANDSET_GC9A01_H_
#define _TFT_COMMANDSET_GC9A01_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_GC9A01 : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("GC9A01 Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(120);
        
        // Enable Command 2 Part I
        seq.addCommand(0xFE, "Enable Command 2 Part I");
        seq.addCommand(0xEF, "Enable Command 2 Part II");
        
        // Internal Configuration
        seq.addCommand(0xEB, "Internal Configuration");
        seq.addData(0x14);
        
        seq.addCommand(0x84, "Internal Settings");
        seq.addData(0x40);
        
        seq.addCommand(0x85, "Internal Settings");
        seq.addData(0xFF);
        
        seq.addCommand(0x86, "Internal Settings");
        seq.addData(0xFF);
        
        seq.addCommand(0x87, "Internal Settings");
        seq.addData(0xFF);
        
        seq.addCommand(0x88, "Internal Settings");
        seq.addData(0x0A);
        
        seq.addCommand(0x89, "Internal Settings");
        seq.addData(0x21);
        
        seq.addCommand(0x8A, "Internal Settings");
        seq.addData(0x00);
        
        seq.addCommand(0x8B, "Internal Settings");
        seq.addData(0x80);
        
        seq.addCommand(0x8C, "Internal Settings");
        seq.addData(0x01);
        
        seq.addCommand(0x8D, "Internal Settings");
        seq.addData(0x01);
        
        seq.addCommand(0x8E, "Internal Settings");
        seq.addData(0xFF);
        
        seq.addCommand(0x8F, "Internal Settings");
        seq.addData(0xFF);
        
        // Display Function Control
        seq.addCommand(0xB6, "Display Function Control");
        seq.addDataList({0x00, 0x00});
        
        // Power Control 2
        seq.addCommand(0xC1, "Power Control 2");
        seq.addData(0x13);
        
        // Power Control 3
        seq.addCommand(0xC2, "Power Control 3");
        seq.addData(0x13);
        
        // Power Control 4
        seq.addCommand(0xC3, "Power Control 4");
        seq.addData(0x13);
        
        // Power Control 5
        seq.addCommand(0xC4, "Power Control 5");
        seq.addData(0x13);
        
        // Power Control 6
        seq.addCommand(0xC5, "Power Control 6");
        seq.addData(0x13);
        
        // Frame Rate Control
        seq.addCommand(0xC6, "Frame Rate Control");
        seq.addData(0x0A);
        
        // Power Control 7
        seq.addCommand(0xC7, "Power Control 7");
        seq.addData(0x13);
        
        // Memory Access Control
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(0x08);
        
        // Interface Pixel Format
        seq.addCommand(0x3A, "Interface Pixel Format");
        seq.addData(0x05);
        
        // Gamma Settings
        seq.addCommand(0xE0, "Positive Gamma Control");
        seq.addDataList({0xD0, 0x00, 0x02, 0x07, 0x0B, 0x1A, 0x31, 0x54,
                        0x40, 0x29, 0x12, 0x12, 0x12, 0x17});
        
        seq.addCommand(0xE1, "Negative Gamma Control");
        seq.addDataList({0xD0, 0x00, 0x02, 0x07, 0x05, 0x25, 0x2D, 0x44,
                        0x45, 0x1C, 0x18, 0x16, 0x1C, 0x1D});
        
        // Sleep Out
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        
        // Display On
        seq.addCommand(0x29, "Display ON");
        seq.addDelay(20);
        
        seq.addEnd();
        return seq;
    }

    CommandSequence createSleepSequence() override {
        CommandSequence seq("GC9A01 Sleep");
        seq.addCommand(0x28, "Display OFF");
        seq.addDelay(20);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("GC9A01 Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x29, "Display ON");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("GC9A01 Display On");
        seq.addCommand(0x29, "Display ON");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("GC9A01 Display Off");
        seq.addCommand(0x28, "Display OFF");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("GC9A01 Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("GC9A01 Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    CommandSequence createIdleModeSequence() override {
        CommandSequence seq("GC9A01 Idle Mode");
        seq.addCommand(0x39, "Idle Mode ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createNormalModeSequence() override {
        CommandSequence seq("GC9A01 Normal Mode");
        seq.addCommand(0x13, "Normal Display Mode ON");
        seq.addEnd();
        return seq;
    }

    // Additional GC9A01 specific sequences
    CommandSequence createAllPixelsOnSequence() {
        CommandSequence seq("GC9A01 All Pixels On");
        seq.addCommand(0x23, "All Pixels ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createTearingEffectLineOnSequence() {
        CommandSequence seq("GC9A01 Tearing Effect Line On");
        seq.addCommand(0x35, "Tearing Effect Line ON");
        seq.addData(0x00);    // V-Blanking only
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_GC9A01_H_
