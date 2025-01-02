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

#if defined(CORE_TEENSY)    
    #include <DMAChannel.h>
    #if defined(__IMXRT1062__)
        #include <imxrt.h>
        #include "FlexIOHandler.h"
    #elif defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__)
        #include <kinetis.h>
        #include <core_pins.h>
    #endif
#endif

namespace TFT_Runtime {

#if defined(CORE_TEENSY)
void dmaInterruptHandler(void);  // Forward declaration without namespace qualification
#endif

class TFT_Interface_Parallel : public TFT_Interface {
public:
    explicit TFT_Interface_Parallel(const Config& config);
    ~TFT_Interface_Parallel() override;

    #if defined(CORE_TEENSY)
    friend void dmaInterruptHandler(void);  // Friend declaration without namespace qualification
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

//     // Platform-specific members
// #if defined(CORE_TEENSY)
//     #if defined(__IMXRT1062__)
//         FlexIOHandler* _flexIO;
//         uint8_t _flexIOShifter;
//         uint8_t _flexIOTimer;
//     #elif defined(__MK66FX1M0__) || defined(__MK64FX512__)  // Teensy 3.6/3.5
//         // volatile uint32_t* _dataPort;       // GPIO port for data pins
//         // uint32_t _dataPinMask;             // Mask for data pins in the port
//         // uint32_t _dataPinShift;            // Shift needed to align data pins
//     #endif
//     // DMAChannel* _dmaChannel;
// #endif

    // DMA related members
    bool _dmaInitialized;  // Tracks DMA initialization state across platforms
    // DMAStatus _dmaStatus;

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

#if defined(CORE_TEENSY) && defined(__IMXRT1062__)
    // Teensy 4.0/4.1 specific members
    FlexIOHandler* _flexIO;
    uint8_t _flexIOShifter;
    uint8_t _flexIOTimer;
#endif

#if defined(CORE_TEENSY) && (defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__))
    // Kinetisk (Teensy 3.x) specific variables for port manipulation
    volatile uint32_t* _dataPort;
    volatile uint32_t* _dataPortSet;
    volatile uint32_t* _dataPortClr;
    uint32_t _dataPinMask;             // Mask for data pins in the port
    uint32_t _dataPinShift;            // Shift needed to align data pins
    volatile uint32_t* _wrPort;
    volatile uint32_t* _wrPortSet;
    volatile uint32_t* _wrPortClear;
    uint8_t _basePort;                 // Base port for data pins
    bool _pinsOnSamePort;              // Flag indicating if all pins are on same port
    volatile uint32_t* _rdPort;      // Added for read support
    volatile uint32_t* _rdPortSet;   // Added for read support
    volatile uint32_t* _rdPortClear; // Added for read support
    volatile uint32_t* _dcPort;      // Added for DC pin support
    volatile uint32_t* _dcPortSet;   // Added for DC pin support
    volatile uint32_t* _dcPortClear; // Added for DC pin support
    volatile uint32_t* _csPort;      // Added for CS pin support
    volatile uint32_t* _csPortSet;   // Added for CS pin support
    volatile uint32_t* _csPortClear; // Added for CS pin support
    uint32_t _dataMask;
    uint32_t _wrPinMask;
    uint32_t _rdPinMask;            // Added for read support
    uint32_t _dcPinMask;            // Added for DC pin support
    uint32_t _csPinMask;            // Added for CS pin support
    uint8_t _dataShift;
#endif

    // DMA support
#if defined(CORE_TEENSY)
    static TFT_Interface_Parallel* _dmaActiveInstance;
    DMAChannel* _dmaChannel;
    uint8_t* _dmaBuffer1;
    uint8_t* _dmaBuffer2;
    uint8_t* _currentDmaBuffer;
    bool _dmaBufferReady;
    static const size_t DMA_BUFFER_SIZE = 4096;
#endif

    // Viewport management
    int32_t _vpX, _vpY, _vpW, _vpH;
    bool _vpDatum;
    bool _vpActive;

};

} // namespace TFT_Runtime

#endif // _TFT_INTERFACE_PARALLEL_H_
