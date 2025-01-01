#ifndef _TFT_INTERFACE_SPI_H_
#define _TFT_INTERFACE_SPI_H_

#include "TFT_Interface.h"
#include <SPI.h>

// Platform-specific includes
#if defined(ESP32)
    #include "soc/spi_struct.h"
    #include "esp32/rom/lldesc.h"
    #include "soc/spi_reg.h"
    #include "driver/spi_master.h"
#elif defined(ARDUINO_ARCH_RP2040)
    #include "hardware/spi.h"
    #include "hardware/dma.h"
    #include "hardware/clocks.h"
#endif

#if defined(CORE_TEENSY) && defined(__IMXRT1062__)
    #include <imxrt.h>
    #include <DMAChannel.h>
#elif defined(CORE_TEENSY) && defined(__MK66FX1M0__)  // Teensy 3.6
    // #include <imxrt.h>
    #include <DMAChannel.h>
    #include <core_pins.h>
    #include <kinetis.h>
#elif defined(CORE_TEENSY)
    #include <imxrt.h>
    #include <DMAChannel.h>
    #if defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__)
        #include <core_pins.h>
        #include <kinetis.h>
    #endif
#endif

namespace TFT_Runtime {

class TFT_Interface_SPI : public TFT_Interface {
public:
    explicit TFT_Interface_SPI(const Config& config);
    ~TFT_Interface_SPI() override;

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

    // Platform-specific write functions
    #if defined(ESP32)
    void writeESP32_8(uint8_t data);
    void writeESP32_16(uint16_t data);
    #elif defined(ESP8266)
    void writeESP8266_8(uint8_t data);
    void writeESP8266_16(uint16_t data);
    #elif defined(ARDUINO_ARCH_RP2040)
    void writeRP2040_8(uint8_t data);
    void writeRP2040_16(uint16_t data);
    #elif defined(STM32)
    void writeSTM32_8(uint8_t data);
    void writeSTM32_16(uint16_t data);
    #elif defined(ARDUINO_SAM_DUE)
    void writeSAMDUE_8(uint8_t data);
    void writeSAMDUE_16(uint16_t data);
    #elif defined(__AVR__)
    void writeAVR_8(uint8_t data);
    void writeAVR_16(uint16_t data);
    #elif defined(CORE_TEENSY)
    #if defined(__IMXRT1062__)
    void writeTeensy40_8(uint8_t data);
    void writeTeensy40_16(uint16_t data);
    #elif defined(__MK66FX1M0__)
    void writeTeensy36_8(uint8_t data);
    void writeTeensy36_16(uint16_t data);
    #elif defined(__MK64FX512__)
    void writeTeensy35_8(uint8_t data);
    void writeTeensy35_16(uint16_t data);
    #elif defined(__MK20DX256__)
    void writeTeensy32_8(uint8_t data);
    void writeTeensy32_16(uint16_t data);
    #elif defined(__MK20DX128__)
    void writeTeensy30_8(uint8_t data);
    void writeTeensy30_16(uint16_t data);
    #endif
    #endif

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
    void setSPISettings();
    void beginSPITransaction();
    void endSPITransaction();

    // DMA related members
    bool _dmaInitialized;  // Tracks DMA initialization state across platforms

private:
    // Pin configuration
    const int8_t _csPin;
    const int8_t _dcPin;
    const int8_t _mosiPin;
    const int8_t _misoPin;
    const int8_t _sckPin;
    const int8_t _rstPin;
    const uint32_t _spiFreq;
    const uint8_t _spiMode;
    
    // SPI settings
    SPISettings _spiSettings;
    SPIClass* _spi;
    bool _hwSPI;

    // Platform-specific members
    #if defined(ESP32)
        spi_device_handle_t _spi_handle;
        lldesc_t* _dmadesc;
        uint8_t* _dmaBuf;
        bool _dmaInitialized;
    #elif defined(ARDUINO_ARCH_RP2040)
        spi_inst_t* _spi_inst;
        uint _dma_tx;
        uint _dma_rx;
        bool _dmaInitialized;
    #elif defined(CORE_TEENSY) && defined(__IMXRT1062__)
        DMAChannel* _dmaChannel;
        bool _dmaInitialized;
    #elif defined(CORE_TEENSY) && defined(__MK66FX1M0__)  // Teensy 3.6
        DMAChannel* _dmaChannel;
        uint32_t _dmaBufSize;
        uint8_t* _dmaBuf;
        size_t _dmaRemaining;  // Track remaining bytes to transfer
        size_t _dmaSent;      // Track bytes already sent
        volatile uint32_t* _spiBaseReg;  // Base register for SPI
        volatile uint32_t* _spiSR;       // Status register
        volatile uint32_t* _spiDR;       // Data register
        volatile uint32_t* _spiMCR;      // Module configuration register
        enum DMAStatus {
            DMA_INACTIVE = 0,
            DMA_ACTIVE = 1,
            DMA_COMPLETE = 2
        };
        volatile DMAStatus _dmaStatus;  // Track DMA transfer status
        
        // Optimized SPI write functions
        inline void fastSPIwrite(uint8_t data) {
            while (!(*_spiSR & SPI_SR_TFFF)) ; // Wait for transmit FIFO not full
            *_spiDR = data;
        }
        
        inline void fastSPIwrite16(uint16_t data) {
            while (!(*_spiSR & SPI_SR_TFFF)) ; // Wait for transmit FIFO not full
            *_spiDR = data;
        }
        
        // DMA optimization functions
        void initTeensyDMA();
        void setupTeensyFIFO();
        static void _dmaInterruptHandlerTeensy36();
    #endif

    // Viewport management
    int32_t _vpX, _vpY, _vpW, _vpH;
    bool _vpDatum;
    bool _vpActive;

    static TFT_Interface_SPI* _instance;
};

} // namespace TFT_Runtime

#endif // _TFT_INTERFACE_SPI_H_
