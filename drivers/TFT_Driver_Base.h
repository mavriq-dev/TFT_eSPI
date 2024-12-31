#ifndef _TFT_DRIVER_BASE_H_
#define _TFT_DRIVER_BASE_H_

#include <Arduino.h>
#include "TFT_Runtime.h"

namespace TFT_Runtime {

class TFT_Driver_Base {
public:
    TFT_Driver_Base(Config& config) : config(config) {}
    virtual ~TFT_Driver_Base() = default;

    // Core interface functions
    virtual void init() = 0;
    virtual void writeCommand(uint8_t cmd) = 0;
    virtual void writeData(uint8_t data) = 0;
    virtual void writeBlock(uint16_t* data, uint32_t len) = 0;

    // Optional functions with default implementations
    virtual void startWrite() { /* Default empty implementation */ }
    virtual void endWrite() { /* Default empty implementation */ }
    virtual void setRotation(uint8_t rotation) { /* Default empty implementation */ }
    virtual void invertDisplay(bool invert) { /* Default empty implementation */ }

    // Display specific parameters
    virtual uint16_t width() const = 0;
    virtual uint16_t height() const = 0;
    virtual uint8_t getRotation() const = 0;

protected:
    Config& config;
    uint8_t _rotation = 0;
    int16_t _width;    // Display width
    int16_t _height;   // Display height
    bool _invert = false;
};

} // namespace TFT_Runtime

#endif // _TFT_DRIVER_BASE_H_
