#include "TFT_Interface_SPI.h"
#include <Arduino.h>

// DMA Interrupt Handlers for different platforms
static void _dmaInterruptHandler() {
    #if defined(CORE_TEENSY)
        #if defined(__MK66FX1M0__)  // Teensy 3.6
            if (TFT_Runtime::TFT_Interface_SPI::_instance) {
                TFT_Runtime::TFT_Interface_SPI::_instance->notifyDMAComplete();
            }
        #endif
    #endif

}


namespace TFT_Runtime {

// Initialize static member
TFT_Interface_SPI* TFT_Interface_SPI::_instance = nullptr;

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
    #elif defined(ARDUINO_ARCH_RP2040)
        _spi_inst = spi0;
        _dma_tx = 0;
        _dma_rx = 1;
        _dmaInitialized = false;
    #elif defined(CORE_TEENSY) && defined(__IMXRT1062__)
        _dmaChannel = nullptr;
        _dmaInitialized = false;
    #elif defined(CORE_TEENSY) && defined(__MK66FX1M0__)
        _dmaChannel = nullptr;
        _dmaInitialized = false;
        _dmaStatus = DMA_INACTIVE;
    #elif defined(CORE_TEENSY) && defined(__MK64FX512__)
        _dmaChannel = nullptr;
        _dmaInitialized = false;
    #elif defined(CORE_TEENSY) && defined(__MK20DX256__)
        _dmaChannel = nullptr;
        _dmaInitialized = false;
    #elif defined(CORE_TEENSY) && defined(__MK20DX128__)
        _dmaChannel = nullptr;
        _dmaInitialized = false;
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
        spi_t* spi = _spi->bus();
        spi->dev->mosi_dlen.val = 8 - 1;
        spi->dev->mosi_data[0] = data;
        spi->dev->cmd.usr = 1;
        while (spi->dev->cmd.usr);
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
            while ((_pimxrt_spi->SR & LPSPI_SR_TDF) == 0);
            _pimxrt_spi->TDR = data;
            while ((_pimxrt_spi->SR & LPSPI_SR_TCF) == 0);
            _pimxrt_spi->SR = LPSPI_SR_TCF;
        #elif defined(__MK66FX1M0__)  // Teensy 3.6
            // Wait if FIFO is full
            while (!(KINETISK_SPI0.SR & SPI_SR_TFFF));
            KINETISK_SPI0.PUSHR = data | SPI_PUSHR_CTAS(0) | SPI_PUSHR_EOQ;
        #elif defined(__MK64FX512__)  // Teensy 3.5
            while (!(KINETISK_SPI0.SR & SPI_SR_TFFF));
            KINETISK_SPI0.PUSHR = data;
            while (!(KINETISK_SPI0.SR & SPI_SR_TCF));
            KINETISK_SPI0.SR = SPI_SR_TCF;
        #elif defined(__MK20DX256__)  // Teensy 3.2/3.1
            while (!(KINETISK_SPI0.SR & SPI_SR_TFFF));
            KINETISK_SPI0.PUSHR = data;
            while (!(KINETISK_SPI0.SR & SPI_SR_TCF));
            KINETISK_SPI0.SR = SPI_SR_TCF;
        #elif defined(__MK20DX128__)  // Teensy 3.0
            while (!(KINETISK_SPI0.SR & SPI_SR_TFFF));
            KINETISK_SPI0.PUSHR = data;
            while (!(KINETISK_SPI0.SR & SPI_SR_TCF));
            KINETISK_SPI0.SR = SPI_SR_TCF;
        #endif
    #else
        _spi->transfer(data);
    #endif
}

void TFT_Interface_SPI::write16(uint16_t data) {
    #if defined(ESP32)
        spi_t* spi = _spi->bus();
        spi->dev->mosi_dlen.val = 16 - 1;
        spi->dev->mosi_data[0] = data;
        spi->dev->cmd.usr = 1;
        while (spi->dev->cmd.usr);
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
            while ((_pimxrt_spi->SR & LPSPI_SR_TDF) == 0);
            _pimxrt_spi->TDR = data;
            while ((_pimxrt_spi->SR & LPSPI_SR_TCF) == 0);
            _pimxrt_spi->SR = LPSPI_SR_TCF;
        #elif defined(__MK66FX1M0__)  // Teensy 3.6
            // Wait if FIFO is full
            while (!(KINETISK_SPI0.SR & SPI_SR_TFFF));
            KINETISK_SPI0.PUSHR = data | SPI_PUSHR_CTAS(1) | SPI_PUSHR_EOQ;
        #elif defined(__MK64FX512__)  // Teensy 3.5
            while (!(KINETISK_SPI0.SR & SPI_SR_TFFF));
            KINETISK_SPI0.PUSHR = data;
            while (!(KINETISK_SPI0.SR & SPI_SR_TCF));
            KINETISK_SPI0.SR = SPI_SR_TCF;
        #elif defined(__MK20DX256__)  // Teensy 3.2/3.1
            while (!(KINETISK_SPI0.SR & SPI_SR_TFFF));
            KINETISK_SPI0.PUSHR = data;
            while (!(KINETISK_SPI0.SR & SPI_SR_TCF));
            KINETISK_SPI0.SR = SPI_SR_TCF;
        #elif defined(__MK20DX128__)  // Teensy 3.0
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

void TFT_Interface_SPI::beginRead() {
    beginTransaction();
    digitalWrite(_dcPin, HIGH);
    digitalWrite(_csPin, LOW);
}

void TFT_Interface_SPI::endRead() {
    digitalWrite(_csPin, HIGH);
    endTransaction();
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
    #if defined(__IMXRT1062__) || defined(__MK66FX1M0__)
    return true;
    #else
    return false;
    #endif
    #else
    return false;
    #endif
}

void TFT_Interface_SPI::initDMA() {
#if defined(CORE_TEENSY)
    if (_dmaInitialized) return;

    #if defined(__IMXRT1062__)  // Teensy 4.0
        _dmaChannel = new DMAChannel();
        if (_dmaChannel) {
            _dmaChannel->disable();
            _dmaChannel->destination(IMXRT_LPSPI4_S.TDR);
            _dmaChannel->disableOnCompletion();
            _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_LPSPI4_TX);
        }
        _dmaInitialized = (_dmaChannel != nullptr);
        
    #elif defined(__MK66FX1M0__)  // Teensy 3.6
        _dmaChannel = new DMAChannel();
        if (_dmaChannel) {
            _dmaChannel->disable();
            _dmaChannel->destination(*(volatile uint32_t*)&SPI0_PUSHR);
            _dmaChannel->disableOnCompletion();
            _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_SPI0_TX);
            
            // Enhanced DMA configuration
            _dmaChannel->transferSize(4); // Use 32-bit transfers for better performance
            _dmaChannel->transferCount(1); // Will be updated in startDMAWrite
            _dmaChannel->interruptAtCompletion();
            
            // Set up SPI registers for DMA
            _spiBaseReg = &SPI0_SR;
            _spiSR = &SPI0_SR;
            _spiDR = &SPI0_PUSHR;
            _spiMCR = &SPI0_MCR;

            // Optimize SPI for DMA operation
            uint32_t mcr = SPI0_MCR;
            mcr |= SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF; // Disable FIFOs for direct access
            mcr |= SPI_MCR_PCSIS(0x1F); // Set all CS high
            SPI0_MCR = mcr;

            // Clear all status flags
            SPI0_SR = SPI_SR_TCF | SPI_SR_EOQF | SPI_SR_TFUF | SPI_SR_TFFF | SPI_SR_RFOF | SPI_SR_RFDF;
            
            // Configure for optimal DMA operation
            SPI0_RSER = SPI_RSER_TFFF_RE | SPI_RSER_TFFF_DIRS; // Enable DMA request on TFFF
            
            // Set optimal FIFO watermarks
            SPI0_MCR &= ~(SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF);
            SPI0_RSER |= SPI_RSER_TFFF_RE | SPI_RSER_TFFF_DIRS;
            
            _dmaStatus = DMA_INACTIVE;
            _dmaInitialized = true;
            
            // Attach optimized interrupt handler
            _dmaChannel->attachInterrupt(_dmaInterruptHandler);
        }
    #endif
#endif
}

bool TFT_Interface_SPI::startDMAWrite(const uint8_t* data, size_t len) {
#if defined(CORE_TEENSY)
    #if defined(__IMXRT1062__)  // Teensy 4.0
        if (_dmaInitialized && len >= 32) {
            // Optimize DMA settings for the transfer
            _dmaChannel->disable();
            _dmaChannel->sourceBuffer((volatile uint8_t*)data, len);
            _dmaChannel->destination(IMXRT_LPSPI4_S.TDR);
            _dmaChannel->transferSize(4); // Use 32-bit transfers
            _dmaChannel->transferCount(len / 4);
            
            // Enable TCR optimization for continuous transfer
            IMXRT_LPSPI4_S.TCR = LPSPI_TCR_CONT;
            
            _dmaChannel->enable();
            return true;
        }
    #elif defined(__MK66FX1M0__)  // Teensy 3.6
        if (!_dmaInitialized || !_dmaChannel || _dmaStatus == DMA_ACTIVE) return false;

        // Calculate optimal transfer size
        size_t transferSize = (len + 3) & ~3; // Round up to nearest multiple of 4
        
        _dmaStatus = DMA_ACTIVE;
        
        // Configure for optimal burst transfers
        _dmaChannel->transferSize(4); // 32-bit transfers
        _dmaChannel->transferCount(transferSize / 4);
        
        _dmaChannel->TCD->ATTR = DMA_TCD_ATTR_SSIZE(2) | DMA_TCD_ATTR_DSIZE(2);
        _dmaChannel->TCD->NBYTES_MLNO = 4;
        _dmaChannel->TCD->CSR |= DMA_TCD_CSR_MAJORELINK;
        
        // Set source buffer with optimized alignment
        _dmaChannel->sourceBuffer(const_cast<uint8_t*>(data), transferSize);
        
        // Clear any pending flags before starting
        SPI0_SR = SPI_SR_TCF | SPI_SR_EOQF | SPI_SR_TFUF | SPI_SR_TFFF | SPI_SR_RFOF | SPI_SR_RFDF;
        
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
        IMXRT_LPSPI4_S.TCR = 0;
        delete _dmaChannel;
        _dmaChannel = nullptr;
    }
#elif defined(__MK66FX1M0__)
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
        IMXRT_LPSPI4_S.TCR = 0;
        _dmaChannel->clearComplete();
    }
#elif defined(__MK66FX1M0__)
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
                _dmaChannel->interruptAtCompletion();
                
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
    #elif defined(__MK66FX1M0__)
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
            uint32_t mcr = *_spiMCR;
            mcr &= ~SPI_MCR_MDIS;
            *_spiMCR = mcr;

            // Configure FIFO watermarks - using direct register access
            // These definitions should be provided by kinetis.h
            #if defined(SPI_RSER_TFFF_RE)
                uint32_t rser = SPI0_RSER;
                rser &= ~(SPI_RSER_TFFF_RE | SPI_RSER_RFDF_RE | 
                         SPI_RSER_TFFF_DIRS | SPI_RSER_RFDF_DIRS);
                SPI0_RSER = rser;
            #endif
        #elif defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41)
            // Teensy 4.x specific FIFO setup if needed
            // Currently no specific FIFO setup needed
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
