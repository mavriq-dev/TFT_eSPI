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

#if defined(CORE_TEENSY) 
    DMAChannel* _dmaChannel;
    #include <DMAChannel.h>
    #if defined(__IMXRT1062__)
        #include <imxrt.h>
        extern "C" void dmaInterruptHandler(void);  // Forward declaration
    #elif defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__)
        #include <core_pins.h>
        #include <kinetis.h>
        void dma_ch_isr(void);  // Forward declaration
    #endif
#endif


namespace TFT_Runtime {


class TFT_Interface_SPI : public TFT_Interface {
public:
    explicit TFT_Interface_SPI(const Config& config);
    ~TFT_Interface_SPI() override;

    // DMA status enum
    enum DMAStatus {
        DMA_INACTIVE,
        DMA_ACTIVE,
        DMA_COMPLETE
    };

    // #if defined(CORE_TEENSY)
    //     #if defined(__IMXRT1062__)
    //     friend void ::dmaInterruptHandler(void);
    //     #elif defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__)
    //     friend void ::dma_ch_isr(void);
    //     #endif
    // #endif

    // Static instance pointer for interrupt handling
    static TFT_Interface_SPI* _instance;

    // Public method for DMA completion handling
    void notifyDMAComplete() {
        _dmaStatus = DMA_COMPLETE;
    }

    // Static DMA handler method
    static void dmaInterruptHandler() {
        if (_instance) {
            _instance->notifyDMAComplete();
        }
    }

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
    bool init();
    
    // Core write/read functions
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
    void setSPISettings();
    void beginSPITransaction();
    void endSPITransaction();
    void setupFIFO();  // Setup FIFO for Teensy 3.6


    // DMA related members
    bool _dmaInitialized;  // Tracks DMA initialization state across platforms
    volatile DMAStatus _dmaStatus; // Tracks current DMA transfer status

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
    #elif defined(ARDUINO_ARCH_RP2040)
    spi_inst_t* _spi_inst;
    int _dma_tx;
    int _dma_rx;
    #elif defined(CORE_TEENSY)
        #if defined(__IMXRT1062__)  // Teensy 4.0
            IMXRT_LPSPI_t* _pimxrt_spi;  // For backwards compatibility
            IMXRT_LPSPI_t* _lpspi_base;  // Current active LPSPI base
        #elif defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__)  // Teensy 3.x series
            
            uint32_t _dmaBufSize;
            uint8_t* _dmaBuf;
            size_t _dmaRemaining;  // Track remaining bytes to transfer
            size_t _dmaSent;      // Track bytes already sent
            volatile uint32_t* _spiBaseReg;  // Base register for SPI
            volatile uint32_t* _spiSR;       // Status register
            volatile uint32_t* _spiDR;       // Data register
            volatile uint32_t* _spiMCR;      // Module configuration register
        #endif
    #endif

    // Viewport management
    int32_t _vpX, _vpY, _vpW, _vpH;
    bool _vpDatum;
    bool _vpActive;

};

} // namespace TFT_Runtime

#endif // _TFT_INTERFACE_SPI_H_
