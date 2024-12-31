/***************************************************
  TFT Driver Factory
  
  This file provides a factory class for creating display
  driver instances based on runtime configuration.
  
  Created: December 2024
 ****************************************************/

#ifndef _TFT_DRIVER_FACTORY_H_
#define _TFT_DRIVER_FACTORY_H_

#include "TFT_Runtime.h"
#include "TFT_Drivers/ST7735_Driver.h"
#include "TFT_Drivers/ILI9341_Driver.h"
#include "TFT_Drivers/ST7789_Driver.h"
#include "TFT_Drivers/ILI9163_Driver.h"
#include "TFT_Drivers/HX8357D_Driver.h"
#include <memory>

namespace TFT_Runtime {

class DriverFactory {
public:
    // Singleton access
    static DriverFactory& getInstance() {
        static DriverFactory instance;
        return instance;
    }
    
    // Create a driver instance based on configuration
    std::unique_ptr<Driver> createDriver(const Configuration& config) {
        // First validate the configuration using the appropriate driver's validator
        bool isValid = false;
        
        switch (config.driver) {
            case DisplayDriver::ST7735:
                isValid = ST7735_Driver::validateConfig(config);
                break;
                
            case DisplayDriver::ILI9341:
            case DisplayDriver::ILI9341_2:
                isValid = ILI9341_Driver::validateConfig(config);
                break;
                
            case DisplayDriver::ST7789:
                isValid = ST7789_Driver::validateConfig(config);
                break;
                
            case DisplayDriver::ILI9163:
                isValid = ILI9163_Driver::validateConfig(config);
                break;
                
            case DisplayDriver::HX8357D:
                isValid = HX8357D_Driver::validateConfig(config);
                break;
                
            // Add validation for other drivers as they are implemented
            case DisplayDriver::ILI9486:
            case DisplayDriver::ILI9488:
                // TODO: Add validation calls for these drivers
                return nullptr;
                
            case DisplayDriver::NONE:
            default:
                return nullptr;
        }
        
        // Only create the driver if validation passed
        if (!isValid) {
            return nullptr;
        }
        
        // Create the appropriate driver
        switch (config.driver) {
            case DisplayDriver::ST7735:
                return std::make_unique<ST7735_Driver>(config);
                
            case DisplayDriver::ILI9341:
            case DisplayDriver::ILI9341_2:
                return std::make_unique<ILI9341_Driver>(config);
                
            case DisplayDriver::ST7789:
                return std::make_unique<ST7789_Driver>(config);
                
            case DisplayDriver::ILI9163:
                return std::make_unique<ILI9163_Driver>(config);
                
            case DisplayDriver::HX8357D:
                return std::make_unique<HX8357D_Driver>(config);
                
            default:
                return nullptr;
        }
    }
    
private:
    DriverFactory() = default;  // Private constructor for singleton
    
    // Prevent copying
    DriverFactory(const DriverFactory&) = delete;
    DriverFactory& operator=(const DriverFactory&) = delete;
};

} // namespace TFT_Runtime

#endif // _TFT_DRIVER_FACTORY_H_
