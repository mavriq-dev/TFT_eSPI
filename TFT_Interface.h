#ifndef _TFT_INTERFACE_H_
#define _TFT_INTERFACE_H_

#include "TFT_Runtime.h"
#include <cstddef>  // For size_t

namespace TFT_Runtime {

// Forward declarations
class TFT_Interface;
class TFT_Interface_Factory;

// DMA status tracking
enum DMAStatus {
    DMA_INACTIVE = 0,
    DMA_ACTIVE = 1,
    DMA_COMPLETE = 2,
    DMA_ERROR = 3
};

// Base interface class
class TFT_Interface {
public:
    TFT_Interface(const Config& config) : _config(config) {}
    virtual ~TFT_Interface() = default;

    // Pure virtual interface methods
    virtual bool begin() = 0;
    virtual void writeCommand(uint8_t cmd) = 0;
    virtual void writeData(uint8_t data) = 0;
    virtual void writeData16(uint16_t data) = 0;
    virtual uint8_t readData() = 0;
    virtual uint16_t readData16() = 0;
    virtual void writeDataBlock(const uint8_t* data, size_t len) = 0;
    virtual void writeDataBlock16(const uint16_t* data, size_t len) = 0;
    virtual void readDataBlock(uint8_t* data, size_t len) = 0;
    virtual void readDataBlock16(uint16_t* data, size_t len) = 0;

    // Transaction management
    virtual void beginTransaction() = 0;
    virtual void endTransaction() = 0;
    virtual void beginRead() = 0;
    virtual void endRead() = 0;
    virtual void begin_nin_write() = 0;
    virtual void end_nin_write() = 0;

    // DMA support
    virtual bool supportsDMA() = 0;
    virtual bool startDMAWrite(const uint8_t* data, size_t len) = 0;

    // Viewport management
    virtual void setViewport(int32_t x, int32_t y, int32_t w, int32_t h, bool vpDatum = false) = 0;
    virtual void resetViewport() = 0;
    virtual bool checkViewport(int32_t x, int32_t y, int32_t w, int32_t h) = 0;
    virtual bool clipAddrWindow(int32_t* x, int32_t* y, int32_t* w, int32_t* h) = 0;
    virtual bool clipWindow(int32_t* xs, int32_t* ys, int32_t* xe, int32_t* ye) = 0;

protected:
    const Config& _config;
};

} // namespace TFT_Runtime

#endif // _TFT_INTERFACE_H_
