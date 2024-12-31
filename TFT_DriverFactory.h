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
#include "TFT_Drivers/ILI9486_Driver.h"
#include "TFT_Drivers/ILI9488_Driver.h"
#include "TFT_Drivers/SSD1963_Driver.h"
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
            case DisplayDriver::ST7735R:
            case DisplayDriver::ST7735S:
                isValid = ST7735_Driver::validateConfig(config);
                break;
                
            case DisplayDriver::ILI9341:
            case DisplayDriver::ILI9341_2:
                isValid = ILI9341_Driver::validateConfig(config);
                break;
                
            case DisplayDriver::ST7789:
            case DisplayDriver::ST7789V:
            case DisplayDriver::ST7789R:
                isValid = ST7789_Driver::validateConfig(config);
                break;
                
            case DisplayDriver::ILI9163:
                isValid = ILI9163_Driver::validateConfig(config);
                break;
                
            case DisplayDriver::HX8357D:
                isValid = HX8357D_Driver::validateConfig(config);
                break;
                
            case DisplayDriver::ILI9486:
                isValid = ILI9486_Driver::validateConfig(config);
                break;
                
            case DisplayDriver::ILI9488:
                isValid = ILI9488_Driver::validateConfig(config);
                break;
                
            case DisplayDriver::SSD1963:
                isValid = SSD1963_Driver::validateConfig(config);
                break;
                
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
            case DisplayDriver::ST7735R:
            case DisplayDriver::ST7735S:
                return std::make_unique<ST7735_Driver>(config);
                
            case DisplayDriver::ILI9341:
            case DisplayDriver::ILI9341_2:
                return std::make_unique<ILI9341_Driver>(config);
                
            case DisplayDriver::ST7789:
            case DisplayDriver::ST7789V:
            case DisplayDriver::ST7789R:
                return std::make_unique<ST7789_Driver>(config);
                
            case DisplayDriver::ILI9163:
                return std::make_unique<ILI9163_Driver>(config);
                
            case DisplayDriver::HX8357D:
                return std::make_unique<HX8357D_Driver>(config);
                
            case DisplayDriver::ILI9486:
                return std::make_unique<ILI9486_Driver>(config);
                
            case DisplayDriver::ILI9488:
                return std::make_unique<ILI9488_Driver>(config);
                
            case DisplayDriver::SSD1963:
                return std::make_unique<SSD1963_Driver>(config);
                
            default:
                return nullptr;
        }
    }
    
    // Get driver name as string
    static const char* getDriverName(DisplayDriver driver) {
        switch (driver) {
            case DisplayDriver::ST7735:   return "ST7735";
            case DisplayDriver::ST7735R:  return "ST7735R";
            case DisplayDriver::ST7735S:  return "ST7735S";
            case DisplayDriver::ILI9341:  return "ILI9341";
            case DisplayDriver::ILI9341_2: return "ILI9341_2";
            case DisplayDriver::ST7789:   return "ST7789";
            case DisplayDriver::ST7789V:  return "ST7789V";
            case DisplayDriver::ST7789R:  return "ST7789R";
            case DisplayDriver::ILI9163:  return "ILI9163";
            case DisplayDriver::HX8357D:  return "HX8357D";
            case DisplayDriver::ILI9486:  return "ILI9486";
            case DisplayDriver::ILI9488:  return "ILI9488";
            case DisplayDriver::SSD1963:  return "SSD1963";
            case DisplayDriver::NONE:     return "NONE";
            default:                      return "UNKNOWN";
        }
    }
    
    // Get default configuration for a driver
    static Configuration getDefaultConfig(DisplayDriver driver) {
        Configuration config;
        config.driver = driver;
        
        switch (driver) {
            case DisplayDriver::ST7735:
            case DisplayDriver::ST7735R:
            case DisplayDriver::ST7735S:
                config.geometry.width = 128;
                config.geometry.height = 160;
                break;
                
            case DisplayDriver::ILI9341:
            case DisplayDriver::ILI9341_2:
                config.geometry.width = 240;
                config.geometry.height = 320;
                break;
                
            case DisplayDriver::ST7789:
            case DisplayDriver::ST7789V:
            case DisplayDriver::ST7789R:
                config.geometry.width = 240;
                config.geometry.height = 320;
                break;
                
            case DisplayDriver::ILI9163:
                config.geometry.width = 128;
                config.geometry.height = 128;
                break;
                
            case DisplayDriver::HX8357D:
                config.geometry.width = 320;
                config.geometry.height = 480;
                break;
                
            case DisplayDriver::ILI9486:
            case DisplayDriver::ILI9488:
                config.geometry.width = 320;
                config.geometry.height = 480;
                break;
                
            case DisplayDriver::SSD1963:
                config.geometry.width = 800;
                config.geometry.height = 480;
                break;
                
            default:
                config.geometry.width = 0;
                config.geometry.height = 0;
                break;
        }
        
        return config;
    }
    
private:
    DriverFactory() = default;  // Private constructor for singleton
    
    // Prevent copying
    DriverFactory(const DriverFactory&) = delete;
    DriverFactory& operator=(const DriverFactory&) = delete;
};

} // namespace TFT_Runtime

#endif // _TFT_DRIVER_FACTORY_H_
