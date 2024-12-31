/***************************************************
  ST7789 Command Sets
  
  This file provides command sequences for initializing
  and controlling ST7789 displays.
  
  Created: December 2024
 ****************************************************/

#ifndef _ST7789_COMMANDS_H_
#define _ST7789_COMMANDS_H_

#include "../TFT_Commands.h"
#include "ST7789_Driver.h"

namespace TFT_Runtime {

class ST7789_Commands {
public:
    // Get initialization sequence
    static CommandSequence getInitSequence() {
        CommandSequence seq;
        
        // Software Reset
        seq.addCommand(ST7789_CMD::SWRESET);
        seq.addDelay(150);
        
        // Sleep Out
        seq.addCommand(ST7789_CMD::SLPOUT);
        seq.addDelay(120);
        
        // Memory Data Access Control
        seq.addCommand(ST7789_CMD::MADCTL);
        seq.addData(0x00);
        
        // Interface Pixel Format
        seq.addCommand(ST7789_CMD::COLMOD);
        seq.addData(0x55);  // 16-bit color
        
        // Porch Setting
        seq.addCommand(ST7789_CMD::PORCTRL);
        seq.addDataBytes((const uint8_t[]){0x0C, 0x0C, 0x00, 0x33, 0x33}, 5);
        
        // Gate Control
        seq.addCommand(ST7789_CMD::GCTRL);
        seq.addData(0x35);
        
        // VCOM Setting
        seq.addCommand(ST7789_CMD::VCOMS);
        seq.addData(0x2B);
        
        // LCM Control
        seq.addCommand(ST7789_CMD::LCMCTRL);
        seq.addData(0x2C);
        
        // VDV and VRH Command Enable
        seq.addCommand(ST7789_CMD::VDVVRHEN);
        seq.addData(0x01);
        seq.addData(0xFF);
        
        // VRH Set
        seq.addCommand(ST7789_CMD::VRHS);
        seq.addData(0x11);
        
        // VDV Set
        seq.addCommand(ST7789_CMD::VDVS);
        seq.addData(0x20);
        
        // Frame Rate Control
        seq.addCommand(ST7789_CMD::FRCTRL2);
        seq.addData(0x0F);  // 60Hz
        
        // Power Control 1
        seq.addCommand(ST7789_CMD::PWCTRL1);
        seq.addDataBytes((const uint8_t[]){0xA4, 0xA1}, 2);
        
        // Positive Voltage Gamma Control
        seq.addCommand(ST7789_CMD::PVGAMCTRL);
        seq.addDataBytes((const uint8_t[]){
            0xD0, 0x00, 0x05, 0x0E, 0x15, 0x0D, 0x37, 0x43,
            0x47, 0x09, 0x15, 0x12, 0x16, 0x19}, 14);
        
        // Negative Voltage Gamma Control
        seq.addCommand(ST7789_CMD::NVGAMCTRL);
        seq.addDataBytes((const uint8_t[]){
            0xD0, 0x00, 0x05, 0x0D, 0x0C, 0x06, 0x2D, 0x44,
            0x40, 0x0E, 0x1C, 0x18, 0x16, 0x19}, 14);
        
        // Inversion On
        seq.addCommand(ST7789_CMD::INVON);
        
        // Normal Display Mode On
        seq.addCommand(ST7789_CMD::NORON);
        seq.addDelay(10);
        
        // Display On
        seq.addCommand(ST7789_CMD::DISPON);
        seq.addDelay(120);
        
        return seq;
    }
    
    // Get sleep sequence
    static CommandSequence getSleepSequence() {
        CommandSequence seq;
        
        seq.addCommand(ST7789_CMD::DISPOFF);
        seq.addDelay(20);
        seq.addCommand(ST7789_CMD::SLPIN);
        seq.addDelay(120);
        
        return seq;
    }
    
    // Get wake sequence
    static CommandSequence getWakeSequence() {
        CommandSequence seq;
        
        seq.addCommand(ST7789_CMD::SLPOUT);
        seq.addDelay(120);
        seq.addCommand(ST7789_CMD::DISPON);
        seq.addDelay(120);
        
        return seq;
    }
    
    // Get color mode sequence
    static CommandSequence getColorModeSequence(uint8_t colorMode) {
        CommandSequence seq;
        
        seq.addCommand(ST7789_CMD::COLMOD);
        seq.addData(colorMode);
        seq.addDelay(10);
        
        return seq;
    }
    
    // Get inversion sequence
    static CommandSequence getInversionSequence(bool enable) {
        CommandSequence seq;
        
        seq.addCommand(enable ? ST7789_CMD::INVON : ST7789_CMD::INVOFF);
        seq.addDelay(10);
        
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _ST7789_COMMANDS_H_
