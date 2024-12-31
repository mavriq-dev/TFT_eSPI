/***************************************************
  ILI9341 Command Sets
  
  This file provides command sequences for initializing
  and controlling ILI9341 displays.
  
  Created: December 2024
 ****************************************************/

#ifndef _ILI9341_COMMANDS_H_
#define _ILI9341_COMMANDS_H_

#include "../TFT_Commands.h"
#include "ILI9341_Driver.h"

namespace TFT_Runtime {

class ILI9341_Commands {
public:
    // Get initialization sequence
    static CommandSequence getInitSequence() {
        CommandSequence seq;
        
        // Software Reset
        seq.addCommand(ILI9341_CMD::SWRESET);
        seq.addDelay(150);
        
        // Power Control A
        seq.addCommand(0xCB);
        seq.addDataBytes((const uint8_t[]){0x39, 0x2C, 0x00, 0x34, 0x02}, 5);
        
        // Power Control B
        seq.addCommand(0xCF);
        seq.addDataBytes((const uint8_t[]){0x00, 0xC1, 0x30}, 3);
        
        // Driver timing control A
        seq.addCommand(0xE8);
        seq.addDataBytes((const uint8_t[]){0x85, 0x00, 0x78}, 3);
        
        // Driver timing control B
        seq.addCommand(0xEA);
        seq.addDataBytes((const uint8_t[]){0x00, 0x00}, 2);
        
        // Power on sequence control
        seq.addCommand(0xED);
        seq.addDataBytes((const uint8_t[]){0x64, 0x03, 0x12, 0x81}, 4);
        
        // Pump ratio control
        seq.addCommand(0xF7);
        seq.addData(0x20);
        
        // Power Control,VRH[5:0]
        seq.addCommand(ILI9341_CMD::PWCTR1);
        seq.addData(0x23);
        
        // Power Control,SAP[2:0];BT[3:0]
        seq.addCommand(ILI9341_CMD::PWCTR2);
        seq.addData(0x10);
        
        // VCM Control 1
        seq.addCommand(ILI9341_CMD::VMCTR1);
        seq.addDataBytes((const uint8_t[]){0x3E, 0x28}, 2);
        
        // VCM Control 2
        seq.addCommand(ILI9341_CMD::VMCTR2);
        seq.addData(0x86);
        
        // Memory Access Control
        seq.addCommand(ILI9341_CMD::MADCTL);
        seq.addData(0x48);
        
        // Pixel Format Set
        seq.addCommand(ILI9341_CMD::PIXFMT);
        seq.addData(0x55);
        
        // Frame Rate Control
        seq.addCommand(ILI9341_CMD::FRMCTR1);
        seq.addDataBytes((const uint8_t[]){0x00, 0x18}, 2);
        
        // Display Function Control
        seq.addCommand(ILI9341_CMD::DFUNCTR);
        seq.addDataBytes((const uint8_t[]){0x08, 0x82, 0x27}, 3);
        
        // Enable 3G
        seq.addCommand(0xF2);
        seq.addData(0x00);
        
        // Gamma Set
        seq.addCommand(ILI9341_CMD::GAMMASET);
        seq.addData(0x01);
        
        // Positive Gamma Correction
        seq.addCommand(ILI9341_CMD::GMCTRP1);
        seq.addDataBytes((const uint8_t[]){
            0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
            0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00}, 15);
        
        // Negative Gamma Correction
        seq.addCommand(ILI9341_CMD::GMCTRN1);
        seq.addDataBytes((const uint8_t[]){
            0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
            0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F}, 15);
        
        // Exit Sleep
        seq.addCommand(ILI9341_CMD::SLPOUT);
        seq.addDelay(120);
        
        // Display on
        seq.addCommand(ILI9341_CMD::DISPON);
        seq.addDelay(120);
        
        return seq;
    }
    
    // Get sleep sequence
    static CommandSequence getSleepSequence() {
        CommandSequence seq;
        
        seq.addCommand(ILI9341_CMD::DISPOFF);
        seq.addDelay(20);
        seq.addCommand(ILI9341_CMD::SLPIN);
        seq.addDelay(120);
        
        return seq;
    }
    
    // Get wake sequence
    static CommandSequence getWakeSequence() {
        CommandSequence seq;
        
        seq.addCommand(ILI9341_CMD::SLPOUT);
        seq.addDelay(120);
        seq.addCommand(ILI9341_CMD::DISPON);
        seq.addDelay(120);
        
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _ILI9341_COMMANDS_H_
