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
    void dmaInterrupt();  // DMA interrupt handler method
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
    #if defined(ESP32)
    bool initESP32();
    #endif
    #if defined(ESP8266)
    bool initESP8266();
    #endif
    #if defined(ARDUINO_ARCH_RP2040)
    bool initRP2040();
    #endif
    #if defined(ARDUINO_SAM_DUE)
    bool initSAMDUE();
    #endif
    #if defined(__AVR__)
    bool initAVR();
    #endif
    #if defined(CORE_TEENSY)
    bool initTeensy();
    #endif

    // Low-level write functions
    void write8(uint8_t data);
    void write16(uint16_t data);
    uint8_t read8();
    uint16_t read16();

    // DMA support methods
    void initDMA();
    void cleanupDMA();
    void waitDMAComplete();

    // Helper methods
    void setupPins();
    void setupCommonPins();
    void setupParallelBus();
    uint8_t getPinBitPosition(uint8_t pin);
    void setSPISettings();
    void beginSPITransaction();
    void endSPITransaction();

    // DMA related members
    bool _dmaInitialized;  // Tracks DMA initialization state across platforms

private:
    // Pin configuration
    const int8_t _csPin;
    const int8_t _dcPin;
    const int8_t _wrPin;
    const int8_t _rdPin;
    const int8_t _rstPin;
    const int8_t _latchPin;
    int8_t _dataPins[8];
    bool _is16Bit;
    bool _useLatch;
    uint8_t _writeDelay;

    // Platform-specific members
    #if defined(ESP32)
    int8_t _dmaChannel;
    uint8_t* _dmaBuf;
    lldesc_t* _dmadesc;
    bool _dmaInitialized;
    #elif defined(ARDUINO_ARCH_RP2040)
    PIO _pio;
    uint _sm;
    uint _dma_chan;
    bool _pioInitialized;
    #elif defined(CORE_TEENSY)
    #if defined(__IMXRT1062__)
    FLEXIO_Type* _flexIO;
    uint8_t _flexIOShifter;
    uint8_t _flexIOTimer;
    DMAChannel* _dmaChannel;
    #elif defined(__MK66FX1M0__)  // Teensy 3.6
    DMAChannel* _dmaChannel;
    uint32_t _dmaBufSize;
    uint8_t* _dmaBuf;
    size_t _dmaRemaining;
    size_t _dmaSent;
    volatile uint32_t* _spiBaseReg;
    volatile uint32_t* _spiSR;
    volatile uint32_t* _spiDR;
    volatile uint32_t* _spiMCR;
    volatile DMAStatus _dmaStatus;  // Using base class DMAStatus
    static TFT_Interface_Parallel* _dmaActiveInstance;

    // Direct port manipulation registers
    volatile uint32_t* _dataPort;     // Data port register
    volatile uint32_t* _dataPortSet;  // Data port SET register
    volatile uint32_t* _dataPortClr;  // Data port CLEAR register
    volatile uint32_t* _wrPortSet;    // WR port SET register
    volatile uint32_t* _wrPortClear;  // WR port CLEAR register
    uint32_t _dataMask;              // Data port mask
    uint32_t _wrPinMask;             // WR pin mask
    uint32_t _dataShift;             // Data shift value
    #endif
    #endif

    // Viewport management
    int32_t _vpX, _vpY, _vpW, _vpH;
    bool _vpDatum;
    bool _vpActive;

    // Helper methods
    void setDataPinsOutput();
    void setDataPinsInput();
    void pulseWR();
    void pulseRD();
    void pulseLatch();
    void delayWrite();
};

} // namespace TFT_Runtime

#endif // _TFT_INTERFACE_PARALLEL_H_
