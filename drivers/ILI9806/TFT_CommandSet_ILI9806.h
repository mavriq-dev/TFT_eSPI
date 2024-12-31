#ifndef _TFT_COMMANDSET_ILI9806_H_
#define _TFT_COMMANDSET_ILI9806_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_ILI9806 : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("ILI9806 Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(120);
        
        // LCD_Write_COM(0xFF);     // EXTC Command Set enable register 
        // LCD_Write_DATA(0xFF);
        // LCD_Write_DATA(0x98);
        // LCD_Write_DATA(0x06);
        seq.addCommand(0xFF, "EXTC Command Set Enable");
        seq.addDataList({0xFF, 0x98, 0x06});
        
        // Power Control
        seq.addCommand(0xBA, "Power Control");
        seq.addData(0x60);
        
        // VCOM Control
        seq.addCommand(0xBC, "VCOM Control");
        seq.addDataList({0x01, 0x18});
        
        // Power Control 1
        seq.addCommand(0xB0, "Power Control 1");
        seq.addDataList({0x00, 0x01, 0x02});
        
        // Power Control 2
        seq.addCommand(0xB1, "Power Control 2");
        seq.addDataList({0x00, 0x12, 0x14});
        
        // Power Control 3
        seq.addCommand(0xB4, "Power Control 3");
        seq.addDataList({0x08, 0x02});
        
        // Display Timing Control
        seq.addCommand(0xB8, "Display Timing Control");
        seq.addDataList({0x40, 0x00});
        
        // Panel Driving Setting
        seq.addCommand(0xC0, "Panel Driving Setting");
        seq.addDataList({0x00, 0x58, 0x00, 0x14, 0x16, 0x00, 0x58, 0x00, 0x14, 0x16});
        
        // Display Timing Control for Normal Mode
        seq.addCommand(0xC1, "Display Timing Control for Normal Mode");
        seq.addDataList({0x20, 0x01, 0x02});
        
        // Display Timing Control for Idle Mode
        seq.addCommand(0xC2, "Display Timing Control for Idle Mode");
        seq.addDataList({0x00, 0x00, 0x00, 0x00});
        
        // Frame Rate and Inversion Control
        seq.addCommand(0xC3, "Frame Rate and Inversion Control");
        seq.addDataList({0x00, 0x00, 0x00, 0x00});
        
        // Interface Control
        seq.addCommand(0xC6, "Interface Control");
        seq.addDataList({0x00, 0x00, 0x00});
        
        // Gamma Setting
        seq.addCommand(0xD0, "Gamma Setting");
        seq.addDataList({0x00, 0x00, 0x1D, 0x17, 0x0F, 0x1F, 0x0F, 0x17,
                        0x33, 0x2C, 0x29, 0x2E, 0x30, 0x30, 0x2F});
        
        seq.addCommand(0xD1, "Gamma Setting");
        seq.addDataList({0x00, 0x00, 0x1D, 0x17, 0x0F, 0x1F, 0x0F, 0x17,
                        0x33, 0x2C, 0x29, 0x2E, 0x30, 0x30, 0x2F});
        
        seq.addCommand(0xD2, "Gamma Setting");
        seq.addDataList({0x00, 0x00, 0x1D, 0x17, 0x0F, 0x1F, 0x0F, 0x17,
                        0x33, 0x2C, 0x29, 0x2E, 0x30, 0x30, 0x2F});
        
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
        CommandSequence seq("ILI9806 Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("ILI9806 Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("ILI9806 Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("ILI9806 Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("ILI9806 Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("ILI9806 Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    // Additional ILI9806-specific sequences
    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("ILI9806 Set Rotation");
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
        CommandSequence seq("ILI9806 Set Window");
        
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

    CommandSequence createSetGammaSequence(uint8_t gamma_set, const std::vector<uint8_t>& gammaValues) {
        CommandSequence seq("ILI9806 Set Gamma");
        if (gammaValues.size() == 15) {
            seq.addCommand(0xD0 + (gamma_set & 0x03), "Gamma Setting");
            seq.addDataList(gammaValues);
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetPowerControlSequence(const std::vector<uint8_t>& powerParams) {
        CommandSequence seq("ILI9806 Set Power Control");
        if (powerParams.size() == 3) {
            seq.addCommand(0xB0, "Power Control 1");
            seq.addDataList(powerParams);
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetVCOMSequence(uint8_t vcom1, uint8_t vcom2) {
        CommandSequence seq("ILI9806 Set VCOM");
        seq.addCommand(0xBC, "VCOM Control");
        seq.addDataList({vcom1, vcom2});
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetFrameRateSequence(const std::vector<uint8_t>& frameParams) {
        CommandSequence seq("ILI9806 Set Frame Rate");
        if (frameParams.size() == 4) {
            seq.addCommand(0xC3, "Frame Rate and Inversion Control");
            seq.addDataList(frameParams);
        }
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_ILI9806_H_
