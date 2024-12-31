#ifndef _TFT_COMMANDSET_OTM8009A_H_
#define _TFT_COMMANDSET_OTM8009A_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_OTM8009A : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("OTM8009A Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(120);
        
        // Command2 Enable
        seq.addCommand(0x00, "NOP");
        seq.addData(0x00);
        
        // Power Control
        seq.addCommand(0xFF, "Command2 Enable");
        seq.addDataList({0x80, 0x09, 0x01});
        
        seq.addCommand(0x80, "Power Control 1");
        seq.addDataList({0x80});
        
        seq.addCommand(0x81, "Power Control 2");
        seq.addDataList({0x00});
        
        seq.addCommand(0x82, "Power Control 3");
        seq.addDataList({0x00});
        
        // VCOM Control
        seq.addCommand(0xC0, "VCOM Control");
        seq.addDataList({0x00, 0x58, 0x00, 0x15});
        
        seq.addCommand(0xC1, "VCOM Control 2");
        seq.addDataList({0x00, 0x01, 0x00});
        
        // Panel Driving Setting
        seq.addCommand(0xC4, "Panel Driving Setting");
        seq.addDataList({0x70, 0x00, 0x00});
        
        seq.addCommand(0xC5, "Panel Driving Setting 2");
        seq.addDataList({0x00, 0x01, 0x00});
        
        // Gamma Setting
        seq.addCommand(0xE0, "Gamma Setting");
        seq.addDataList({0x00, 0x0C, 0x15, 0x0F, 0x11, 0x08, 0x08, 0x08,
                        0x08, 0x23, 0x04, 0x13, 0x12, 0x2B, 0x34, 0x1F});
        
        seq.addCommand(0xE1, "Gamma Setting 2");
        seq.addDataList({0x00, 0x0C, 0x15, 0x0F, 0x11, 0x08, 0x08, 0x08,
                        0x08, 0x23, 0x04, 0x13, 0x12, 0x2B, 0x34, 0x1F});
        
        // Exit Command2
        seq.addCommand(0xFF, "Exit Command2");
        seq.addDataList({0xFF, 0xFF, 0xFF});
        
        // Memory Access Control
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(0x00);
        
        // Interface Pixel Format
        seq.addCommand(0x3A, "Interface Pixel Format");
        seq.addData(0x55);  // 16-bit color
        
        // Exit Sleep Mode
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        
        // Display On
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        
        seq.addEnd();
        return seq;
    }

    CommandSequence createSleepSequence() override {
        CommandSequence seq("OTM8009A Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("OTM8009A Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("OTM8009A Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("OTM8009A Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("OTM8009A Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("OTM8009A Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("OTM8009A Set Rotation");
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
        CommandSequence seq("OTM8009A Set Window");
        
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
        CommandSequence seq("OTM8009A Set Gamma");
        if (gammaValues.size() == 16) {
            seq.addCommand(positive ? 0xE0 : 0xE1, 
                         positive ? "Positive Gamma" : "Negative Gamma");
            seq.addDataList(gammaValues);
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetVCOMSequence(const std::vector<uint8_t>& vcomParams) {
        CommandSequence seq("OTM8009A Set VCOM");
        if (vcomParams.size() >= 4) {
            seq.addCommand(0xC0, "VCOM Control");
            seq.addDataList(std::vector<uint8_t>(vcomParams.begin(), vcomParams.begin() + 4));
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetPowerControlSequence(const std::vector<uint8_t>& powerParams) {
        CommandSequence seq("OTM8009A Set Power Control");
        if (powerParams.size() >= 3) {
            seq.addCommand(0xC4, "Panel Driving Setting");
            seq.addDataList(std::vector<uint8_t>(powerParams.begin(), powerParams.begin() + 3));
        }
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_OTM8009A_H_
