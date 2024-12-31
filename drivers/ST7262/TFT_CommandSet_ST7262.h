#ifndef _TFT_COMMANDSET_ST7262_H_
#define _TFT_COMMANDSET_ST7262_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_ST7262 : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("ST7262 Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(120);
        
        // Power Control A
        seq.addCommand(0xCB, "Power Control A");
        seq.addDataList({0x39, 0x2C, 0x00, 0x34, 0x02});
        
        // Power Control B
        seq.addCommand(0xCF, "Power Control B");
        seq.addDataList({0x00, 0xC1, 0x30});
        
        // Driver Timing Control A
        seq.addCommand(0xE8, "Driver Timing Control A");
        seq.addDataList({0x85, 0x00, 0x78});
        
        // Driver Timing Control B
        seq.addCommand(0xEA, "Driver Timing Control B");
        seq.addDataList({0x00, 0x00});
        
        // Power On Sequence Control
        seq.addCommand(0xED, "Power On Sequence Control");
        seq.addDataList({0x64, 0x03, 0x12, 0x81});
        
        // Pump Ratio Control
        seq.addCommand(0xF7, "Pump Ratio Control");
        seq.addData(0x20);
        
        // Power Control 1
        seq.addCommand(0xC0, "Power Control 1");
        seq.addData(0x23);
        
        // Power Control 2
        seq.addCommand(0xC1, "Power Control 2");
        seq.addData(0x10);
        
        // VCOM Control 1
        seq.addCommand(0xC5, "VCOM Control 1");
        seq.addDataList({0x3E, 0x28});
        
        // VCOM Control 2
        seq.addCommand(0xC7, "VCOM Control 2");
        seq.addData(0x86);
        
        // Memory Access Control
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(0x48);
        
        // Pixel Format Set
        seq.addCommand(0x3A, "Pixel Format Set");
        seq.addData(0x55);  // 16-bit color
        
        // Frame Rate Control
        seq.addCommand(0xB1, "Frame Rate Control");
        seq.addDataList({0x00, 0x18});
        
        // Display Function Control
        seq.addCommand(0xB6, "Display Function Control");
        seq.addDataList({0x08, 0x82, 0x27});
        
        // Enable 3G
        seq.addCommand(0xF2, "Enable 3G");
        seq.addData(0x00);
        
        // Gamma Set
        seq.addCommand(0x26, "Gamma Set");
        seq.addData(0x01);
        
        // Positive Gamma Correction
        seq.addCommand(0xE0, "Positive Gamma Correction");
        seq.addDataList({0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
                        0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00});
        
        // Negative Gamma Correction
        seq.addCommand(0xE1, "Negative Gamma Correction");
        seq.addDataList({0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
                        0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F});
        
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
        CommandSequence seq("ST7262 Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("ST7262 Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("ST7262 Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("ST7262 Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("ST7262 Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("ST7262 Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("ST7262 Set Rotation");
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
        CommandSequence seq("ST7262 Set Window");
        
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
        CommandSequence seq("ST7262 Set Gamma");
        if (gammaValues.size() == 15) {
            seq.addCommand(positive ? 0xE0 : 0xE1, 
                         positive ? "Positive Gamma" : "Negative Gamma");
            seq.addDataList(gammaValues);
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetVCOMSequence(uint8_t vcom1, uint8_t vcom2) {
        CommandSequence seq("ST7262 Set VCOM");
        seq.addCommand(0xC5, "VCOM Control 1");
        seq.addDataList({vcom1, vcom2});
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetPowerControlSequence(const std::vector<uint8_t>& powerParams) {
        CommandSequence seq("ST7262 Set Power Control");
        if (powerParams.size() >= 5) {
            seq.addCommand(0xCB, "Power Control A");
            seq.addDataList(std::vector<uint8_t>(powerParams.begin(), powerParams.begin() + 5));
        }
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_ST7262_H_
