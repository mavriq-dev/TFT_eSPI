#ifndef _TFT_COMMANDSET_ILI9486_H_
#define _TFT_COMMANDSET_ILI9486_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_ILI9486 : public CommandSequenceFactory {
public:
    CommandSequence createInitSequence() override {
        CommandSequence seq("ILI9486 Init");
        
        // Software Reset
        seq.addCommand(0x01, "Software Reset");
        seq.addDelay(120);
        
        // Power Control A
        seq.addCommand(0xC0, "Power Control A");
        seq.addDataList({0x0D, 0x0D, 0x02, 0x02});
        
        // Power Control B
        seq.addCommand(0xC1, "Power Control B");
        seq.addDataList({0x02, 0x82});
        
        // Driver Timing Control A
        seq.addCommand(0xC2, "Driver Timing Control A");
        seq.addDataList({0x01, 0x02});
        
        // Driver Timing Control B
        seq.addCommand(0xC3, "Driver Timing Control B");
        seq.addDataList({0x01, 0x02});
        
        // Power On Sequence Control
        seq.addCommand(0xC4, "Power On Sequence Control");
        seq.addDataList({0x04, 0x93, 0x30});
        
        // Power Control 1
        seq.addCommand(0xC5, "Power Control 1");
        seq.addDataList({0x0E, 0x0E});
        
        // Power Control 2
        seq.addCommand(0xC6, "Power Control 2");
        seq.addData(0x41);
        
        // Power Control 3
        seq.addCommand(0xC7, "Power Control 3");
        seq.addData(0x00);
        
        // Frame Rate Control
        seq.addCommand(0xB1, "Frame Rate Control");
        seq.addDataList({0xB0, 0x11});
        
        // Display Function Control
        seq.addCommand(0xB6, "Display Function Control");
        seq.addDataList({0x02, 0x22, 0x3B});
        
        // VCOM Control 1
        seq.addCommand(0xBE, "VCOM Control 1");
        seq.addData(0x00);
        
        // Enable 3G
        seq.addCommand(0xF2, "Enable 3G");
        seq.addData(0x02);
        
        // Interface Control
        seq.addCommand(0xB0, "Interface Control");
        seq.addDataList({0x00, 0x00, 0x00});
        
        // Gamma Set
        seq.addCommand(0x26, "Gamma Set");
        seq.addData(0x01);
        
        // Positive Gamma Correction
        seq.addCommand(0xE0, "Positive Gamma Correction");
        seq.addDataList({0x0F, 0x21, 0x1C, 0x0B, 0x0E, 0x08, 0x49, 0x98,
                        0x38, 0x09, 0x11, 0x03, 0x14, 0x10, 0x00});
        
        // Negative Gamma Correction
        seq.addCommand(0xE1, "Negative Gamma Correction");
        seq.addDataList({0x0F, 0x2F, 0x2B, 0x0C, 0x0E, 0x06, 0x47, 0x76,
                        0x37, 0x07, 0x11, 0x04, 0x23, 0x1E, 0x00});
        
        // Column Address Set
        seq.addCommand(0x2A, "Column Address Set");
        seq.addDataList({0x00, 0x00, 0x01, 0x3F});  // 320 pixels
        
        // Page Address Set
        seq.addCommand(0x2B, "Page Address Set");
        seq.addDataList({0x00, 0x00, 0x01, 0xDF});  // 480 pixels
        
        // Memory Access Control
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(0x0A);
        
        // Pixel Format Set
        seq.addCommand(0x3A, "Pixel Format Set");
        seq.addData(0x55);  // 16-bit color
        
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
        CommandSequence seq("ILI9486 Sleep");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addCommand(0x10, "Enter Sleep");
        seq.addDelay(120);
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("ILI9486 Wake");
        seq.addCommand(0x11, "Sleep Out");
        seq.addDelay(120);
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("ILI9486 Display On");
        seq.addCommand(0x29, "Display On");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("ILI9486 Display Off");
        seq.addCommand(0x28, "Display Off");
        seq.addDelay(20);
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("ILI9486 Invert On");
        seq.addCommand(0x21, "Display Inversion ON");
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("ILI9486 Invert Off");
        seq.addCommand(0x20, "Display Inversion OFF");
        seq.addEnd();
        return seq;
    }

    // Additional ILI9486-specific sequences
    CommandSequence createSetWindowSequence(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
        CommandSequence seq("ILI9486 Set Window");
        
        seq.addCommand(0x2A, "Column Address Set");
        seq.addDataList({
            static_cast<uint8_t>(x1 >> 8),
            static_cast<uint8_t>(x1 & 0xFF),
            static_cast<uint8_t>(x2 >> 8),
            static_cast<uint8_t>(x2 & 0xFF)
        });
        
        seq.addCommand(0x2B, "Page Address Set");
        seq.addDataList({
            static_cast<uint8_t>(y1 >> 8),
            static_cast<uint8_t>(y1 & 0xFF),
            static_cast<uint8_t>(y2 >> 8),
            static_cast<uint8_t>(y2 & 0xFF)
        });
        
        seq.addCommand(0x2C, "Memory Write");
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("ILI9486 Set Rotation");
        uint8_t madctl = 0;
        
        switch (rotation) {
            case 0:
                madctl = 0x40|0x08;
                break;
            case 1:
                madctl = 0x20|0x08;
                break;
            case 2:
                madctl = 0x80|0x08;
                break;
            case 3:
                madctl = 0x40|0x80|0x20|0x08;
                break;
        }
        
        seq.addCommand(0x36, "Memory Access Control");
        seq.addData(madctl);
        seq.addEnd();
        return seq;
    }

    CommandSequence createVerticalScrollingSequence(uint16_t topFixedArea, uint16_t scrollArea, uint16_t bottomFixedArea) {
        CommandSequence seq("ILI9486 Vertical Scrolling");
        
        seq.addCommand(0x33, "Vertical Scrolling Definition");
        seq.addDataList({
            static_cast<uint8_t>(topFixedArea >> 8),
            static_cast<uint8_t>(topFixedArea & 0xFF),
            static_cast<uint8_t>(scrollArea >> 8),
            static_cast<uint8_t>(scrollArea & 0xFF),
            static_cast<uint8_t>(bottomFixedArea >> 8),
            static_cast<uint8_t>(bottomFixedArea & 0xFF)
        });
        
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_ILI9486_H_
