/***************************************************
  ILI9163 Command Sets
  
  This file provides command sequences for initializing
  and controlling ILI9163 displays.
  
  Created: December 2024
 ****************************************************/

#ifndef _ILI9163_COMMANDS_H_
#define _ILI9163_COMMANDS_H_

#include "../TFT_Commands.h"
#include "ILI9163_Driver.h"

namespace TFT_Runtime {

class ILI9163_Commands {
public:
    // Get initialization sequence
    static CommandSequence getInitSequence() {
        CommandSequence seq;
        
        // Software Reset
        seq.addCommand(ILI9163_CMD::SWRESET);
        seq.addDelay(150);
        
        // Exit Sleep Mode
        seq.addCommand(ILI9163_CMD::SLPOUT);
        seq.addDelay(150);
        
        // Frame Rate Control (In normal mode/Full colors)
        seq.addCommand(ILI9163_CMD::FRMCTR1);
        seq.addDataBytes((const uint8_t[]){0x08, 0x08, 0x08}, 3); // 70Hz
        
        // Frame Rate Control (In Idle mode/8-colors)
        seq.addCommand(ILI9163_CMD::FRMCTR2);
        seq.addDataBytes((const uint8_t[]){0x08, 0x08, 0x08}, 3); // 70Hz
        
        // Frame Rate Control (In Partial mode/full colors)
        seq.addCommand(ILI9163_CMD::FRMCTR3);
        seq.addDataBytes((const uint8_t[]){0x08, 0x08, 0x08}, 3); // 70Hz
        
        // Display Inversion Control
        seq.addCommand(ILI9163_CMD::INVCTR);
        seq.addData(0x07);
        
        // Power Control 1
        seq.addCommand(ILI9163_CMD::PWCTR1);
        seq.addDataBytes((const uint8_t[]){0x0A, 0x02}, 2);
        
        // Power Control 2
        seq.addCommand(ILI9163_CMD::PWCTR2);
        seq.addData(0x02);
        
        // Power Control 3
        seq.addCommand(ILI9163_CMD::PWCTR3);
        seq.addData(0x01);
        
        // Power Control 4
        seq.addCommand(ILI9163_CMD::PWCTR4);
        seq.addData(0x01);
        
        // Power Control 5
        seq.addCommand(ILI9163_CMD::PWCTR5);
        seq.addData(0x01);
        
        // VCOM Control 1
        seq.addCommand(ILI9163_CMD::VMCTR1);
        seq.addDataBytes((const uint8_t[]){0x0E, 0x14}, 2);
        
        // Color Mode - 16 bit per pixel
        seq.addCommand(ILI9163_CMD::COLMOD);
        seq.addData(0x05);
        
        // Gamma Set
        seq.addCommand(ILI9163_CMD::GAMMASEL);
        seq.addData(0x01);
        
        // Positive Gamma Correction
        seq.addCommand(ILI9163_CMD::GMCTRP1);
        seq.addDataBytes((const uint8_t[]){
            0x3F, 0x25, 0x1C, 0x1E, 0x20, 0x12, 0x2A, 0x90,
            0x24, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00}, 15);
        
        // Negative Gamma Correction
        seq.addCommand(ILI9163_CMD::GMCTRN1);
        seq.addDataBytes((const uint8_t[]){
            0x20, 0x20, 0x20, 0x20, 0x05, 0x00, 0x15, 0xA7,
            0x3D, 0x18, 0x25, 0x2A, 0x2B, 0x2B, 0x3A}, 15);
        
        // Normal Display Mode
        seq.addCommand(ILI9163_CMD::NORON);
        seq.addDelay(10);
        
        // Display on
        seq.addCommand(ILI9163_CMD::DISPON);
        seq.addDelay(100);
        
        return seq;
    }
    
    // Get sleep sequence
    static CommandSequence getSleepSequence() {
        CommandSequence seq;
        
        seq.addCommand(ILI9163_CMD::DISPOFF);
        seq.addDelay(20);
        seq.addCommand(ILI9163_CMD::SLPIN);
        seq.addDelay(120);
        
        return seq;
    }
    
    // Get wake sequence
    static CommandSequence getWakeSequence() {
        CommandSequence seq;
        
        seq.addCommand(ILI9163_CMD::SLPOUT);
        seq.addDelay(120);
        seq.addCommand(ILI9163_CMD::DISPON);
        seq.addDelay(120);
        
        return seq;
    }
    
    // Get color mode sequence
    static CommandSequence getColorModeSequence(uint8_t colorMode) {
        CommandSequence seq;
        
        seq.addCommand(ILI9163_CMD::COLMOD);
        seq.addData(colorMode);
        seq.addDelay(10);
        
        return seq;
    }
    
    // Get inversion sequence
    static CommandSequence getInversionSequence(bool enable) {
        CommandSequence seq;
        
        seq.addCommand(enable ? ILI9163_CMD::INVON : ILI9163_CMD::INVOFF);
        seq.addDelay(10);
        
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _ILI9163_COMMANDS_H_
