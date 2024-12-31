#ifndef _TFT_COMMANDSET_ILI9225_H_
#define _TFT_COMMANDSET_ILI9225_H_

#include "../../TFT_Command.h"

namespace TFT_Runtime {

class TFT_CommandSet_ILI9225 : public CommandSequenceFactory {
public:
    // ILI9225 specific registers
    static constexpr uint16_t ILI9225_DRIVER_OUTPUT_CTRL      = 0x01;  // Driver Output Control
    static constexpr uint16_t ILI9225_LCD_AC_DRIVING_CTRL     = 0x02;  // LCD AC Driving Control
    static constexpr uint16_t ILI9225_ENTRY_MODE              = 0x03;  // Entry Mode
    static constexpr uint16_t ILI9225_DISP_CTRL1             = 0x07;  // Display Control 1
    static constexpr uint16_t ILI9225_BLANK_PERIOD_CTRL1     = 0x08;  // Blank Period Control
    static constexpr uint16_t ILI9225_FRAME_CYCLE_CTRL       = 0x0B;  // Frame Cycle Control
    static constexpr uint16_t ILI9225_INTERFACE_CTRL         = 0x0C;  // Interface Control
    static constexpr uint16_t ILI9225_OSC_CTRL               = 0x0F;  // Oscillation Control
    static constexpr uint16_t ILI9225_POWER_CTRL1            = 0x10;  // Power Control 1
    static constexpr uint16_t ILI9225_POWER_CTRL2            = 0x11;  // Power Control 2
    static constexpr uint16_t ILI9225_POWER_CTRL3            = 0x12;  // Power Control 3
    static constexpr uint16_t ILI9225_POWER_CTRL4            = 0x13;  // Power Control 4
    static constexpr uint16_t ILI9225_POWER_CTRL5            = 0x14;  // Power Control 5
    static constexpr uint16_t ILI9225_VCI_RECYCLING          = 0x15;  // VCI Recycling
    static constexpr uint16_t ILI9225_RAM_ADDR_SET1          = 0x20;  // Horizontal GRAM Address Set
    static constexpr uint16_t ILI9225_RAM_ADDR_SET2          = 0x21;  // Vertical GRAM Address Set
    static constexpr uint16_t ILI9225_GRAM_DATA_REG          = 0x22;  // GRAM Data Register
    static constexpr uint16_t ILI9225_GATE_SCAN_CTRL         = 0x30;  // Gate Scan Control
    static constexpr uint16_t ILI9225_VERTICAL_SCROLL_CTRL1  = 0x31;  // Vertical Scroll Control 1
    static constexpr uint16_t ILI9225_VERTICAL_SCROLL_CTRL2  = 0x32;  // Vertical Scroll Control 2
    static constexpr uint16_t ILI9225_VERTICAL_SCROLL_CTRL3  = 0x33;  // Vertical Scroll Control 3
    static constexpr uint16_t ILI9225_PARTIAL_DRIVING_POS1   = 0x34;  // Partial Driving Position 1
    static constexpr uint16_t ILI9225_PARTIAL_DRIVING_POS2   = 0x35;  // Partial Driving Position 2
    static constexpr uint16_t ILI9225_GAMMA_CTRL1            = 0x50;  // Gamma Control 1
    static constexpr uint16_t ILI9225_GAMMA_CTRL2            = 0x51;  // Gamma Control 2
    static constexpr uint16_t ILI9225_GAMMA_CTRL3            = 0x52;  // Gamma Control 3
    static constexpr uint16_t ILI9225_GAMMA_CTRL4            = 0x53;  // Gamma Control 4
    static constexpr uint16_t ILI9225_GAMMA_CTRL5            = 0x54;  // Gamma Control 5
    static constexpr uint16_t ILI9225_GAMMA_CTRL6            = 0x55;  // Gamma Control 6
    static constexpr uint16_t ILI9225_GAMMA_CTRL7            = 0x56;  // Gamma Control 7
    static constexpr uint16_t ILI9225_GAMMA_CTRL8            = 0x57;  // Gamma Control 8
    static constexpr uint16_t ILI9225_GAMMA_CTRL9            = 0x58;  // Gamma Control 9
    static constexpr uint16_t ILI9225_GAMMA_CTRL10           = 0x59;  // Gamma Control 10

    CommandSequence createInitSequence() override {
        CommandSequence seq("ILI9225 Init");
        
        // Power On sequence
        seq.addCommand(ILI9225_POWER_CTRL1, "Power Control 1");
        seq.addData(0x0000);  // Set SAP,DSTB,STB
        seq.addCommand(ILI9225_POWER_CTRL2, "Power Control 2");
        seq.addData(0x0000);  // Set APON,PON,AON,VCI1EN,VC
        seq.addCommand(ILI9225_POWER_CTRL3, "Power Control 3");
        seq.addData(0x0000);  // Set BT,DC1,DC2,DC3
        seq.addCommand(ILI9225_POWER_CTRL4, "Power Control 4");
        seq.addData(0x0000);  // Set GVDD
        seq.addCommand(ILI9225_POWER_CTRL5, "Power Control 5");
        seq.addData(0x0000);  // Set VCOMH/VCOML voltage
        seq.addDelay(40);
        
        // Power Control
        seq.addCommand(ILI9225_POWER_CTRL2, "Power Control 2");
        seq.addData(0x0018);  // Set APON,PON,AON,VCI1EN,VC
        seq.addCommand(ILI9225_POWER_CTRL3, "Power Control 3");
        seq.addData(0x6121);  // Set BT,DC1,DC2,DC3
        seq.addCommand(ILI9225_POWER_CTRL4, "Power Control 4");
        seq.addData(0x006F);  // Set GVDD
        seq.addCommand(ILI9225_POWER_CTRL5, "Power Control 5");
        seq.addData(0x495F);  // Set VCOMH/VCOML voltage
        seq.addCommand(ILI9225_POWER_CTRL1, "Power Control 1");
        seq.addData(0x0800);  // Set SAP,DSTB,STB
        seq.addDelay(10);
        
        // Display Control
        seq.addCommand(ILI9225_DRIVER_OUTPUT_CTRL, "Driver Output Control");
        seq.addData(0x011C);  // Set SS and SM bit
        seq.addCommand(ILI9225_LCD_AC_DRIVING_CTRL, "LCD AC Driving Control");
        seq.addData(0x0100);  // Set line inversion
        seq.addCommand(ILI9225_ENTRY_MODE, "Entry Mode");
        seq.addData(0x1030);  // Set GRAM write direction and BGR=1
        seq.addCommand(ILI9225_DISP_CTRL1, "Display Control 1");
        seq.addData(0x0000);  // Set D1=0, D0=0
        seq.addCommand(ILI9225_BLANK_PERIOD_CTRL1, "Blank Period Control");
        seq.addData(0x0808);  // Set front and back porch
        seq.addCommand(ILI9225_FRAME_CYCLE_CTRL, "Frame Cycle Control");
        seq.addData(0x1100);  // Set frame cycle
        seq.addCommand(ILI9225_INTERFACE_CTRL, "Interface Control");
        seq.addData(0x0000);  // CPU interface
        seq.addCommand(ILI9225_OSC_CTRL, "Oscillation Control");
        seq.addData(0x0D01);  // Set OSC
        
        // Set GRAM area
        seq.addCommand(ILI9225_GATE_SCAN_CTRL, "Gate Scan Control");
        seq.addData(0x0000);  // Set scanning direction
        seq.addCommand(ILI9225_VERTICAL_SCROLL_CTRL1, "Vertical Scroll Control 1");
        seq.addData(0x00DB);  // Set scrolling size
        seq.addCommand(ILI9225_VERTICAL_SCROLL_CTRL2, "Vertical Scroll Control 2");
        seq.addData(0x0000);  // Set scrolling position
        seq.addCommand(ILI9225_VERTICAL_SCROLL_CTRL3, "Vertical Scroll Control 3");
        seq.addData(0x0000);  // Set scrolling position
        seq.addCommand(ILI9225_PARTIAL_DRIVING_POS1, "Partial Driving Position 1");
        seq.addData(0x00DB);  // Set scrolling size
        seq.addCommand(ILI9225_PARTIAL_DRIVING_POS2, "Partial Driving Position 2");
        seq.addData(0x0000);  // Set scrolling position
        
        // Set Gamma Curve
        seq.addCommand(ILI9225_GAMMA_CTRL1, "Gamma Control 1");
        seq.addData(0x0000);
        seq.addCommand(ILI9225_GAMMA_CTRL2, "Gamma Control 2");
        seq.addData(0x0808);
        seq.addCommand(ILI9225_GAMMA_CTRL3, "Gamma Control 3");
        seq.addData(0x080A);
        seq.addCommand(ILI9225_GAMMA_CTRL4, "Gamma Control 4");
        seq.addData(0x000A);
        seq.addCommand(ILI9225_GAMMA_CTRL5, "Gamma Control 5");
        seq.addData(0x0A08);
        seq.addCommand(ILI9225_GAMMA_CTRL6, "Gamma Control 6");
        seq.addData(0x0808);
        seq.addCommand(ILI9225_GAMMA_CTRL7, "Gamma Control 7");
        seq.addData(0x0000);
        seq.addCommand(ILI9225_GAMMA_CTRL8, "Gamma Control 8");
        seq.addData(0x0A00);
        seq.addCommand(ILI9225_GAMMA_CTRL9, "Gamma Control 9");
        seq.addData(0x0710);
        seq.addCommand(ILI9225_GAMMA_CTRL10, "Gamma Control 10");
        seq.addData(0x0710);
        
        // Display ON
        seq.addCommand(ILI9225_DISP_CTRL1, "Display Control 1");
        seq.addData(0x1017);  // Set D1=1, D0=1
        
        seq.addEnd();
        return seq;
    }

    CommandSequence createSleepSequence() override {
        CommandSequence seq("ILI9225 Sleep");
        seq.addCommand(ILI9225_DISP_CTRL1, "Display Control 1");
        seq.addData(0x0000);  // Display off
        seq.addCommand(ILI9225_POWER_CTRL1, "Power Control 1");
        seq.addData(0x0000);  // Power off
        seq.addEnd();
        return seq;
    }

    CommandSequence createWakeSequence() override {
        CommandSequence seq("ILI9225 Wake");
        seq.addCommand(ILI9225_POWER_CTRL1, "Power Control 1");
        seq.addData(0x0800);  // Power on
        seq.addDelay(10);
        seq.addCommand(ILI9225_DISP_CTRL1, "Display Control 1");
        seq.addData(0x1017);  // Display on
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOnSequence() override {
        CommandSequence seq("ILI9225 Display On");
        seq.addCommand(ILI9225_DISP_CTRL1, "Display Control 1");
        seq.addData(0x1017);  // Set D1=1, D0=1
        seq.addEnd();
        return seq;
    }

    CommandSequence createDisplayOffSequence() override {
        CommandSequence seq("ILI9225 Display Off");
        seq.addCommand(ILI9225_DISP_CTRL1, "Display Control 1");
        seq.addData(0x0000);  // Set D1=0, D0=0
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOnSequence() override {
        CommandSequence seq("ILI9225 Invert On");
        seq.addCommand(ILI9225_DRIVER_OUTPUT_CTRL, "Driver Output Control");
        seq.addData(0x031C);  // Set SS and SM bit with INV=1
        seq.addEnd();
        return seq;
    }

    CommandSequence createInvertOffSequence() override {
        CommandSequence seq("ILI9225 Invert Off");
        seq.addCommand(ILI9225_DRIVER_OUTPUT_CTRL, "Driver Output Control");
        seq.addData(0x011C);  // Set SS and SM bit with INV=0
        seq.addEnd();
        return seq;
    }

    // Additional ILI9225-specific sequences
    CommandSequence createSetRotationSequence(uint8_t rotation) {
        CommandSequence seq("ILI9225 Set Rotation");
        uint16_t entryMode = 0;
        
        switch (rotation) {
            case 0:  // Portrait
                entryMode = 0x1030;
                break;
            case 1:  // Landscape
                entryMode = 0x1028;
                break;
            case 2:  // Portrait inverted
                entryMode = 0x1000;
                break;
            case 3:  // Landscape inverted
                entryMode = 0x1018;
                break;
        }
        
        seq.addCommand(ILI9225_ENTRY_MODE, "Entry Mode");
        seq.addData(entryMode);
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetWindowSequence(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
        CommandSequence seq("ILI9225 Set Window");
        
        seq.addCommand(ILI9225_RAM_ADDR_SET1, "Horizontal GRAM Address Set");
        seq.addData(x0);
        seq.addCommand(ILI9225_RAM_ADDR_SET2, "Vertical GRAM Address Set");
        seq.addData(y0);
        seq.addCommand(ILI9225_GRAM_DATA_REG, "GRAM Data Register");
        
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetGammaSequence(const std::vector<uint16_t>& gammaValues) {
        CommandSequence seq("ILI9225 Set Gamma");
        if (gammaValues.size() == 10) {
            seq.addCommand(ILI9225_GAMMA_CTRL1, "Gamma Control 1");
            seq.addData(gammaValues[0]);
            seq.addCommand(ILI9225_GAMMA_CTRL2, "Gamma Control 2");
            seq.addData(gammaValues[1]);
            seq.addCommand(ILI9225_GAMMA_CTRL3, "Gamma Control 3");
            seq.addData(gammaValues[2]);
            seq.addCommand(ILI9225_GAMMA_CTRL4, "Gamma Control 4");
            seq.addData(gammaValues[3]);
            seq.addCommand(ILI9225_GAMMA_CTRL5, "Gamma Control 5");
            seq.addData(gammaValues[4]);
            seq.addCommand(ILI9225_GAMMA_CTRL6, "Gamma Control 6");
            seq.addData(gammaValues[5]);
            seq.addCommand(ILI9225_GAMMA_CTRL7, "Gamma Control 7");
            seq.addData(gammaValues[6]);
            seq.addCommand(ILI9225_GAMMA_CTRL8, "Gamma Control 8");
            seq.addData(gammaValues[7]);
            seq.addCommand(ILI9225_GAMMA_CTRL9, "Gamma Control 9");
            seq.addData(gammaValues[8]);
            seq.addCommand(ILI9225_GAMMA_CTRL10, "Gamma Control 10");
            seq.addData(gammaValues[9]);
        }
        seq.addEnd();
        return seq;
    }

    CommandSequence createSetPowerSequence(uint16_t ctrl1, uint16_t ctrl2, uint16_t ctrl3,
                                         uint16_t ctrl4, uint16_t ctrl5) {
        CommandSequence seq("ILI9225 Set Power");
        seq.addCommand(ILI9225_POWER_CTRL1, "Power Control 1");
        seq.addData(ctrl1);
        seq.addCommand(ILI9225_POWER_CTRL2, "Power Control 2");
        seq.addData(ctrl2);
        seq.addCommand(ILI9225_POWER_CTRL3, "Power Control 3");
        seq.addData(ctrl3);
        seq.addCommand(ILI9225_POWER_CTRL4, "Power Control 4");
        seq.addData(ctrl4);
        seq.addCommand(ILI9225_POWER_CTRL5, "Power Control 5");
        seq.addData(ctrl5);
        seq.addEnd();
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDSET_ILI9225_H_
