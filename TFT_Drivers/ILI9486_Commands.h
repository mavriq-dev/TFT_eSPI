/***************************************************
  ILI9486 Command Sets
  
  This file provides command sequences for initializing
  and controlling ILI9486 displays.
  
  Created: December 2024
 ****************************************************/

#ifndef _ILI9486_COMMANDS_H_
#define _ILI9486_COMMANDS_H_

#include "../TFT_Commands.h"
#include "ILI9486_Driver.h"

namespace TFT_Runtime {

class ILI9486_Commands {
public:
    // Get initialization sequence
    static CommandSequence getInitSequence() {
        CommandSequence seq;
        
        // Software Reset
        seq.addCommand(ILI9486_CMD::SWRESET);
        seq.addDelay(120);
        
        // Power Control A
        seq.addCommand(ILI9486_CMD::PWCTRL1);
        seq.addDataBytes((const uint8_t[]){0x14, 0x14}, 2);
        
        // Power Control B
        seq.addCommand(ILI9486_CMD::PWCTRL2);
        seq.addData(0x08);
        
        // Power Control 3
        seq.addCommand(ILI9486_CMD::PWCTRL3);
        seq.addDataBytes((const uint8_t[]){0x41, 0x00}, 2);
        
        // VCOM Control
        seq.addCommand(ILI9486_CMD::VMCTRL1);
        seq.addDataBytes((const uint8_t[]){0x00, 0x00, 0x00, 0x00}, 4);
        
        // Memory Access Control
        seq.addCommand(ILI9486_CMD::MADCTL);
        seq.addData(0x48);
        
        // Pixel Format Set
        seq.addCommand(ILI9486_CMD::PIXFMT);
        seq.addData(0x55);  // 16-bit color
        
        // Frame Rate Control
        seq.addCommand(ILI9486_CMD::FRMCTR1);
        seq.addDataBytes((const uint8_t[]){0x00, 0x18}, 2);
        
        // Display Function Control
        seq.addCommand(ILI9486_CMD::DISCTRL);
        seq.addDataBytes((const uint8_t[]){0x02, 0x02, 0x3B}, 3);
        
        // Entry Mode Set
        seq.addCommand(ILI9486_CMD::ETMOD);
        seq.addData(0x07);
        
        // Set Gamma
        seq.addCommand(ILI9486_CMD::GAMMA3ENA);
        seq.addData(0x00);
        
        // Positive Gamma Control
        seq.addCommand(ILI9486_CMD::PGAMCTRL);
        seq.addDataBytes((const uint8_t[]){
            0x0F, 0x1F, 0x1C, 0x0C, 0x0F, 0x08, 0x48, 0x98,
            0x37, 0x0A, 0x13, 0x04, 0x11, 0x0D, 0x00}, 15);
        
        // Negative Gamma Control
        seq.addCommand(ILI9486_CMD::NGAMCTRL);
        seq.addDataBytes((const uint8_t[]){
            0x0F, 0x32, 0x2E, 0x0B, 0x0D, 0x05, 0x47, 0x75,
            0x37, 0x06, 0x10, 0x03, 0x24, 0x20, 0x00}, 15);
        
        // Digital Gamma Control
        seq.addCommand(ILI9486_CMD::DGAMCTRL);
        seq.addDataBytes((const uint8_t[]){
            0x0F, 0x32, 0x2E, 0x0B, 0x0D, 0x05, 0x47, 0x75,
            0x37, 0x06, 0x10, 0x03, 0x24, 0x20, 0x00}, 15);
        
        // Exit Sleep
        seq.addCommand(ILI9486_CMD::SLPOUT);
        seq.addDelay(150);
        
        // Display on
        seq.addCommand(ILI9486_CMD::DISPON);
        seq.addDelay(150);
        
        return seq;
    }
    
    // Get sleep sequence
    static CommandSequence getSleepSequence() {
        CommandSequence seq;
        
        seq.addCommand(ILI9486_CMD::DISPOFF);
        seq.addDelay(20);
        seq.addCommand(ILI9486_CMD::SLPIN);
        seq.addDelay(120);
        
        return seq;
    }
    
    // Get wake sequence
    static CommandSequence getWakeSequence() {
        CommandSequence seq;
        
        seq.addCommand(ILI9486_CMD::SLPOUT);
        seq.addDelay(120);
        seq.addCommand(ILI9486_CMD::DISPON);
        seq.addDelay(120);
        
        return seq;
    }
    
    // Get power control sequence
    static CommandSequence getPowerControlSequence() {
        CommandSequence seq;
        
        // Power Control 1
        seq.addCommand(ILI9486_CMD::PWCTRL1);
        seq.addDataBytes((const uint8_t[]){0x14, 0x14}, 2);
        
        // Power Control 2
        seq.addCommand(ILI9486_CMD::PWCTRL2);
        seq.addData(0x08);
        
        // Power Control 3
        seq.addCommand(ILI9486_CMD::PWCTRL3);
        seq.addDataBytes((const uint8_t[]){0x41, 0x00}, 2);
        
        return seq;
    }
    
    // Get VCOM sequence
    static CommandSequence getVCOMSequence() {
        CommandSequence seq;
        
        seq.addCommand(ILI9486_CMD::VMCTRL1);
        seq.addDataBytes((const uint8_t[]){0x00, 0x00, 0x00, 0x00}, 4);
        
        return seq;
    }
    
    // Get color mode sequence
    static CommandSequence getColorModeSequence(uint8_t colorMode) {
        CommandSequence seq;
        
        seq.addCommand(ILI9486_CMD::PIXFMT);
        seq.addData(colorMode);
        seq.addDelay(10);
        
        return seq;
    }
    
    // Get inversion sequence
    static CommandSequence getInversionSequence(bool enable) {
        CommandSequence seq;
        
        seq.addCommand(enable ? ILI9486_CMD::INVON : ILI9486_CMD::INVOFF);
        seq.addDelay(10);
        
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _ILI9486_COMMANDS_H_
