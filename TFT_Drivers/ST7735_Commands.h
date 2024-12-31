/***************************************************
  ST7735 Command Sets
  
  This file provides command sequences for initializing
  and controlling ST7735 displays.
  
  Created: December 2024
 ****************************************************/

#ifndef _ST7735_COMMANDS_H_
#define _ST7735_COMMANDS_H_

#include "../TFT_Commands.h"
#include "ST7735_Driver.h"

namespace TFT_Runtime {

class ST7735_Commands {
public:
    // Get initialization sequence for specific variant
    static CommandSequence getInitSequence(ST7735Variant variant) {
        CommandSequence seq;
        
        // Common initialization
        addCommonInit(seq);
        
        // Variant-specific initialization
        switch (variant) {
            case ST7735Variant::INITR_BLACKTAB:
                addBlackTabInit(seq);
                break;
            case ST7735Variant::INITR_REDTAB:
                addRedTabInit(seq);
                break;
            case ST7735Variant::INITR_GREENTAB:
            case ST7735Variant::INITR_GREENTAB2:
            case ST7735Variant::INITR_GREENTAB3:
                addGreenTabInit(seq);
                break;
            default:
                // Default to black tab
                addBlackTabInit(seq);
                break;
        }
        
        return seq;
    }
    
private:
    // Common initialization sequence
    static void addCommonInit(CommandSequence& seq) {
        // Software reset
        seq.addCommand(ST7735_CMD::SWRESET);
        seq.addDelay(150);
        
        // Out of sleep mode
        seq.addCommand(ST7735_CMD::SLPOUT);
        seq.addDelay(255);
        
        // Frame rate control - normal mode
        seq.addCommand(ST7735_CMD::FRMCTR1);
        seq.addData(0x01);  // Rate = fosc/(1x2+40) * (LINE+2C+2D)
        seq.addData(0x2C);
        seq.addData(0x2D);
        
        // Frame rate control - idle mode
        seq.addCommand(ST7735_CMD::FRMCTR2);
        seq.addData(0x01);
        seq.addData(0x2C);
        seq.addData(0x2D);
        
        // Frame rate control - partial mode
        seq.addCommand(ST7735_CMD::FRMCTR3);
        seq.addData(0x01);
        seq.addData(0x2C);
        seq.addData(0x2D);
        seq.addData(0x01);
        seq.addData(0x2C);
        seq.addData(0x2D);
        
        // Display inversion control
        seq.addCommand(ST7735_CMD::INVCTR);
        seq.addData(0x07);
        
        // Power control settings
        seq.addCommand(ST7735_CMD::PWCTR1);
        seq.addData(0xA2);
        seq.addData(0x02);
        seq.addData(0x84);
        
        seq.addCommand(ST7735_CMD::PWCTR2);
        seq.addData(0xC5);
        
        seq.addCommand(ST7735_CMD::PWCTR3);
        seq.addData(0x0A);
        seq.addData(0x00);
        
        seq.addCommand(ST7735_CMD::PWCTR4);
        seq.addData(0x8A);
        seq.addData(0x2A);
        
        seq.addCommand(ST7735_CMD::PWCTR5);
        seq.addData(0x8A);
        seq.addData(0xEE);
        
        // VCOM control
        seq.addCommand(ST7735_CMD::VMCTR1);
        seq.addData(0x0E);
    }
    
    // Black tab specific initialization
    static void addBlackTabInit(CommandSequence& seq) {
        seq.addCommand(ST7735_CMD::MADCTL);
        seq.addData(0xC0);
        
        seq.addCommand(ST7735_CMD::COLMOD);
        seq.addData(0x05);  // 16-bit color
        
        // No offsets for black tab
        // Enable display
        seq.addCommand(ST7735_CMD::NORON);
        seq.addDelay(10);
        seq.addCommand(ST7735_CMD::DISPON);
        seq.addDelay(100);
    }
    
    // Red tab specific initialization
    static void addRedTabInit(CommandSequence& seq) {
        seq.addCommand(ST7735_CMD::MADCTL);
        seq.addData(0xC0);
        
        seq.addCommand(ST7735_CMD::COLMOD);
        seq.addData(0x05);
        
        // Enable display
        seq.addCommand(ST7735_CMD::NORON);
        seq.addDelay(10);
        seq.addCommand(ST7735_CMD::DISPON);
        seq.addDelay(100);
    }
    
    // Green tab specific initialization
    static void addGreenTabInit(CommandSequence& seq) {
        seq.addCommand(ST7735_CMD::MADCTL);
        seq.addData(0xC0);
        
        seq.addCommand(ST7735_CMD::COLMOD);
        seq.addData(0x05);
        
        // Enable display
        seq.addCommand(ST7735_CMD::NORON);
        seq.addDelay(10);
        seq.addCommand(ST7735_CMD::DISPON);
        seq.addDelay(100);
    }
};

} // namespace TFT_Runtime

#endif // _ST7735_COMMANDS_H_
