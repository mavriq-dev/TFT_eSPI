#ifndef _TFT_INTERFACE_PARALLEL_H_
#define _TFT_INTERFACE_PARALLEL_H_

#include "TFT_Interface.h"

// Platform-specific includes
#if defined(ESP32)
    #include "soc/spi_struct.h"
    #include "esp32/rom/lldesc.h"
#elif defined(ARDUINO_ARCH_RP2040)
    #include "hardware/pio.h"
    #include "hardware/dma.h"
    #include "hardware/clocks.h"
#endif

#if defined(CORE_TEENSY) && defined(__IMXRT1062__)
    #include <imxrt.h>
    #include <FlexIO_t4.h>
    #include <DMAChannel.h>
#endif

#if defined(CORE_TEENSY) && defined(__MK66FX1M0__)  // Teensy 3.6
    #include <kinetis.h>
    #include <DMAChannel.h>
    #include <core_pins.h>
#endif

namespace TFT_Runtime {

class TFT_Interface_Parallel : public TFT_Interface {
public:
    explicit TFT_Interface_Parallel(const Config& config);
    ~TFT_Interface_Parallel() override;

    #if defined(CORE_TEENSY)
    static TFT_Interface_Parallel* getDMAActiveInstance() { return _dmaActiveInstance; }
    void dmaInterrupt();  // Moved to public section
    #endif

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

    bool supportsDMA() override;
    bool startDMAWrite(const uint8_t* data, size_t len) override;

    void setViewport(int32_t x, int32_t y, int32_t w, int32_t h, bool vpDatum = false) override;
    void resetViewport() override;
    bool checkViewport(int32_t x, int32_t y, int32_t w, int32_t h) override;
    bool clipAddrWindow(int32_t* x, int32_t* y, int32_t* w, int32_t* h) override;
    bool clipWindow(int32_t* xs, int32_t* ys, int32_t* xe, int32_t* ye) override;

protected:
    // Platform-specific initialization
    bool initESP32();
    bool initESP8266();
    bool initRP2040();
    bool initSAMDUE();
    bool initAVR();
    bool initTeensy();

    // Platform-specific setup functions
    #if defined(ESP32)
    void setupESP32Parallel();
    #elif defined(ESP8266)
    void setupESP8266Parallel();
    #elif defined(ARDUINO_ARCH_RP2040)
    void setupRP2040Parallel();
    #elif defined(STM32)
    void setupSTM32Parallel();
    #elif defined(ARDUINO_SAM_DUE)
    void setupDueParallel();
    #elif defined(ARDUINO_AVR_MEGA2560)
    void setupMegaParallel();
    #elif defined(CORE_TEENSY)
    #if defined(__IMXRT1062__)
    void setupTeensy40Parallel();
    #elif defined(__MK66FX1M0__)
    void setupTeensy36Parallel();
    void writeTeensy36_8(uint8_t data);
    void writeTeensy36_16(uint16_t data);
    void initTeensy36DMA();
    void setupParallelDMA();
    #elif defined(__MK64FX512__)
    void setupTeensy35Parallel();
    #elif defined(__MK20DX256__)
    void setupTeensy32Parallel();
    #elif defined(__MK20DX128__)
    void setupTeensy30Parallel();
    #endif
    #endif

    // DMA related members
    volatile TFT_Runtime::DMAStatus _dmaStatus = DMA_INACTIVE;  // Track DMA transfer status
    bool _dmaInitialized = false;

    void setupCommonPins();

    // Platform-specific write methods
    void writeESP32_8(uint8_t data);
    void writeESP32_16(uint16_t data);
    void writeESP8266_8(uint8_t data);
    void writeESP8266_16(uint16_t data);
    void writeRP2040_8(uint8_t data);
    void writeRP2040_16(uint16_t data);
    void writeSAMDUE_8(uint8_t data);
    void writeSAMDUE_16(uint16_t data);
    void writeAVR_8(uint8_t data);
    void writeAVR_16(uint16_t data);
    void writeTeensy_8(uint8_t data);
    void writeTeensy_16(uint16_t data);

    // Platform-specific read methods
    uint8_t readESP32_8();
    uint16_t readESP32_16();
    uint8_t readESP8266_8();
    uint16_t readESP8266_16();
    uint8_t readRP2040_8();
    uint16_t readRP2040_16();
    uint8_t readSAMDUE_8();
    uint16_t readSAMDUE_16();
    uint8_t readAVR_8();
    uint16_t readAVR_16();
    uint8_t readTeensy_8();
    uint16_t readTeensy_16();

    // DMA support methods
    void initDMA();
    void cleanupDMA();
    void waitDMAComplete();

    // Helper methods
    void setupPins();
    void setDataPinsOutput();
    void setDataPinsInput();
    void pulseWR();
    void pulseRD();
    void pulseLatch();
    void delayWrite();

private:
    // Platform-specific pin position helper
    static uint8_t getPinBitPosition(uint8_t pin);
    
    bool _dmaActive;  // True if DMA is in progress, false if complete/idle

    #if defined(CORE_TEENSY)
    static TFT_Interface_Parallel* _dmaActiveInstance; // For interrupt handling
    #endif

    // Pin configuration
    const int8_t _csPin;
    const int8_t _dcPin;    // Data/Command pin
    const int8_t _wrPin;
    const int8_t _rdPin;
    const int8_t _rstPin;
    const int8_t _latchPin;
    const bool _is16Bit;
    const bool _useLatch;
    const uint8_t _writeDelay;

    // Data pins array
    int8_t _dataPins[16];  // Support up to 16-bit parallel
    uint32_t _dataShift;  // Data shift value for parallel interface

    // Platform-specific members
#if defined(ESP32)
    int8_t _dmaChannel;
    uint8_t* _dmaBuf;
    lldesc_t* _dmadesc;
#elif defined(ARDUINO_ARCH_RP2040)
    PIO _pio;
    uint _sm;
    uint _dma_chan;
#elif defined(CORE_TEENSY)
    #if defined(__IMXRT1062__)
        IMXRT_FLEXIO_t* _flexIO;
        uint8_t _flexIOShifter;
        uint8_t _flexIOTimer;
        DMAChannel* _dmaChannel;
    #elif defined(__MK66FX1M0__)
        DMAChannel* _dmaChannel;
        uint32_t _dmaBufSize;
        uint8_t* _dmaBuf;
        size_t _dmaRemaining;  // Remaining bytes to transfer
        size_t _dmaSent;       // Bytes sent so far
        
        // Port configuration
        volatile uint8_t* _dataPort;
        uint8_t _dataMask;
        
        // Port registers for direct manipulation
        volatile uint8_t* _portMode;   // Port MODE register (for direction control)
        volatile uint8_t* _portSet;    // Port SET register
        volatile uint8_t* _portClear;  // Port CLEAR register
        volatile uint8_t* _wrPortSet;  // WR pin SET register
        volatile uint8_t* _wrPortClear;// WR pin CLEAR register
        uint8_t _wrPinMask;           // WR pin mask
        uint8_t _dataPortMask;        // Data port mask
        
        // Optimized functions for Teensy 3.6
        void initTeensyParallelDMA();
        void setupParallelBus();
        
        // Fast parallel write functions for Teensy 3.6
        inline void fastParallelWriteTeensy36(uint8_t data) {
            *_dataPort = (*_dataPort & ~_dataMask) | ((uint8_t)data << _dataShift);
        }
        inline void fastParallelWriteTeensy36_16(uint16_t data) {
            // Write high byte first
            *_dataPort = (*_dataPort & ~_dataMask) | ((uint8_t)(data >> 8) << _dataShift);
            // Then low byte
            *_dataPort = (*_dataPort & ~_dataMask) | ((uint8_t)(data & 0xFF) << _dataShift);
        }
        
        const uint8_t* _dmaBuffer;  // Pointer to the data buffer
    #endif
#endif

    // Viewport management
    int32_t _vpX, _vpY, _vpW, _vpH;
    bool _vpDatum;
    bool _vpActive;

    // Optimized write functions
    inline void fastParallelWrite(uint8_t data) {
        *_dataClear = _dataMask;
        *_dataSet = ((uint32_t)data << _dataShift) & _dataMask;
        *_wrClear = _wrPinMask;
        *_wrSet = _wrPinMask;
    }
    
    inline void fastParallelWrite16(uint16_t data) {
        // Write high byte
        *_dataClear = _dataMask;
        *_dataSet = ((uint32_t)(data >> 8) << _dataShift) & _dataMask;
        *_wrClear = _wrPinMask;
        *_wrSet = _wrPinMask;
        
        // Write low byte
        *_dataClear = _dataMask;
        *_dataSet = ((uint32_t)(data & 0xFF) << _dataShift) & _dataMask;
        *_wrClear = _wrPinMask;
        *_wrSet = _wrPinMask;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_INTERFACE_PARALLEL_H_
