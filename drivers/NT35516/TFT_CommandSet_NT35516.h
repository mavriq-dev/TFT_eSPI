#ifndef _TFT_COMMANDSET_NT35516_H_
#define _TFT_COMMANDSET_NT35516_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_NT35516 : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("NT35516 Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(120);
        
        // Command Set Enable
        seq.addCommand(0xF000, "Command Set Enable 1");
        seq.addDataList({0x55, 0xAA, 0x52, 0x08, 0x01});
        
        // AVDD Set
        seq.addCommand(0xB000, "AVDD Set");
        seq.addDataList({0x05, 0x05, 0x05});
        
        seq.addCommand(0xB001, "AVDD Ratio");
        seq.addDataList({0x05, 0x05, 0x05});
        
        // AVEE Set
        seq.addCommand(0xB100, "AVEE Set");
        seq.addDataList({0x05, 0x05, 0x05});
        
        seq.addCommand(0xB101, "AVEE Ratio");
        seq.addDataList({0x05, 0x05, 0x05});
        
        // VCL Set
        seq.addCommand(0xB200, "VCL Set");
        seq.addDataList({0x03, 0x03, 0x03});
        
        seq.addCommand(0xB201, "VCL Ratio");
        seq.addDataList({0x03, 0x03, 0x03});
        
        // VCOM
        seq.addCommand(0xB300, "VCOM Set");
        seq.addDataList({0x0B, 0x0B, 0x0B});
        
        seq.addCommand(0xB301, "VCOM Ratio");
        seq.addDataList({0x0B, 0x0B, 0x0B});
        
        // Power Control
        seq.addCommand(0xB900, "Power Control");
        seq.addDataList({0x24, 0x24, 0x24});
        
        seq.addCommand(0xBA00, "Power Control 2");
        seq.addDataList({0x24, 0x24, 0x24});
        
        // Command Set Enable 2
        seq.addCommand(0xF000, "Command Set Enable 2");
        seq.addDataList({0x55, 0xAA, 0x52, 0x08, 0x00});
        
        // Display Control
        seq.addCommand(0xB000, "Display Control");
        seq.addDataList({0x08, 0x08});
        
        seq.addCommand(0xB100, "Display Timing");
        seq.addDataList({0xCC, 0x00});
        
        // Source Timing Control
        seq.addCommand(0xB600, "Source Timing Control");
        seq.addDataList({0x05, 0x05});
        
        // Memory Access Control
        seq.addCommand(0x3600, "Memory Access Control");
        seq.addData(0x00);
        
        // Interface Pixel Format
        seq.addCommand(0x3A00, "Interface Pixel Format");
        seq.addData(0x55);  // 16-bit color
        
        // Exit Sleep Mode
        seq.addCommand(0x1100, "Sleep Out");
        seq.addDelay(120);
        
        // Display On
        seq.addCommand(0x2900, "Display On");
        seq.addDelay(20);
        
        seq.addEnd();
        return seq;
    }

    CommandSequence createSleepSequence() override {
        CommandSequence seq("NT35516 Sleep");
        seq.addCommand(0x2800, "Display Off");
        seq.addDelay(20);
        seq.addCommand(0x1000, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("NT35516 Wake");
        seq.addCommand(0x1100, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x2900, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("NT35516 Display On");
        seq.addCommand(0x2900, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("NT35516 Display Off");
        seq.addCommand(0x2800, "Display Off");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("NT35516 Invert On");
        seq.addCommand(0x2100, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("NT35516 Invert Off");
        seq.addCommand(0x2000, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("NT35516 Set Rotation");
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
        
        seq.addCommand(0x3600, "Memory Access Control");
        seq.addData(madctl);
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetWindowSequence(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
        CommandSequence seq("NT35516 Set Window");
        
        seq.addCommand(0x2A00, "Column Address Set");
        seq.addDataList({
            static_cast<uint8_t>(x0 >> 8),
            static_cast<uint8_t>(x0 & 0xFF),
            static_cast<uint8_t>(x1 >> 8),
            static_cast<uint8_t>(x1 & 0xFF)
        });
        
        seq.addCommand(0x2B00, "Page Address Set");
        seq.addDataList({
            static_cast<uint8_t>(y0 >> 8),
            static_cast<uint8_t>(y0 & 0xFF),
            static_cast<uint8_t>(y1 >> 8),
            static_cast<uint8_t>(y1 & 0xFF)
        });
        
        seq.addCommand(0x2C00, "Memory Write");
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetVCOMSequence(uint8_t vcom1, uint8_t vcom2, uint8_t vcom3) {
        CommandSequence seq("NT35516 Set VCOM");
        seq.addCommand(0xB300, "VCOM Set");
        seq.addDataList({vcom1, vcom2, vcom3});
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetPowerControlSequence(const std::vector<uint8_t>& powerParams) {
        CommandSequence seq("NT35516 Set Power Control");
        if (powerParams.size() >= 3) {
            seq.addCommand(0xB900, "Power Control");
            seq.addDataList(std::vector<uint8_t>(powerParams.begin(), powerParams.begin() + 3));
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetDisplayTimingSequence(uint8_t timing1, uint8_t timing2) {
        CommandSequence seq("NT35516 Set Display Timing");
        seq.addCommand(0xB100, "Display Timing");
        seq.addDataList({timing1, timing2});
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_NT35516_H_