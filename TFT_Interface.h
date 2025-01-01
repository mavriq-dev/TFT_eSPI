#ifndef _TFT_INTERFACE_H_
#define _TFT_INTERFACE_H_

#include "TFT_Runtime.h"
#include <Arduino.h>

namespace TFT_Runtime {

class TFT_Interface {
public:
    explicit TFT_Interface(const Config& config) : _config(config) {}
    virtual ~TFT_Interface() = default;

    // Interface initialization
    virtual bool begin() = 0;
    virtual void end() = 0;

    // Basic I/O operations
    virtual void writeCommand(uint8_t cmd) = 0;
    virtual void writeData(uint8_t data) = 0;
    virtual void writeData16(uint16_t data) = 0;
    virtual uint8_t readData() = 0;
    virtual uint16_t readData16() = 0;

    // Bulk data operations
    virtual void writeDataBlock(const uint8_t* data, size_t len) = 0;
    virtual void writeDataBlock16(const uint16_t* data, size_t len) = 0;
    virtual void readDataBlock(uint8_t* data, size_t len) = 0;
    virtual void readDataBlock16(uint16_t* data, size_t len) = 0;

    // Control signals
    virtual void setCS(bool level) = 0;
    virtual void setDC(bool level) = 0;
    virtual void setRST(bool level) = 0;

    // DMA operations (optional)
    virtual bool supportsDMA() const { return false; }
    virtual bool startDMAWrite(const uint8_t* data, size_t len) { return false; }
    virtual bool startDMAWrite16(const uint16_t* data, size_t len) { return false; }
    virtual bool isDMABusy() const { return false; }
    virtual void waitDMAComplete() {}

    // Buffer operations
    virtual bool supportsBuffering() const { return false; }
    virtual void startBuffer() {}
    virtual void endBuffer() {}
    virtual void flushBuffer() {}

protected:
    const Config& _config;
};

} // namespace TFT_Runtime

#endif // _TFT_INTERFACE_H_
