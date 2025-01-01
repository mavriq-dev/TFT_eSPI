#ifndef _TFT_INTERFACE_PARALLEL_H_
#define _TFT_INTERFACE_PARALLEL_H_

#include "TFT_Interface.h"
#include <Arduino.h>
#if defined(CORE_TEENSY)
#include <DMAChannel.h>
#endif

namespace TFT_Runtime {

class TFT_Interface_Parallel : public TFT_Interface {
public:
    explicit TFT_Interface_Parallel(const Config& config);
    ~TFT_Interface_Parallel() override;

    bool begin() override;
    void end() override;

    void writeCommand(uint8_t cmd) override;
    void writeData(uint8_t data) override;
    void writeData16(uint16_t data) override;
    uint8_t readData() override;
    uint16_t readData16() override;

    void writeDataBlock(const uint8_t* data, size_t len) override;
    void writeDataBlock16(const uint16_t* data, size_t len) override;
    void readDataBlock(uint8_t* data, size_t len) override;
    void readDataBlock16(uint16_t* data, size_t len) override;

    void setCS(bool level) override;
    void setDC(bool level) override;
    void setRST(bool level) override;

    bool supportsDMA() const override;
    bool startDMAWrite(const uint8_t* data, size_t len) override;
    bool startDMAWrite16(const uint16_t* data, size_t len) override;
    bool isDMABusy() const override;
    void waitDMAComplete() override;

    bool supportsBuffering() const override;
    void startBuffer() override;
    void endBuffer() override;
    void flushBuffer() override;

    // Transaction management
    void beginTransaction();
    void endTransaction();
    void beginRead();
    void endRead();
    
    // Non-inlined versions for override
    virtual void begin_nin_write();
    virtual void end_nin_write();
    
    // Viewport management
    void setViewport(int32_t x, int32_t y, int32_t w, int32_t h, bool vpDatum = false);
    void resetViewport();
    bool checkViewport(int32_t x, int32_t y, int32_t w, int32_t h);
    int32_t getViewportX() const { return _vpX; }
    int32_t getViewportY() const { return _vpY; }
    int32_t getViewportWidth() const { return _vpW; }
    int32_t getViewportHeight() const { return _vpH; }
    bool getViewportDatum() const { return _vpDatum; }
    
    // Clipping helpers
    bool clipAddrWindow(int32_t* x, int32_t* y, int32_t* w, int32_t* h);
    bool clipWindow(int32_t* xs, int32_t* ys, int32_t* xe, int32_t* ye);

private:
    // Pin configuration
    int8_t _csPin;
    int8_t _dcPin;     // Added DC pin
    int8_t _wrPin;
    int8_t _rdPin;
    int8_t _rstPin;
    int8_t _latchPin;
    int8_t _dataPins[16];  // D0-D15 pins
    bool _is16Bit;
    bool _useLatch;
    uint32_t _writeDelay;
    bool _dmaEnabled;
    bool _buffering;

    // Buffer management
    static constexpr size_t BUFFER_SIZE = 1024;
    uint8_t* _buffer;
    size_t _bufferIndex;

    // Viewport variables
    int32_t _vpX, _vpY;     // Viewport top left corner
    int32_t _vpW, _vpH;     // Viewport dimensions
    int32_t _xDatum;        // Datum x position
    int32_t _yDatum;        // Datum y position
    bool _vpDatum;          // Datum position in viewport
    bool _vpOoB;           // Out of Bounds flag

    // Platform-specific optimizations
    #if defined(ESP32)
        // ESP32 specific parallel bus handling
        int _dataPortIdx[2];  // Two ports for 16-bit mode
        uint32_t _dataPortMask[2];
        uint32_t _dcPinMask;  // Added DC pin mask
        volatile uint32_t* _dataPortSet[2];
        volatile uint32_t* _dataPortClr[2];
        volatile uint32_t* _wrPortSet;
        volatile uint32_t* _wrPortClr;
        volatile uint32_t* _rdPortSet;
        volatile uint32_t* _rdPortClr;
        volatile uint32_t* _latchPortSet;
        volatile uint32_t* _latchPortClr;
        // DMA channels
        int _dmaChannel;
        uint8_t* _dmaBuf;
        size_t _dmaBufSize;
        lldesc_t* _dmadesc;
        bool _dmaInitialized;
    #elif defined(ESP8266)
        // ESP8266 specific optimizations
        uint32_t _dataPortMask;
        uint32_t _wrBit;
        uint32_t _rdBit;
        uint32_t _dataBits[16];
    #elif defined(ARDUINO_ARCH_RP2040)
        // RP2040 specific optimizations
        PIO _pio;
        uint _sm;
        uint _dma_chan;
        uint32_t _pinMask;
        bool _pioInitialized;
        uint _program_offset;
    #elif defined(ARDUINO_SAM_DUE)
        // SAM DUE specific optimizations
        Pio* _dataPio[2];
        uint32_t _dataPinMask[2];
        Pio* _wrPio;
        uint32_t _wrPinMask;
        Pio* _rdPio;
        uint32_t _rdPinMask;
    #elif defined(__AVR__)
        // AVR specific optimizations
        uint8_t _dataPort[2];
        uint8_t _dataDdr[2];
        uint8_t _dataPin[2];
        uint8_t _wrPort;
        uint8_t _wrDdr;
        uint8_t _wrPin;
        uint8_t _rdPort;
        uint8_t _rdDdr;
        uint8_t _rdPin;
    #elif defined(CORE_TEENSY)
        // Teensy specific optimizations
        #if defined(__IMXRT1062__) // Teensy 4.0/4.1
            IMXRT_GPIO_t* _dataPorts[2];
            uint32_t _dataPinMasks[2];
            IMXRT_GPIO_t* _wrPort;
            uint32_t _wrPinMask;
            IMXRT_GPIO_t* _rdPort;
            uint32_t _rdPinMask;
            // FlexIO for hardware acceleration
            IMXRT_FLEXIO_t* _flexIO;
            uint8_t _flexIOShifter;
            uint8_t _flexIOTimer;
            bool _useFlexIO;
            // DMA support
            DMAChannel* _dmaChannel;
            DMASetting* _dmaSettings;
            uint32_t* _dmaBuf;
            size_t _dmaBufSize;
        #elif defined(__MK66FX1M0__) || defined(__MK64FX512__) // Teensy 3.6/3.5
            volatile uint32_t* _dataPorts[2];
            uint32_t _dataPinMasks[2];
            volatile uint32_t* _wrPort;
            uint32_t _wrPinMask;
            volatile uint32_t* _rdPort;
            uint32_t _rdPinMask;
            // DMA support
            DMAChannel* _dmaChannel;
            uint32_t* _dmaBuf;
            size_t _dmaBufSize;
        #elif defined(__MK20DX256__)
            volatile uint32_t* _dataPorts[2];
            uint32_t _dataPinMasks[2];
            volatile uint32_t* _wrPort;
            uint32_t _wrPinMask;
            volatile uint32_t* _rdPort;
            uint32_t _rdPinMask;
        #endif
    #endif

    void setupPins();
    void initDMA();
    void writeParallelBus8(uint8_t data);
    void writeParallelBus16(uint16_t data);
    uint8_t readParallelBus8();
    uint16_t readParallelBus16();
    void setDataPinsOutput();
    void setDataPinsInput();
    inline void pulseWR();
    inline void pulseRD();
    inline void pulseLatch();
    inline void delayWrite();

    // Platform-specific initialization methods
    #if defined(ESP32)
        bool initESP32();
        void initESP32DMA();
    #elif defined(ESP8266)
        bool initESP8266();
    #elif defined(ARDUINO_ARCH_RP2040)
        bool initRP2040();
        void initRP2040PIO();
    #elif defined(ARDUINO_SAM_DUE)
        bool initSAMDUE();
    #elif defined(__AVR__)
        bool initAVR();
    #elif defined(CORE_TEENSY)
        bool initTeensy();
        #if defined(__IMXRT1062__)
            void initTeensyFlexIO();
            void initTeensyDMA();
        #elif defined(__MK66FX1M0__) || defined(__MK64FX512__)
            void initTeensyDMA();
        #endif
    #endif

    // Platform-specific write methods
    #if defined(ESP32)
        void writeESP32_8(uint8_t data);
        void writeESP32_16(uint16_t data);
        void writeESP32DMA(const uint8_t* data, size_t len);
    #elif defined(ESP8266)
        void writeESP8266_8(uint8_t data);
        void writeESP8266_16(uint16_t data);
    #elif defined(ARDUINO_ARCH_RP2040)
        void writeRP2040_8(uint8_t data);
        void writeRP2040_16(uint16_t data);
        void writeRP2040DMA(const uint8_t* data, size_t len);
    #elif defined(ARDUINO_SAM_DUE)
        void writeSAMDUE_8(uint8_t data);
        void writeSAMDUE_16(uint16_t data);
    #elif defined(__AVR__)
        void writeAVR_8(uint8_t data);
        void writeAVR_16(uint16_t data);
    #elif defined(CORE_TEENSY)
        #if defined(__IMXRT1062__)
            void writeTeensy4_8(uint8_t data);
            void writeTeensy4_16(uint16_t data);
            void writeTeensy4FlexIO(const uint8_t* data, size_t len);
            void writeTeensy4DMA(const uint8_t* data, size_t len);
        #elif defined(__MK66FX1M0__) || defined(__MK64FX512__)
            void writeTeensy3_8(uint8_t data);
            void writeTeensy3_16(uint16_t data);
            void writeTeensy3DMA(const uint8_t* data, size_t len);
        #elif defined(__MK20DX256__)
            void writeTeensy32_8(uint8_t data);
            void writeTeensy32_16(uint16_t data);
        #endif
    #endif
};

} // namespace TFT_Runtime

#endif // _TFT_INTERFACE_PARALLEL_H_
