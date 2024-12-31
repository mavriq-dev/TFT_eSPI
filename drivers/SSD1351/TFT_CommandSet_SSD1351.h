#ifndef _TFT_COMMANDSET_SSD1351_H_
#define _TFT_COMMANDSET_SSD1351_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_SSD1351 : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("SSD1351 Init");
        
        // Unlock commands
        seq.addCommand(0xFD, "Command Lock");
        seq.addData(0x12);    // Unlock OLED driver IC MCU interface
        
        // Display off
        seq.addCommand(0xAE, "Display Off");
        
        // Set clock divider / oscillator frequency
        seq.addCommand(0xB3, "Clock Div/Osc Freq");
        seq.addData(0xF1);    // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio
        
        // Set MUX ratio
        seq.addCommand(0xCA, "Set MUX Ratio");
        seq.addData(0x7F);    // 128MUX
        
        // Set re-map & data format
        seq.addCommand(0xA0, "Set Remap");
        seq.addData(0x74);    // 0x74 = Color Sequence: C -> B -> A, Enable COM Split Odd Even
        
        // Set display start line
        seq.addCommand(0xA1, "Display Start Line");
        seq.addData(0x00);    // Start at line 0
        
        // Set display offset
        seq.addCommand(0xA2, "Display Offset");
        seq.addData(0x00);
        
        // Set display mode
        seq.addCommand(0xA4, "Normal Display");    // Normal display mode (not all pixels on/off)
        
        // Set contrast for A, B, C
        seq.addCommand(0xC1, "Contrast ABC");
        seq.addDataList({0xC8, 0x80, 0xC8});
        
        // Set master contrast
        seq.addCommand(0xC7, "Master Contrast");
        seq.addData(0x0F);    // Maximum
        
        // Set default linear gray scale table
        seq.addCommand(0xB9, "Default Linear Gray Scale");
        
        // Set phase length
        seq.addCommand(0xB1, "Phase Length");
        seq.addData(0x32);    // Phase 1 = 5 DCLKs, Phase 2 = 3 DCLKs
        
        // Enhance driving scheme capability
        seq.addCommand(0xB4, "Pre-charge/Enhance");
        seq.addDataList({0xA0, 0xB5, 0x55});    // Enhance display performance
        
        // Set pre-charge voltage
        seq.addCommand(0xBB, "Pre-charge Voltage");
        seq.addData(0x17);    // 0.6 * VCC
        
        // Set second pre-charge period
        seq.addCommand(0xB6, "Second Pre-charge");
        seq.addData(0x01);    // Default
        
        // Set VCOMH voltage
        seq.addCommand(0xBE, "VCOMH Voltage");
        seq.addData(0x05);    // 0.82 * VCC
        
        // Set master current
        seq.addCommand(0xA6, "Master Current");
        
        // Set VSL
        seq.addCommand(0xB5, "Set VSL");
        seq.addDataList({0x0A, 0x00, 0x00});    // External VSL
        
        // Pre-charge2
        seq.addCommand(0xB4, "Pre-charge 2");
        seq.addData(0x03);
        
        // Display on
        seq.addCommand(0xAF, "Display On");
        seq.addDelay(100);    // Allow display to stabilize
        
        seq.addEnd();
        return seq;
    }

    CommandSequence createSleepSequence() override {
        CommandSequence seq("SSD1351 Sleep");
        seq.addCommand(0xAE, "Display Off");
        seq.addDelay(100);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("SSD1351 Wake");
        seq.addCommand(0xAF, "Display On");
        seq.addDelay(100);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("SSD1351 Display On");
        seq.addCommand(0xAF, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("SSD1351 Display Off");
        seq.addCommand(0xAE, "Display Off");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("SSD1351 Invert On");
        seq.addCommand(0xA7, "Invert Display");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("SSD1351 Invert Off");
        seq.addCommand(0xA6, "Normal Display");
        seq.addEnd();
        return seq;
    }

    // Additional SSD1351-specific sequences
    CommandSequence createSetContrastSequence(uint8_t a, uint8_t b, uint8_t c) {
        CommandSequence seq("SSD1351 Set Contrast");
        seq.addCommand(0xC1, "Set Contrast");
        seq.addDataList({a, b, c});
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetColorDepthSequence(bool mode65K) {
        CommandSequence seq("SSD1351 Set Color Depth");
        seq.addCommand(0xA0, "Remap/Color Depth");
        seq.addData(mode65K ? 0x74 : 0x70);  // 0x74 for 65K, 0x70 for 262K
        seq.addEnd();
        return seq;
    }

    CommandSequence createDimModeSequence(uint8_t contrast = 0x80) {
        CommandSequence seq("SSD1351 Dim Mode");
        seq.addCommand(0xC1, "Set Contrast");
        seq.addDataList({contrast, contrast, contrast});
        seq.addCommand(0xC7, "Master Contrast");
        seq.addData(0x0A);  // Reduced master contrast
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetScrollingSequence(uint8_t startRow, uint8_t numRows, uint8_t scrollSpeed) {
        CommandSequence seq("SSD1351 Set Scrolling");
        seq.addCommand(0xA2, "Vertical Scroll Area");
        seq.addDataList({startRow, numRows});
        seq.addCommand(0x2F, "Activate Scrolling");
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_SSD1351_H_
