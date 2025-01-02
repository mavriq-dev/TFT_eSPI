#include "TFT_Interface_SPI.h"
#include <Arduino.h>

#if defined(CORE_TEENSY)
    #if defined(__IMXRT1062__)
    // Teensy 4.x DMA interrupt handler is defined by the core library
    extern "C" void dmaInterruptHandler(void);
    #endif
#endif

namespace TFT_Runtime {

// Initialize static member
TFT_Interface_SPI* TFT_Interface_SPI::_instance = nullptr;

#if defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__)
    // Teensy 3.x DMA interrupt handler
    void dma_ch_isr(void) {
        if (TFT_Runtime::TFT_Interface_SPI::_instance) {
            TFT_Runtime::TFT_Interface_SPI::_instance->notifyDMAComplete();
        }
    }
#endif

TFT_Interface_SPI::TFT_Interface_SPI(const Config& config) : TFT_Interface(config) 
    , _csPin(config.spi.cs_pin)
    , _dcPin(config.spi.dc_pin)
    , _mosiPin(config.spi.mosi_pin)
    , _misoPin(config.spi.miso_pin)
    , _sckPin(config.spi.sck_pin)
    , _rstPin(config.rst_pin)
    , _spiFreq(config.spi.frequency)
    , _spiMode(config.spi.spi_mode)
    , _spiSettings(_spiFreq, MSBFIRST, _spiMode)
    , _spi(&SPI)
    , _hwSPI(true)
    , _vpX(0)
    , _vpY(0)
    , _vpW(config.width)
    , _vpH(config.height)
    , _vpDatum(false)
    , _vpActive(false)
{
    _instance = this;
    #if defined(ESP32)
        _spi_handle = nullptr;
        _dmadesc = nullptr;
        _dmaBuf = nullptr;
        _dmaInitialized = false;
        // Initialize ESP32 specific SPI host
        #if CONFIG_IDF_TARGET_ESP32
            _spi_host = VSPI_HOST;  // Use VSPI by default for ESP32
        #else
            _spi_host = SPI2_HOST;  // For ESP32-S2, ESP32-S3, etc.
        #endif
        // Initialize bus config structure
        memset(&_spi_bus_config, 0, sizeof(_spi_bus_config));
        memset(&_spi_device_config, 0, sizeof(_spi_device_config));
    #elif defined(ARDUINO_ARCH_RP2040)
        _spi_inst = spi0;
        _dma_tx = 0;
        _dma_rx = 1;
        _dmaInitialized = false;

    #if defined(CORE_TEENSY)
        _dmaChannel = nullptr;
        _dmaInitialized = false;
        #if defined(__IMXRT1062__)
            _pimxrt_spi = &IMXRT_LPSPI4_S;  // Initialize SPI hardware registers pointer
            _lpspi_base = nullptr;
        #endif
        #if defined(__MK66FX1M0__)
            _dmaStatus = DMA_INACTIVE;
        #endif
    #endif
#endif
}

TFT_Interface_SPI::~TFT_Interface_SPI() {
    if (_instance == this) {
        _instance = nullptr;
    }
    cleanupDMA();
}

bool TFT_Interface_SPI::begin() {
    setupPins();
    setSPISettings();

    return init();
}

void TFT_Interface_SPI::write8(uint8_t data) {
    #if defined(ESP32)
        #if CONFIG_IDF_TARGET_ESP32
            spi_dev_t* spi_dev = (spi_dev_t*)_spi->bus();
            #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
                spi_dev->data_buf[0] = data;
            #else
                spi_dev->mosi_dlen.val = 8 - 1;
                spi_dev->mosi_data[0] = data;
            #endif
            spi_dev->cmd.usr = 1;
            while (spi_dev->cmd.usr);
        #else
            _spi->write(data);
        #endif
    #elif defined(ESP8266)
        SPI1W0 = data;
        SPI1CMD |= SPIBUSY;
        while(SPI1CMD & SPIBUSY);
    #elif defined(ARDUINO_ARCH_RP2040)
        spi_get_hw(_spi_inst)->dr = (uint32_t)data;
        while (spi_get_hw(_spi_inst)->sr & SPI_SSPSR_BSY_BITS);
    #elif defined(STM32)
        SPI.transfer(data);
    #elif defined(ARDUINO_SAM_DUE)
        SPI.transfer(data);
        while ((SPI0->SPI_SR & SPI_SR_TXEMPTY) == 0);
    #elif defined(__AVR__)
        SPDR = data;
        while(!(SPSR & _BV(SPIF)));
    #elif defined(CORE_TEENSY)
        #if defined(__IMXRT1062__)  // Teensy 4.0
            while ((_lpspi_base->SR & LPSPI_SR_TDF) == 0);
            _lpspi_base->TDR = data;
            while ((_lpspi_base->SR & LPSPI_SR_TCF) == 0);
            _lpspi_base->SR = LPSPI_SR_TCF;
        #elif defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__)
            while (!(KINETISK_SPI0.SR & SPI_SR_TFFF));
            KINETISK_SPI0.PUSHR = data | SPI_PUSHR_CTAS(0);
            #if defined(__MK66FX1M0__)  // Only Teensy 3.6 needs EOQ
                KINETISK_SPI0.PUSHR |= SPI_PUSHR_EOQ;
            #endif
            while (!(KINETISK_SPI0.SR & SPI_SR_TCF));
            KINETISK_SPI0.SR = SPI_SR_TCF;
        #endif
    #else
        _spi->transfer(data);
    #endif
}

void TFT_Interface_SPI::write16(uint16_t data) {
    #if defined(ESP32)
        #if CONFIG_IDF_TARGET_ESP32
            spi_dev_t* spi_dev = (spi_dev_t*)_spi->bus();
            #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
                spi_dev->data_buf[0] = data;
            #else
                spi_dev->mosi_dlen.val = 16 - 1;
                spi_dev->mosi_data[0] = data;
            #endif
            spi_dev->cmd.usr = 1;
            while (spi_dev->cmd.usr);
        #else
            _spi->write16(data);
        #endif
    #elif defined(ESP8266)
        SPI1W0 = data;
        SPI1CMD |= SPIBUSY;
        while(SPI1CMD & SPIBUSY);
    #elif defined(ARDUINO_ARCH_RP2040)
        spi_get_hw(_spi_inst)->dr = (uint32_t)data;
        while (spi_get_hw(_spi_inst)->sr & SPI_SSPSR_BSY_BITS);
    #elif defined(STM32)
        SPI.transfer16(data);
    #elif defined(ARDUINO_SAM_DUE)
        SPI.transfer16(data);
        while ((SPI0->SPI_SR & SPI_SR_TXEMPTY) == 0);
    #elif defined(__AVR__)
        SPDR = data >> 8;
        while(!(SPSR & _BV(SPIF)));
        SPDR = data;
        while(!(SPSR & _BV(SPIF)));
    #elif defined(CORE_TEENSY)
        #if defined(__IMXRT1062__)  // Teensy 4.0
            while ((_lpspi_base->SR & LPSPI_SR_TDF) == 0);
            _lpspi_base->TDR = data;
            while ((_lpspi_base->SR & LPSPI_SR_TCF) == 0);
            _lpspi_base->SR = LPSPI_SR_TCF;
        #elif defined(__MK66FX1M0__)  // Teensy 3.6
            // Wait if FIFO is full
            while (!(KINETISK_SPI0.SR & SPI_SR_TFFF));
            KINETISK_SPI0.PUSHR = data | SPI_PUSHR_CTAS(1) | SPI_PUSHR_EOQ;
        #elif defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__)
            while (!(KINETISK_SPI0.SR & SPI_SR_TFFF));
            KINETISK_SPI0.PUSHR = data;
            while (!(KINETISK_SPI0.SR & SPI_SR_TCF));
            KINETISK_SPI0.SR = SPI_SR_TCF;
        #endif
    #else
        _spi->transfer16(data);
    #endif
}


void TFT_Interface_SPI::writeCommand(uint8_t cmd) {
    beginTransaction();
    digitalWrite(_dcPin, LOW);
    digitalWrite(_csPin, LOW);
    write8(cmd);
    digitalWrite(_csPin, HIGH);
    digitalWrite(_dcPin, HIGH);
}

void TFT_Interface_SPI::writeData(uint8_t data) {
    beginTransaction();
    digitalWrite(_csPin, LOW);
    write8(data);
    digitalWrite(_csPin, HIGH);
}

void TFT_Interface_SPI::writeData16(uint16_t data) {
    beginTransaction();
    digitalWrite(_csPin, LOW);
    write16(data);
    digitalWrite(_csPin, HIGH);
}

void TFT_Interface_SPI::writeDataBlock(const uint8_t* data, size_t len) {
    if (len == 0) return;

    #if defined(ESP32) || defined(ARDUINO_ARCH_RP2040) || defined(CORE_TEENSY)
    if (supportsDMA() && startDMAWrite(data, len)) {
        return;
    }
    #endif

    beginTransaction();
    digitalWrite(_csPin, LOW);
    
    for (size_t i = 0; i < len; i++) {
        write8(data[i]);
    }
    
    digitalWrite(_csPin, HIGH);
}

uint8_t TFT_Interface_SPI::readData() {
    beginRead();
    uint8_t data = _spi->transfer(0);
    endRead();
    return data;
}

uint16_t TFT_Interface_SPI::readData16() {
    beginRead();
    uint16_t data = _spi->transfer16(0);
    endRead();
    return data;
}

void TFT_Interface_SPI::readDataBlock(uint8_t* data, size_t len) {
    if (len == 0) return;
    beginRead();
    _spi->transfer(data, len);
    endRead();
}

void TFT_Interface_SPI::readDataBlock16(uint16_t* data, size_t len) {
    if (len == 0) return;
    beginRead();
    for (size_t i = 0; i < len; i++) {
        data[i] = _spi->transfer16(0);
    }
    endRead();
}

void TFT_Interface_SPI::beginTransaction() {
    _spi->beginTransaction(_spiSettings);
}

void TFT_Interface_SPI::endTransaction() {
    _spi->endTransaction();
}

void TFT_Interface_SPI::begin_nin_write() {
    beginTransaction();
}

void TFT_Interface_SPI::end_nin_write() {
    endTransaction();
}

bool TFT_Interface_SPI::supportsDMA() {
    #if defined(ESP32)
    return true;
    #elif defined(ARDUINO_ARCH_RP2040)
    return true;
    #elif defined(CORE_TEENSY)
    #if defined(__IMXRT1062__) || defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__)    
    return true;
    #else
    return false;
    #endif
    #else
    return false;
    #endif
}

void TFT_Interface_SPI::initDMA() {
#if defined(ESP32)
    if (_dmaInitialized) return;

    // Allocate DMA buffer with error checking
    _dmaBuf = (uint8_t*)heap_caps_malloc(TFT_DMA_MAX_BLOCK_SIZE, MALLOC_CAP_DMA);
    if (!_dmaBuf) {
        log_e("Failed to allocate DMA buffer");
        return;
    }

    // Allocate DMA descriptor with error checking
    _dmadesc = (lldesc_t*)heap_caps_malloc(sizeof(lldesc_t), MALLOC_CAP_DMA);
    if (!_dmadesc) {
        log_e("Failed to allocate DMA descriptor");
        heap_caps_free(_dmaBuf);
        _dmaBuf = nullptr;
        return;
    }

    // Configure SPI bus
    _spi_bus_config.flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_IOMUX_PINS;
    _spi_bus_config.mosi_io_num = _mosiPin;
    _spi_bus_config.miso_io_num = _misoPin;
    _spi_bus_config.sclk_io_num = _sckPin;
    _spi_bus_config.quadwp_io_num = -1;
    _spi_bus_config.quadhd_io_num = -1;
    _spi_bus_config.max_transfer_sz = TFT_DMA_MAX_BLOCK_SIZE;

    esp_err_t ret = spi_bus_initialize(_spi_host, &_spi_bus_config, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        log_e("SPI bus initialization failed: %d", ret);
        heap_caps_free(_dmadesc);
        heap_caps_free(_dmaBuf);
        _dmadesc = nullptr;
        _dmaBuf = nullptr;
        return;
    }

    // Configure SPI device
    _spi_device_config.clock_speed_hz = _spiFreq;
    _spi_device_config.mode = _spiMode;
    _spi_device_config.spics_io_num = _csPin;
    _spi_device_config.queue_size = 1;
    _spi_device_config.flags = SPI_DEVICE_NO_DUMMY;
    _spi_device_config.pre_cb = nullptr;
    _spi_device_config.post_cb = nullptr;

    ret = spi_bus_add_device(_spi_host, &_spi_device_config, &_spi_handle);
    if (ret != ESP_OK) {
        log_e("SPI device addition failed: %d", ret);
        spi_bus_free(_spi_host);
        heap_caps_free(_dmadesc);
        heap_caps_free(_dmaBuf);
        _dmadesc = nullptr;
        _dmaBuf = nullptr;
        return;
    }

    _dmaInitialized = true;
#elif defined(ARDUINO_ARCH_RP2040)
    _spi_inst = spi0;
    _dma_tx = 0;
    _dma_rx = 1;
    _dmaInitialized = false;
#elif defined(CORE_TEENSY)
    if (_dmaInitialized) return;

    _dmaChannel = new DMAChannel();
    if (_dmaChannel) {
        _dmaChannel->disable();
        
        // Set up SPI registers for DMA
        #if defined(__IMXRT1062__)  // Teensy 4.x
            _spiBaseReg = &IMXRT_LPSPI4_S.SR;
            _spiSR = &IMXRT_LPSPI4_S.SR;
            _spiDR = &IMXRT_LPSPI4_S.TDR;
            _spiMCR = &IMXRT_LPSPI4_S.CR;

            // IMXRT1062 specific SPI configuration
            uint32_t ccr = IMXRT_LPSPI4_S.CCR;
            ccr &= ~(LPSPI_CCR_SCKDIV_MASK);
            ccr |= LPSPI_CCR_SCKDIV(1);
            IMXRT_LPSPI4_S.CCR = ccr;

            // Configure FIFO and DMA settings for Teensy 4.x
            IMXRT_LPSPI4_S.FCR = LPSPI_FCR_TXWATER(15); // TX FIFO watermark
            IMXRT_LPSPI4_S.DER = LPSPI_DER_TDDE; // TX DMA Request Enable

            _dmaChannel->destination(*(volatile uint32_t*)&IMXRT_LPSPI4_S.TDR);
            _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_LPSPI4_TX);
            _dmaChannel->attachInterrupt(dmaInterruptHandler);
        #else  // Teensy 3.x
            // Basic DMA setup that doesn't depend on transfer size
            #if defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__)
                _dmaChannel->destination(*(volatile uint32_t*)&SPI0_PUSHR);
            #endif
            _dmaChannel->disableOnCompletion();
            #if defined(__IMXRT1062__)  // Teensy 4.x
                _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_LPSPI4_TX);
            #elif defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__)
                _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_SPI0_TX);
            #endif
            #if defined(__IMXRT1062__)
                _dmaChannel->attachInterrupt(dmaInterruptHandler);
            #else
                _dmaChannel->attachInterrupt(::dma_ch_isr);
            #endif
            
            // Set up SPI registers for DMA
            _spiBaseReg = &SPI0_SR;
            _spiSR = &SPI0_SR;
            _spiDR = &SPI0_PUSHR;
            _spiMCR = &SPI0_MCR;

            // Optimize SPI for DMA operation
            uint32_t mcr = SPI0_MCR;
            mcr &= ~SPI_MCR_MDIS;  // Enable SPI
            mcr |= SPI_MCR_HALT;   // Halt transfers during configuration
            *_spiMCR = mcr;
            
            // Configure FIFO if available
            #if defined(__MK66FX1M0__) || defined(__MK64FX512__)  // Teensy 3.5/3.6
                #define SPI_HAS_FIFO 1
                #define SPI_FIFO_DEPTH 4
                // Enhanced SPI configuration
                mcr &= ~(SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF);  // Enable FIFOs
                mcr |= SPI_MCR_CLR_TXF | SPI_MCR_CLR_RXF;     // Clear FIFOs
                *_spiMCR = mcr;
                mcr &= ~(SPI_MCR_CLR_TXF | SPI_MCR_CLR_RXF);  // Clear FIFO clear flags
                *_spiMCR = mcr;
                
                // Configure TX FIFO watermark for optimal DMA operation
                uint32_t rser = SPI_RSER_TFFF_RE | SPI_RSER_TFFF_DIRS;  // Enable DMA on TX FIFO
                *(_spiBaseReg + 0x1C/4) = rser;
                *(_spiBaseReg + 0x24/4) = 0;  // Set TX FIFO watermark to 0 for immediate triggers
                
            #elif defined(__MK20DX256__)     // Teensy 3.1/3.2
                #define SPI_HAS_FIFO 1
                #define SPI_FIFO_DEPTH 3
                // Standard FIFO configuration
                mcr &= ~(SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF);  // Enable FIFOs
                mcr |= SPI_MCR_CLR_TXF | SPI_MCR_CLR_RXF;     // Clear FIFOs
                *_spiMCR = mcr;
                mcr &= ~(SPI_MCR_CLR_TXF | SPI_MCR_CLR_RXF);  // Clear FIFO clear flags
                *_spiMCR = mcr;
                
                // Configure TX FIFO for standard operation
                uint32_t rser = SPI_RSER_RFDF_RE | SPI_RSER_RFDF_DIRS;
                *(_spiBaseReg + 0x1C/4) = rser;
                *(_spiBaseReg + 0x20/4) = 0; // Clear status
                *(_spiBaseReg + 0x24/4) = 0; // Use default watermarks
            #elif defined(__MK20DX128__)  // Teensy 3.0
                // Teensy 3.0 requires careful handling
                #if defined(SPI_SR_TXCTR) && defined(SPI_MCR_DIS_TXF)
                    #define SPI_HAS_FIFO 1
                    #define SPI_FIFO_DEPTH 3
                    // Only enable FIFO if hardware supports it
                    mcr &= ~(SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF);
                    mcr |= SPI_MCR_CLR_TXF | SPI_MCR_CLR_RXF;
                    *_spiMCR = mcr;
                    mcr &= ~(SPI_MCR_CLR_TXF | SPI_MCR_CLR_RXF);
                    *_spiMCR = mcr;
                    
                    uint32_t rser = SPI_RSER_TFFF_RE | SPI_RSER_TFFF_DIRS;  // Enable DMA on TX FIFO
                    *(_spiBaseReg + 0x1C/4) = rser;
                    *(_spiBaseReg + 0x24/4) = 0;
                #else
                    #define SPI_HAS_FIFO 0
                    // Basic SPI configuration without FIFO
                    *_spiMCR = mcr;
                #endif
            #endif
        
    // Configure SPI for optimal performance
        // #if defined(__IMXRT1062__)  // Teensy 4.x
        //     // IMXRT1062 specific SPI configuration
        //     uint32_t ccr = IMXRT_LPSPI4_S.CCR;
        //     ccr &= ~(LPSPI_CCR_SCKDIV_MASK);
        //     ccr |= LPSPI_CCR_SCKDIV(1);
        //     IMXRT_LPSPI4_S.CCR = ccr;

        //     // Configure FIFO and DMA settings for Teensy 4.x
        //     IMXRT_LPSPI4_S.FCR = LPSPI_FCR_TXWATER(15); // TX FIFO watermark
        //     IMXRT_LPSPI4_S.DER = LPSPI_DER_TDDE; // TX DMA Request Enable

        //     _dmaChannel->destination(*(volatile uint32_t*)&IMXRT_LPSPI4_S.TDR);
        //     _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_LPSPI4_TX);
        //     _dmaChannel->attachInterrupt(dmaInterruptHandler);
        // #else  // Teensy 3.x
        //     // Define SPI register masks if not already defined
        //     #ifndef SPI_CTAR_BR_MASK
        //         #define SPI_CTAR_BR_MASK    (0x0F << 0)
        //         #define SPI_CTAR_DBR        (1 << 31)
        //         #define SPI_CTAR_BR(n)      ((n) & 0x0F)
        //     #endif

        //     uint32_t ctar = SPI0_CTAR0;
        //     ctar &= ~(SPI_CTAR_BR_MASK | SPI_CTAR_DBR);  // Clear baud rate settings
        //     #if defined(__MK66FX1M0__) || defined(__MK64FX512__)  // Teensy 3.5/3.6
        //         ctar |= SPI_CTAR_BR(0) | SPI_CTAR_DBR;  // Fastest possible speed
        //     #else  // Teensy 3.0/3.1/3.2
        //         ctar |= SPI_CTAR_BR(1);  // Half speed
        //     #endif
        //     SPI0_CTAR0 = ctar;
        // #endif
    #endif

        // Clear all status flags
        #if defined(KINETISK)  // Teensy 3.0, 3.1, 3.2, 3.5, 3.6
            SPI0_SR = SPI_SR_TCF | SPI_SR_EOQF | SPI_SR_TFUF | SPI_SR_TFFF | SPI_SR_RFOF | SPI_SR_RFDF;
            // Enable DMA request on TX FIFO
            SPI0_RSER = SPI_RSER_TFFF_RE | SPI_RSER_TFFF_DIRS;
        #elif defined(__IMXRT1062__)  // Teensy 4.x
            IMXRT_LPSPI4_S.SR = LPSPI_SR_TCF | LPSPI_SR_FCF | LPSPI_SR_WCF;
            // Enable DMA request
            IMXRT_LPSPI4_S.DER = LPSPI_DER_TDDE;
        #endif
        
    // // Configure SPI for continuous transfer
    //     #if defined(__IMXRT1062__)  // Teensy 4.x
    //         // IMXRT1062 specific SPI configuration
    //         uint32_t ccr = IMXRT_LPSPI4_S.CCR;
    //         ccr &= ~(LPSPI_CCR_SCKDIV_MASK);
    //         ccr |= LPSPI_CCR_SCKDIV(1);
    //         IMXRT_LPSPI4_S.CCR = ccr;
    //     #else  // Teensy 3.x
    //         // Define SPI register masks if not already defined
    //         #ifndef SPI_CTAR_BR_MASK
    //             #define SPI_CTAR_BR_MASK    (0x0F << 0)
    //             #define SPI_CTAR_DBR        (1 << 31)
    //             #define SPI_CTAR_BR(n)      ((n) & 0x0F)
    //         #endif

    //         uint32_t ctar = SPI0_CTAR0;
    //         ctar &= ~(SPI_CTAR_BR_MASK | SPI_CTAR_DBR);  // Clear baud rate settings
    //         #if defined(__MK66FX1M0__) || defined(__MK64FX512__)  // Teensy 3.5/3.6
    //             ctar |= SPI_CTAR_BR(0) | SPI_CTAR_DBR;  // Fastest possible speed
    //         #else  // Teensy 3.0/3.1/3.2
    //             ctar |= SPI_CTAR_BR(1);  // Half speed
    //         #endif
    //         SPI0_CTAR0 = ctar;
    //     #endif
        
    // Re-enable SPI
        // #if defined(__IMXRT1062__)  // Teensy 4.x
        //     uint32_t mcr = IMXRT_LPSPI4_S.CR;
        //     mcr &= ~LPSPI_CR_MDIS;
        //     IMXRT_LPSPI4_S.CR = mcr;
        // #else  // Teensy 3.x
        //     uint32_t mcr = SPI0_MCR;
        //     mcr &= ~SPI_MCR_MDIS;
        //     SPI0_MCR = mcr;
        // #endif
        
        _dmaInitialized = true;
    }
#endif
}

bool TFT_Interface_SPI::startDMAWrite(const uint8_t* data, size_t len) {
#if defined(CORE_TEENSY)
    #if defined(__IMXRT1062__)  // Teensy 4.0
        if (_dmaInitialized && len >= 16) { // Reduced minimum size for DMA
            // Optimize DMA settings for the transfer
            _dmaChannel->disable();
            
            // Configure for optimal burst transfers
            size_t transferSize = (len + 3) & ~3; // Round up to nearest multiple of 4
            _dmaChannel->sourceBuffer((volatile uint8_t*)data, transferSize);
            _dmaChannel->transferSize(4); // Use 32-bit transfers
            _dmaChannel->transferCount(transferSize / 4);
            
            // Optimize LPSPI settings for the transfer
            _lpspi_base->TCR = LPSPI_TCR_CONT | LPSPI_TCR_RXMSK; // Continuous transfer, ignore RX
            _lpspi_base->CFGR1 |= LPSPI_CFGR1_NOSTALL; // Prevent stalling
            
            // Clear status before starting
            _lpspi_base->SR = 0x3F00;
            
            _dmaStatus = DMA_ACTIVE;
            _dmaChannel->enable();
            return true;
        }
    #elif defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__)  // Teensy 3.x series
        if (!_dmaInitialized || !_dmaChannel || _dmaStatus == DMA_ACTIVE) return false;

        // Set minimum transfer size based on model capabilities
        #if defined(__MK66FX1M0__) || defined(__MK64FX512__)  // Teensy 3.5/3.6
            if (len < 32) return false;  // Use regular SPI for small transfers
            #define DMA_OPTIMAL_SIZE 4   // 32-bit transfers
        #elif defined(__MK20DX256__)     // Teensy 3.1/3.2
            if (len < 16) return false;  // Use regular SPI for small transfers
            #define DMA_OPTIMAL_SIZE 2   // 16-bit transfers
        #else                            // Teensy 3.0
            if (len < 8) return false;   // Lower threshold for Teensy 3.0
            #define DMA_OPTIMAL_SIZE 2   // 16-bit transfers
        #endif

        _dmaChannel->disable();
        
        // Ensure proper alignment and size for DMA transfer
        size_t transferSize;
        #if defined(__MK66FX1M0__) || defined(__MK64FX512__)
            // 32-bit alignment for 3.5/3.6
            transferSize = (len + 3) & ~3;
            _dmaChannel->transferSize(4);
            _dmaChannel->transferCount(transferSize / 4);
        #else
            // 16-bit alignment for 3.0/3.1/3.2
            transferSize = (len + 1) & ~1;
            _dmaChannel->transferSize(2);
            _dmaChannel->transferCount(transferSize / 2);
        #endif
        
        // Configure source buffer
        _dmaChannel->sourceBuffer((volatile uint8_t*)data, transferSize);
        
        // Clear status flags before starting
        SPI0_SR = SPI_SR_TCF | SPI_SR_EOQF | SPI_SR_TFUF | SPI_SR_TFFF | SPI_SR_RFOF | SPI_SR_RFDF;
        
        // Configure SPI for continuous transfer
        uint32_t pushr = SPI_PUSHR_CONT;
        #if SPI_HAS_FIFO
            pushr |= SPI_PUSHR_PCS(0);
            
            // Wait for FIFO to be ready
            while (!(SPI0_SR & SPI_SR_TFFF)) {
                // Add timeout if needed
            }
        #endif
        SPI0_PUSHR = pushr;
        
        _dmaStatus = DMA_ACTIVE;
        _dmaChannel->enable();
        return true;
    #endif
#endif
    return false;
}

void TFT_Interface_SPI::cleanupDMA() {
#if defined(__IMXRT1062__)
    if (_dmaChannel) {
        _dmaChannel->disable();
        // Reset TCR
        _lpspi_base->TCR = 0;
        delete _dmaChannel;
        _dmaChannel = nullptr;
    }
#elif defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__)  // Teensy 3.x series
    if (_dmaChannel) {
        _dmaChannel->disable();
        delete _dmaChannel;
        _dmaChannel = nullptr;
    }
#endif
}

void TFT_Interface_SPI::waitDMAComplete() {
#if defined(__IMXRT1062__)
    if (_dmaInitialized) {
        while (!_dmaChannel->complete()) {
            // Use ARM wait instruction directly
            asm volatile("wfi");
        }
        // Reset TCR after transfer
        _lpspi_base->TCR = 0;
        _dmaChannel->clearComplete();
    }
#elif defined(__MK66FX1M0__) || defined(__MK64FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__)  // Teensy 3.x series
        if (_dmaStatus == DMA_ACTIVE) {
        while (!_dmaChannel->complete()) {
            // Wait for DMA to complete
        }
        _dmaChannel->clearComplete();
        _dmaStatus = DMA_COMPLETE;
        digitalWriteFast(_csPin, HIGH);  // Chip select inactive
    }
#endif
}


bool TFT_Interface_SPI::init() {
#if defined(CORE_TEENSY)
    #if defined(__IMXRT1062__)
        if (supportsDMA()) {
            _dmaChannel = new DMAChannel();
            if (_dmaChannel) {
                _dmaChannel->begin();
                // Configure DMA for optimal performance on Teensy 4/4.1
                _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_LPSPI4_TX);
                _dmaChannel->transferSize(4); // 32-bit transfers for better throughput
                _dmaChannel->transferCount(1);
                _dmaChannel->disableOnCompletion();
                #if defined(__IMXRT1062__)  // Teensy 4.x
                    _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_LPSPI4_TX);
                #elif defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__) // Teensy 3.x
                    _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_SPI0_TX);
                #elif defined(ESP32)
                    // ESP32 uses different DMA configuration - handled elsewhere
                #elif defined(ESP8266)
                    // ESP8266 uses different DMA configuration - handled elsewhere
                #elif defined(ARDUINO_ARCH_STM32)
                    // STM32 specific DMA configuration would go here
                #elif defined(ARDUINO_ARCH_RP2040)
                    // RP2040 specific DMA configuration would go here
                #elif defined(ARDUINO_SAM_DUE)
                    // Due specific DMA configuration would go here
                #elif defined(ARDUINO_AVR_MEGA2560)
                    // Mega specific DMA configuration would go here
                #endif
                #if defined(__IMXRT1062__)
                    _dmaChannel->attachInterrupt(dmaInterruptHandler);
                #else
                    _dmaChannel->attachInterrupt(::dma_ch_isr);
                #endif
                
                // Enable FLEXIO for faster GPIO operations
                CCM_CCGR5 |= (3 << 6);  // FLEXIO2 clock gate, set to 11 (always enabled)
                
                // Configure SPI for maximum speed
                _spi->beginTransaction(SPISettings(
                    _config.spi.frequency,
                    MSBFIRST,
                    _config.spi.spi_mode
                ));
                
                _dmaInitialized = true;
            }
        }
    #elif defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__)  // Teensy 3.x series
        if (supportsDMA()) {
            initDMA();
            setupFIFO();
        }
#endif
    return true;
#endif
}


void TFT_Interface_SPI::setupFIFO() {
    #if defined(CORE_TEENSY)
        #if defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__)
            // Teensy 3.x series specific FIFO setup
            if (_spiMCR) {  // Check if the register pointer is valid
                uint32_t mcr = *_spiMCR;
                mcr &= ~SPI_MCR_MDIS;
                
                // Configure FIFO watermarks - using direct register access
                // These definitions should be provided by kinetis.h
                #if defined(SPI_CTAR_FMSZ)  // Only if FIFO is supported on this device
                    if (_spiBaseReg) {
                        // Set FIFO watermarks for optimal performance
                        *(_spiBaseReg + 0x1C/4) = SPI_RSER_RFDF_RE | SPI_RSER_RFDF_DIRS; // Enable FIFO
                        *(_spiBaseReg + 0x20/4) = 0; // Clear status
                        *(_spiBaseReg + 0x24/4) = 0; // Use default watermarks
                    }
                #endif
            }
        #endif
    #endif
}

void TFT_Interface_SPI::setupPins() {
    pinMode(_csPin, OUTPUT);
    pinMode(_dcPin, OUTPUT);
    if (_rstPin >= 0) pinMode(_rstPin, OUTPUT);
    
    if (_hwSPI) {
        _spi->begin();
    } else {
        pinMode(_mosiPin, OUTPUT);
        pinMode(_sckPin, OUTPUT);
        if (_misoPin >= 0) pinMode(_misoPin, INPUT);
    }
    
    digitalWrite(_csPin, HIGH);
    digitalWrite(_dcPin, HIGH);
    if (_rstPin >= 0) digitalWrite(_rstPin, HIGH);
}

void TFT_Interface_SPI::setSPISettings() {
    _spiSettings = SPISettings(_spiFreq, MSBFIRST, _spiMode);
}

// Viewport Management
void TFT_Interface_SPI::setViewport(int32_t x, int32_t y, int32_t w, int32_t h, bool vpDatum) {
    _vpX = x;
    _vpY = y;
    _vpW = w;
    _vpH = h;
    _vpDatum = vpDatum;
    _vpActive = true;
}

void TFT_Interface_SPI::resetViewport() {
    _vpActive = false;
}

bool TFT_Interface_SPI::checkViewport(int32_t x, int32_t y, int32_t w, int32_t h) {
    if (!_vpActive) return true;
    return (x >= _vpX && (x + w) <= (_vpX + _vpW) &&
            y >= _vpY && (y + h) <= (_vpY + _vpH));
}

bool TFT_Interface_SPI::clipAddrWindow(int32_t* x, int32_t* y, int32_t* w, int32_t* h) {
    if (!_vpActive) return true;
    
    if (*x < _vpX) { *w -= (_vpX - *x); *x = _vpX; }
    if (*y < _vpY) { *h -= (_vpY - *y); *y = _vpY; }
    
    if ((*x + *w) > (_vpX + _vpW)) *w = _vpX + _vpW - *x;
    if ((*y + *h) > (_vpY + _vpH)) *h = _vpY + _vpH - *y;
    
    return (*w > 0 && *h > 0);
}

bool TFT_Interface_SPI::clipWindow(int32_t* xs, int32_t* ys, int32_t* xe, int32_t* ye) {
    if (!_vpActive) return true;
    
    if (*xs < _vpX) *xs = _vpX;
    if (*ys < _vpY) *ys = _vpY;
    
    if (*xe > (_vpX + _vpW - 1)) *xe = _vpX + _vpW - 1;
    if (*ye > (_vpY + _vpH - 1)) *ye = _vpY + _vpH - 1;
    
    return (*xs <= *xe && *ys <= *ye);
}

} // namespace TFT_Runtime
