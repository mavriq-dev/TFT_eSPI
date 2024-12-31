/***************************************************
  SSD1963 Command Sets
  
  This file provides command sequences for initializing
  and controlling SSD1963 displays.
  
  Created: December 2024
 ****************************************************/

#ifndef _SSD1963_COMMANDS_H_
#define _SSD1963_COMMANDS_H_

#include "../TFT_Commands.h"
#include "SSD1963_Driver.h"

namespace TFT_Runtime {

class SSD1963_Commands {
public:
    // Get initialization sequence
    static CommandSequence getInitSequence(const Configuration& config) {
        CommandSequence seq;
        
        // Software Reset
        seq.addCommand(SSD1963_CMD::SOFT_RESET);
        seq.addDelay(20);
        
        // Set PLL configuration
        seq.addCommand(SSD1963_CMD::SET_PLL);
        seq.addData(0x01);  // Enable PLL
        seq.addDelay(1);
        
        seq.addCommand(SSD1963_CMD::SET_PLL_MN);
        seq.addDataBytes((const uint8_t[]){0x23, 0x02, 0x54}, 3);  // Multiply by 35, divide by 2
        seq.addDelay(1);
        
        seq.addCommand(SSD1963_CMD::SET_PLL);
        seq.addData(0x03);  // Use PLL
        seq.addDelay(1);
        
        // Set LCD mode
        seq.addCommand(SSD1963_CMD::SET_LCD_MODE);
        seq.addDataBytes((const uint8_t[]){
            0x20,                   // 24-bit mode, TFT mode
            (config.geometry.width >> 8) & 0xFF,   // Set horizontal panel size
            config.geometry.width & 0xFF,
            (config.geometry.height >> 8) & 0xFF,  // Set vertical panel size
            config.geometry.height & 0xFF,
            0x00                    // RGB mode
        }, 6);
        
        // Set pixel data interface
        seq.addCommand(SSD1963_CMD::SET_PIXEL_DATA_IF);
        seq.addData(0x00);  // 8-bit interface
        
        // Set horizontal period
        seq.addCommand(SSD1963_CMD::SET_HORI_PERIOD);
        seq.addDataBytes((const uint8_t[]){
            0x01, 0x57, 0x00, 0x0A, 0x00, 0x01, 0x00, 0x00
        }, 8);
        
        // Set vertical period
        seq.addCommand(SSD1963_CMD::SET_VERT_PERIOD);
        seq.addDataBytes((const uint8_t[]){
            0x01, 0x0E, 0x00, 0x0A, 0x00, 0x00
        }, 6);
        
        // Set address mode
        seq.addCommand(SSD1963_CMD::SET_ADDR_MODE);
        seq.addData(0x00);
        
        // Set pixel format
        seq.addCommand(SSD1963_CMD::SET_PIXEL_FMT);
        seq.addData(0x70);  // 24-bit
        
        // Set post processor
        seq.addCommand(SSD1963_CMD::SET_POST_PROC);
        seq.addDataBytes((const uint8_t[]){
            0x40,  // Contrast
            0x80,  // Brightness
            0x40,  // Saturation
            0x01   // Enable post processor
        }, 4);
        
        // Configure PWM for backlight
        seq.addCommand(SSD1963_CMD::SET_PWM_CONF);
        seq.addDataBytes((const uint8_t[]){
            0x06,    // PWM frequency = PLL/(256*(1+5))
            0xFF,    // PWM duty cycle
            0x01,    // Enable PWM
            0x00,    // Enable PWM during sleep
            0x00,    // Reserved
            0x00     // Reserved
        }, 6);
        
        // Exit sleep
        seq.addCommand(SSD1963_CMD::EXIT_SLEEP);
        seq.addDelay(5);
        
        // Display on
        seq.addCommand(SSD1963_CMD::ON_DISPLAY);
        seq.addDelay(5);
        
        return seq;
    }
    
    // Get sleep sequence
    static CommandSequence getSleepSequence() {
        CommandSequence seq;
        
        seq.addCommand(SSD1963_CMD::BLANK_DISPLAY);
        seq.addDelay(20);
        seq.addCommand(SSD1963_CMD::ENT_SLEEP);
        seq.addDelay(5);
        
        return seq;
    }
    
    // Get wake sequence
    static CommandSequence getWakeSequence() {
        CommandSequence seq;
        
        seq.addCommand(SSD1963_CMD::EXIT_SLEEP);
        seq.addDelay(5);
        seq.addCommand(SSD1963_CMD::ON_DISPLAY);
        seq.addDelay(5);
        
        return seq;
    }
    
    // Get PLL configuration sequence
    static CommandSequence getPLLSequence(uint8_t mult, uint8_t div) {
        CommandSequence seq;
        
        seq.addCommand(SSD1963_CMD::SET_PLL);
        seq.addData(0x00);  // Disable PLL
        
        seq.addCommand(SSD1963_CMD::SET_PLL_MN);
        seq.addDataBytes((const uint8_t[]){mult, div, 0x54}, 3);
        
        seq.addCommand(SSD1963_CMD::SET_PLL);
        seq.addData(0x01);  // Enable PLL
        seq.addDelay(1);
        
        seq.addCommand(SSD1963_CMD::SET_PLL);
        seq.addData(0x03);  // Use PLL
        
        return seq;
    }
    
    // Get LCD timing sequence
    static CommandSequence getLCDTimingSequence(const LCDTiming& timing) {
        CommandSequence seq;
        
        // Set horizontal period
        seq.addCommand(SSD1963_CMD::SET_HORI_PERIOD);
        seq.addDataBytes((const uint8_t[]){
            (timing.horizontalTotal >> 8) & 0xFF,
            timing.horizontalTotal & 0xFF,
            (timing.horizontalDisplay >> 8) & 0xFF,
            timing.horizontalDisplay & 0xFF,
            (timing.horizontalSyncStart >> 8) & 0xFF,
            timing.horizontalSyncStart & 0xFF,
            (timing.horizontalSyncWidth >> 8) & 0xFF,
            timing.horizontalSyncWidth & 0xFF
        }, 8);
        
        // Set vertical period
        seq.addCommand(SSD1963_CMD::SET_VERT_PERIOD);
        seq.addDataBytes((const uint8_t[]){
            (timing.verticalTotal >> 8) & 0xFF,
            timing.verticalTotal & 0xFF,
            (timing.verticalDisplay >> 8) & 0xFF,
            timing.verticalDisplay & 0xFF,
            (timing.verticalSyncStart >> 8) & 0xFF,
            timing.verticalSyncStart & 0xFF
        }, 6);
        
        return seq;
    }
    
    // Get PWM configuration sequence
    static CommandSequence getPWMSequence(uint8_t prescaler, uint8_t compare, uint8_t period) {
        CommandSequence seq;
        
        seq.addCommand(SSD1963_CMD::SET_PWM_CONF);
        seq.addDataBytes((const uint8_t[]){
            prescaler,  // PWM frequency
            compare,    // PWM duty cycle
            0x01,      // Enable PWM
            0x00,      // Disable during sleep
            period,    // PWM period
            0x00       // Reserved
        }, 6);
        
        return seq;
    }
    
    // Get backlight sequence
    static CommandSequence getBacklightSequence(uint8_t level) {
        CommandSequence seq;
        
        seq.addCommand(SSD1963_CMD::SET_PWM_CONF);
        seq.addDataBytes((const uint8_t[]){
            0x06,    // PWM frequency
            level,   // PWM duty cycle (brightness)
            0x01,    // Enable PWM
            0x00,    // Disable during sleep
            0x00,    // Reserved
            0x00     // Reserved
        }, 6);
        
        return seq;
    }
    
    // Get tearing effect sequence
    static CommandSequence getTearingSequence(bool enable, bool vblank) {
        CommandSequence seq;
        
        if (enable) {
            seq.addCommand(SSD1963_CMD::SET_TEAR_ON);
            seq.addData(vblank ? 0x00 : 0x01);
        } else {
            seq.addCommand(SSD1963_CMD::SET_TEAR_OFF);
        }
        
        return seq;
    }
};

} // namespace TFT_Runtime

#endif // _SSD1963_COMMANDS_H_
