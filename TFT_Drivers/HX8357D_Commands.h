/***************************************************
  HX8357D Command Sets
  
  This file provides command sequences for initializing
  and controlling HX8357D displays.
  
  Created: December 2024
 ****************************************************/

#ifndef _HX8357D_COMMANDS_H_
#define _HX8357D_COMMANDS_H_

#include "../TFT_Commands.h"
#include "HX8357D_Driver.h"

namespace TFT_Runtime {

class HX8357D_Commands {
public:
    // Get initialization sequence
    static CommandSequence getInitSequence() {
        CommandSequence seq;
        
        // Software Reset
        seq.addCommand(HX8357D_CMD::SWRESET);
        seq.addDelay(10);
        
        // Set extension command
        seq.addCommand(HX8357D_CMD::SETEXTC);
        seq.addDataBytes((const uint8_t[]){0xFF, 0x83, 0x57}, 3);
        
        // Set RGB interface
        seq.addCommand(HX8357D_CMD::SETRGB);
        seq.addDataBytes((const uint8_t[]){0x80, 0x00, 0x06, 0x06}, 4);
        
        // Set VCOM voltage
        seq.addCommand(HX8357D_CMD::SETVCOM);
        seq.addDataBytes((const uint8_t[]){0x25}, 1);
        
        // Set power control
        seq.addCommand(HX8357D_CMD::SETPOWER);
        seq.addDataBytes((const uint8_t[]){0x07, 0x42, 0x18}, 3);
        
        // Set VCOM control
        seq.addCommand(HX8357D_CMD::SETVCOM);
        seq.addDataBytes((const uint8_t[]){0x00, 0x07, 0x10}, 3);
        
        // Set Power control for normal mode
        seq.addCommand(HX8357D_CMD::SETPWRNOR);
        seq.addDataBytes((const uint8_t[]){0x01, 0x02}, 2);
        
        // Set Panel control
        seq.addCommand(HX8357D_CMD::SETPANEL);
        seq.addData(0x02);
        
        // Set frame rate
        seq.addCommand(HX8357D_CMD::SETCYC);
        seq.addDataBytes((const uint8_t[]){0x40, 0x40, 0x01, 0x02}, 4);
        
        // Set Gamma
        seq.addCommand(HX8357D_CMD::SETGAMMA);
        seq.addDataBytes((const uint8_t[]){
            0x02, 0x0A, 0x11, 0x1d, 0x23, 0x35, 0x41, 0x4b,
            0x4b, 0x42, 0x3A, 0x27, 0x1B, 0x08, 0x09, 0x03,
            0x02, 0x0A, 0x11, 0x1d, 0x23, 0x35, 0x41, 0x4b,
            0x4b, 0x42, 0x3A, 0x27, 0x1B, 0x08, 0x09, 0x03,
            0x00, 0x01}, 34);
        
        // Set COLMOD to 16-bit color
        seq.addCommand(HX8357D_CMD::COLMOD);
        seq.addData(0x55);
        
        // Exit Sleep
        seq.addCommand(HX8357D_CMD::SLPOUT);
        seq.addDelay(150);
        
        // Set display on
        seq.addCommand(HX8357D_CMD::DISPON);
        seq.addDelay(50);
        
        return seq;
    }
    
    // Get sleep sequence
    static CommandSequence getSleepSequence() {
        CommandSequence seq;
        
        seq.addCommand(HX8357D_CMD::DISPOFF);
        seq.addDelay(20);
        seq.addCommand(HX8357D_CMD::SLPIN);
        seq.addDelay(150);
        
        return seq;
    }
    
    // Get wake sequence
    static CommandSequence getWakeSequence() {
        CommandSequence seq;
        
        seq.addCommand(HX8357D_CMD::SLPOUT);
        seq.addDelay(150);
        seq.addCommand(HX8357D_CMD::DISPON);
        seq.addDelay(150);
        
        return seq;
    }
    
    // Get power control sequence
    static CommandSequence getPowerControlSequence() {
        CommandSequence seq;
        
        // Set power control
        seq.addCommand(HX8357D_CMD::SETPOWER);
        seq.addDataBytes((const uint8_t[]){0x07, 0x42, 0x18}, 3);
        
        // Set VCOM control
        seq.addCommand(HX8357D_CMD::SETVCOM);
        seq.addDataBytes((const uint8_t[]){0x00, 0x07, 0x10}, 3);
        
        // Set Power control for normal mode
        seq.addCommand(HX8357D_CMD::SETPWRNOR);
        seq.addDataBytes((const uint8_t[]){0x01, 0x02}, 2);
        
        return seq;
    }
    
    // Get color mode sequence
    static CommandSequence getColorModeSequence(uint8_t colorMode) {
        CommandSequence seq;
        
        seq.addCommand(HX8357D_CMD::COLMOD);
        seq.addData(colorMode);
        seq.addDelay(10);
        
        return seq;
    }
    
    // Get inversion sequence
    static CommandSequence getInversionSequence(bool enable) {
        CommandSequence seq;
        
        seq.addCommand(enable ? HX8357D_CMD::INVON : HX8357D_CMD::INVOFF);
        seq.addDelay(10);
        
        return seq;
    }
    
    // Get VCOM sequence
    static CommandSequence getVCOMSequence() {
        CommandSequence seq;
        
        seq.addCommand(HX8357D_CMD::SETVCOM);
        seq.addDataBytes((const uint8_t[]){0x00, 0x07, 0x10}, 3);
        seq.addDelay(10);
        
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _HX8357D_COMMANDS_H_
