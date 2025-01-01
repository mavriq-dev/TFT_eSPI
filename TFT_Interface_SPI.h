#ifndef _TFT_INTERFACE_SPI_H_
#define _TFT_INTERFACE_SPI_H_

#include "TFT_Interface.h"
#include <SPI.h>

namespace TFT_Runtime {

class TFT_Interface_SPI : public TFT_Interface {
public:
    explicit TFT_Interface_SPI(const Config& config);
    ~TFT_Interface_SPI() override;

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

protected:
    // Viewport variables
    int32_t _vpX, _vpY;     // Viewport top left corner
    int32_t _vpW, _vpH;     // Viewport dimensions
    int32_t _xDatum;        // Datum x position
    int32_t _yDatum;        // Datum y position
    bool _vpDatum;          // Datum position in viewport
    bool _vpOoB;           // Out of Bounds flag

    // Display dimensions
    uint16_t _width;
    uint16_t _height;

private:
    SPISettings _spiSettings;
    SPIClass* _spi;
    bool _threeWire;
    bool _inTransaction;
    bool _dmaEnabled;
    bool _buffering;
    
    // Pin caching for faster access
    int8_t _csPin;
    int8_t _dcPin;
    int8_t _rstPin;
    
    // Buffer management
    static constexpr size_t BUFFER_SIZE = 512;
    uint8_t* _buffer;
    size_t _bufferIndex;

    void write(uint8_t data);
    void write16(uint16_t data);
    bool writeDMA(const uint8_t* data, size_t len);
    bool writeDMA16(const uint16_t* data, size_t len);

    // Platform-specific members
    #if defined(ESP32)
        spi_device_handle_t _spi;
        spi_device_interface_config_t _spi_cfg;
        spi_host_device_t _spi_host;
        bool _spiInitialized;
        DMAChannel* _dmaChannel;
        lldesc_t* _dmadesc;
        uint8_t* _dmaBuffer;
        size_t _dmaBufferSize;
    #elif defined(ESP8266)
        uint32_t _spiFreq;
        bool _spiInitialized;
    #elif defined(CORE_TEENSY)
        #if defined(__IMXRT1062__) // Teensy 4.0/4.1
            uint32_t _spiClock;
            uint32_t _spiPort;
            bool _useDMA;
            DMAChannel* _dmaChannel;
            DMASetting* _dmaSettings;
            uint8_t* _dmaBuffer;
            size_t _dmaBufferSize;
            // Hardware SPI registers for direct access
            IMXRT_LPSPI_t* _hardware;
            uint32_t _clock_gate_register;
            uint32_t _clock_gate_mask;
        #elif defined(__MK66FX1M0__) || defined(__MK64FX512__) // Teensy 3.6/3.5
            uint32_t _spiClock;
            bool _useDMA;
            DMAChannel* _dmaChannel;
            uint8_t* _dmaBuffer;
            size_t _dmaBufferSize;
            // Hardware SPI registers
            volatile uint32_t* _spiBaseReg;
            volatile uint32_t* _spiDataReg;
            volatile uint32_t* _spiStatusReg;
        #elif defined(__MK20DX256__)
            uint32_t _spiClock;
            // Hardware SPI registers
            volatile uint32_t* _spiBaseReg;
            volatile uint32_t* _spiDataReg;
            volatile uint32_t* _spiStatusReg;
        #endif
    #elif defined(ARDUINO_ARCH_RP2040)
        spi_inst_t* _spi;
        uint _dma_tx;
        uint _dma_rx;
        dma_channel_config _dma_tx_config;
        dma_channel_config _dma_rx_config;
        uint8_t* _dmaBuffer;
        size_t _dmaBufferSize;
        bool _spiInitialized;
    #elif defined(__SAM3X8E__) || defined(__SAM3X8H__)
        Spi* _spi;
        uint32_t _spiClock;
        bool _spiInitialized;
    #elif defined(__AVR__)
        uint8_t _spiPort;
        uint32_t _spiClock;
        bool _spiInitialized;
    #endif

    // Platform-specific initialization methods
    #if defined(ESP32)
        bool initESP32();
        void initESP32DMA();
    #elif defined(ESP8266)
        bool initESP8266();
    #elif defined(CORE_TEENSY)
        bool initTeensy();
        #if defined(__IMXRT1062__)
            void initTeensyDMA();
            void configureTeensy4DirectSPI();
        #elif defined(__MK66FX1M0__) || defined(__MK64FX512__)
            void initTeensyDMA();
            void configureTeensy3DirectSPI();
        #elif defined(__MK20DX256__)
            void configureTeensy32DirectSPI();
        #endif
    #elif defined(ARDUINO_ARCH_RP2040)
        bool initRP2040();
        void initRP2040DMA();
    #elif defined(__SAM3X8E__) || defined(__SAM3X8H__)
        bool initSAMDUE();
    #elif defined(__AVR__)
        bool initAVR();
    #endif

    // Platform-specific transfer methods
    #if defined(ESP32)
        void writeESP32(uint8_t data);
        void writeESP32DMA(const uint8_t* data, size_t len);
    #elif defined(ESP8266)
        void writeESP8266(uint8_t data);
    #elif defined(CORE_TEENSY)
        #if defined(__IMXRT1062__)
            void writeTeensy4(uint8_t data);
            void writeTeensy4DMA(const uint8_t* data, size_t len);
            void writeTeensy4Direct(uint8_t data);
        #elif defined(__MK66FX1M0__) || defined(__MK64FX512__)
            void writeTeensy3(uint8_t data);
            void writeTeensy3DMA(const uint8_t* data, size_t len);
            void writeTeensy3Direct(uint8_t data);
        #elif defined(__MK20DX256__)
            void writeTeensy32(uint8_t data);
            void writeTeensy32Direct(uint8_t data);
        #endif
    #elif defined(ARDUINO_ARCH_RP2040)
        void writeRP2040(uint8_t data);
        void writeRP2040DMA(const uint8_t* data, size_t len);
    #elif defined(__SAM3X8E__) || defined(__SAM3X8H__)
        void writeSAMDUE(uint8_t data);
    #elif defined(__AVR__)
        void writeAVR(uint8_t data);
    #endif

    void setupPins();
    void initDMA();
};

} // namespace TFT_Runtime

#endif // _TFT_INTERFACE_SPI_H_
