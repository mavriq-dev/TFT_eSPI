/***************************************************
  ILI9488 Command Sets
  
  This file provides command sequences for initializing
  and controlling ILI9488 displays.
  
  Created: December 2024
 ****************************************************/

#ifndef _ILI9488_COMMANDS_H_
#define _ILI9488_COMMANDS_H_

#include "../TFT_Commands.h"
#include "ILI9488_Driver.h"

namespace TFT_Runtime {

class ILI9488_Commands {
public:
    // Get initialization sequence
    static CommandSequence getInitSequence() {
        CommandSequence seq;
        
        // Software Reset
        seq.addCommand(ILI9488_CMD::SWRESET);
        seq.addDelay(120);
        
        // Power Control A
        seq.addCommand(ILI9488_CMD::PWCTRL1);
        seq.addDataBytes((const uint8_t[]){0x17, 0x15}, 2);
        
        // Power Control B
        seq.addCommand(ILI9488_CMD::PWCTRL2);
        seq.addData(0x41);
        
        // VCOM Control
        seq.addCommand(ILI9488_CMD::VMCTRL1);
        seq.addDataBytes((const uint8_t[]){0x00, 0x12, 0x80}, 3);
        
        // Memory Access Control
        seq.addCommand(ILI9488_CMD::MADCTL);
        seq.addData(0x48);
        
        // Interface Pixel Format
        seq.addCommand(ILI9488_CMD::PIXFMT);
        seq.addData(0x66);  // 18-bit color
        
        // Interface Mode Control
        seq.addCommand(ILI9488_CMD::IFMODE);
        seq.addData(0x00);
        
        // Frame Rate Control
        seq.addCommand(ILI9488_CMD::FRMCTR1);
        seq.addDataBytes((const uint8_t[]){0xA0}, 1);
        
        // Display Inversion Control
        seq.addCommand(ILI9488_CMD::INVCTR);
        seq.addData(0x02);
        
        // Display Function Control
        seq.addCommand(ILI9488_CMD::BLCTRL2);
        seq.addDataBytes((const uint8_t[]){0x02, 0x02}, 2);
        
        // Set Image Function
        seq.addCommand(ILI9488_CMD::BLCTRL4);
        seq.addData(0x00);
        
        // Adjust Control 3
        seq.addCommand(ILI9488_CMD::BLCTRL7);
        seq.addDataBytes((const uint8_t[]){0x00, 0x00}, 2);
        
        // Set Gamma
        seq.addCommand(ILI9488_CMD::PGAMCTRL);
        seq.addDataBytes((const uint8_t[]){
            0x00, 0x03, 0x09, 0x08, 0x16, 0x0A, 0x3F, 0x78,
            0x4C, 0x09, 0x0A, 0x08, 0x16, 0x1A, 0x0F}, 15);
        
        seq.addCommand(ILI9488_CMD::NGAMCTRL);
        seq.addDataBytes((const uint8_t[]){
            0x00, 0x16, 0x19, 0x03, 0x0F, 0x05, 0x32, 0x45,
            0x46, 0x04, 0x0E, 0x0D, 0x35, 0x37, 0x0F}, 15);
        
        // Exit Sleep
        seq.addCommand(ILI9488_CMD::SLPOUT);
        seq.addDelay(120);
        
        // Display on
        seq.addCommand(ILI9488_CMD::DISPON);
        seq.addDelay(25);
        
        return seq;
    }
    
    // Get sleep sequence
    static CommandSequence getSleepSequence() {
        CommandSequence seq;
        
        seq.addCommand(ILI9488_CMD::DISPOFF);
        seq.addDelay(20);
        seq.addCommand(ILI9488_CMD::SLPIN);
        seq.addDelay(120);
        
        return seq;
    }
    
    // Get wake sequence
    static CommandSequence getWakeSequence() {
        CommandSequence seq;
        
        seq.addCommand(ILI9488_CMD::SLPOUT);
        seq.addDelay(120);
        seq.addCommand(ILI9488_CMD::DISPON);
        seq.addDelay(120);
        
        return seq;
    }
    
    // Get power control sequence
    static CommandSequence getPowerControlSequence() {
        CommandSequence seq;
        
        // Power Control 1
        seq.addCommand(ILI9488_CMD::PWCTRL1);
        seq.addDataBytes((const uint8_t[]){0x17, 0x15}, 2);
        
        // Power Control 2
        seq.addCommand(ILI9488_CMD::PWCTRL2);
        seq.addData(0x41);
        
        return seq;
    }
    
    // Get VCOM sequence
    static CommandSequence getVCOMSequence() {
        CommandSequence seq;
        
        seq.addCommand(ILI9488_CMD::VMCTRL1);
        seq.addDataBytes((const uint8_t[]){0x00, 0x12, 0x80}, 3);
        
        return seq;
    }
    
    // Get backlight sequence
    static CommandSequence getBacklightSequence(uint8_t level) {
        CommandSequence seq;
        
        seq.addCommand(ILI9488_CMD::WRDISBV);
        seq.addData(level);
        
        seq.addCommand(ILI9488_CMD::WRCTRLD);
        seq.addData(0x24);  // Enable PWM
        
        return seq;
    }
    
    // Get color mode sequence
    static CommandSequence getColorModeSequence(uint8_t colorMode) {
        CommandSequence seq;
        
        seq.addCommand(ILI9488_CMD::PIXFMT);
        seq.addData(colorMode);
        seq.addDelay(10);
        
        return seq;
    }
    
    // Get inversion sequence
    static CommandSequence getInversionSequence(bool enable) {
        CommandSequence seq;
        
        seq.addCommand(enable ? ILI9488_CMD::INVON : ILI9488_CMD::INVOFF);
        seq.addDelay(10);
        
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _ILI9488_COMMANDS_H_
