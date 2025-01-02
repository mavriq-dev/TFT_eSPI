#ifndef _TFT_INTERFACE_PARALLEL_H_
#define _TFT_INTERFACE_PARALLEL_H_

#include "TFT_Interface.h"

// Platform-specific includes
#if defined(ESP32)
    #include "soc/spi_struct.h"
    #include "esp32/rom/lldesc.h"
    #include "driver/i2s.h"
    #include "driver/gpio.h"
    #include "esp_heap_caps.h"
    #define I2S_PORT I2S_NUM_0  // Define default I2S port for ESP32
#elif defined(ARDUINO_ARCH_RP2040)
    #include "hardware/pio.h"
    #include "hardware/dma.h"
    #include "hardware/clocks.h"
#endif


namespace TFT_Runtime {


class TFT_Interface_Parallel : public TFT_Interface {
public:
    explicit TFT_Interface_Parallel(const Config& config);
    ~TFT_Interface_Parallel() override;
    
    void dmaInterrupt();  // DMA interrupt handler method - available for all platforms

    bool begin() override;
    void writeCommand(uint8_t cmd) override;
    void writeData(uint8_t data) override;
    void writeData16(uint16_t data) override;
    uint8_t readData() override;
    uint16_t readData16() override;
    void writeDataBlock(const uint8_t* data, size_t len) override;
    void writeDataBlock16(const uint16_t* data, size_t len) override;
    void readDataBlock(uint8_t* data, size_t len) override;
    void readDataBlock16(uint16_t* data, size_t len) override;

    void beginTransaction() override;
    void endTransaction() override;
    void beginRead() override;
    void endRead() override;
    void begin_nin_write() override;
    void end_nin_write() override;

    // bool supportsDMA() override;
    bool startDMAWrite(const uint8_t* data, size_t len) override;

    void setViewport(int32_t x, int32_t y, int32_t w, int32_t h, bool vpDatum = false) override;
    void resetViewport() override;
    bool checkViewport(int32_t x, int32_t y, int32_t w, int32_t h) override;
    bool clipAddrWindow(int32_t* x, int32_t* y, int32_t* w, int32_t* h) override;
    bool clipWindow(int32_t* xs, int32_t* ys, int32_t* xe, int32_t* ye) override;

protected:
    // Interface initialization
    bool initInterface();
    void setupPins();
    void setupCommonPins();
    void cleanupDMA();
    void waitDMAComplete();
    void setDataPinsOutput();
    void setDataPinsInput();
    void pulseWR();
    void pulseRD();
    void pulseLatch();
    void delayWrite();
    bool supportsDMA();

    // Low-level write functions
    void write8(uint8_t data);
    void write16(uint16_t data);
    uint8_t read8();
    uint16_t read16();

    // Helper methods
    void setupParallelBus();
    uint8_t getPinBitPosition(uint8_t pin);
    void setSPISettings();
    void beginSPITransaction();
    void endSPITransaction();

    // DMA related members
    bool _dmaInitialized;  // Tracks DMA initialization state across platforms
    bool _dmaBufferReady;

    // Platform-specific DMA members
#if defined(ESP32)
    static const size_t DMA_BUFFER_SIZE = 4096;
    int _dmaChannel;
    lldesc_t* _dmadesc;
    uint8_t* _dmaBuf;
#elif defined(ARDUINO_ARCH_RP2040)
    PIO _pio;
    uint _sm;
    uint _dma_chan;
#endif

private:
    // Pin configuration
    int8_t _csPin;
    int8_t _dcPin;
    int8_t _wrPin;
    int8_t _rdPin;
    int8_t _rstPin;
    int8_t _latchPin;
    int8_t _dataPins[16];  // Support for up to 16-bit parallel
    bool _is16Bit;
    bool _useLatch;
    uint8_t _writeDelay;

    // Viewport management
    int32_t _vpX, _vpY, _vpW, _vpH;
    bool _vpDatum;
    bool _vpActive;

};

} // namespace TFT_Runtime

#endif // _TFT_INTERFACE_PARALLEL_H_
