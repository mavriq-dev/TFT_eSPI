#include "TFT_Interface_SPI.h"
#if defined(__IMXRT1062__)
#include <imxrt.h>
#include <SPI.h>
#include <LPSPI.h>
#endif

namespace TFT_Runtime {

TFT_Interface_SPI::TFT_Interface_SPI(const Config& config)
    : TFT_Interface(config)
    , _spiSettings(config.spi.frequency, MSBFIRST, config.spi.spi_mode)
    , _spi(&SPI)
    , _threeWire(config.spi.three_wire)
    , _inTransaction(false)
    , _dmaEnabled(false)
    , _buffering(false)
    , _csPin(config.spi.cs_pin)
    , _dcPin(config.spi.dc_pin)
    , _rstPin(config.rst_pin)
    , _buffer(nullptr)
    , _bufferIndex(0)
    , _xDatum(0)
    , _yDatum(0)
    , _vpX(0)
    , _vpY(0)
    , _vpW(0)
    , _vpH(0)
    , _vpDatum(false)
    , _vpOoB(false)
    , _width(config.width)
    , _height(config.height)
{
}

TFT_Interface_SPI::~TFT_Interface_SPI() {
    end();
    if (_buffer) {
        delete[] _buffer;
    }
}

bool TFT_Interface_SPI::begin() {
    setupPins();
    _spi->begin();
    
    if (_config.spi.frequency > 0) {
        _spiSettings = SPISettings(_config.spi.frequency, MSBFIRST, _config.spi.spi_mode);
    }
    
    // Initialize buffer if buffering is supported
    if (supportsBuffering() && !_buffer) {
        _buffer = new uint8_t[BUFFER_SIZE];
    }
    
    // Initialize DMA if supported
    if (supportsDMA()) {
        initDMA();
    }
    
    return true;
}

void TFT_Interface_SPI::end() {
    if (_inTransaction) {
        endTransaction();
    }
    _spi->end();
}

void TFT_Interface_SPI::writeCommand(uint8_t cmd) {
    beginTransaction();
    setDC(false);  // Command mode
    write(cmd);
    setDC(true);   // Back to data mode
}

void TFT_Interface_SPI::writeData(uint8_t data) {
    beginTransaction();
    write(data);
}

void TFT_Interface_SPI::writeData16(uint16_t data) {
    beginTransaction();
    write16(data);
}

uint8_t TFT_Interface_SPI::readData() {
    beginTransaction();
    return _spi->transfer(0);
}

uint16_t TFT_Interface_SPI::readData16() {
    beginTransaction();
    return _spi->transfer16(0);
}

void TFT_Interface_SPI::writeDataBlock(const uint8_t* data, size_t len) {
    if (_buffering) {
        // Add to buffer if there's space
        size_t remaining = BUFFER_SIZE - _bufferIndex;
        if (len <= remaining) {
            memcpy(_buffer + _bufferIndex, data, len);
            _bufferIndex += len;
            return;
        }
        // If buffer is full, flush it
        flushBuffer();
    }
    
    beginTransaction();
    if (supportsDMA() && len > 32) {
        writeDMA(data, len);
    } else {
        _spi->transfer(data, nullptr, len);
    }
}

void TFT_Interface_SPI::writeDataBlock16(const uint16_t* data, size_t len) {
    beginTransaction();
    if (supportsDMA() && len > 16) {
        writeDMA16(data, len);
    } else {
        while (len--) {
            _spi->transfer16(*data++);
        }
    }
}

void TFT_Interface_SPI::readDataBlock(uint8_t* data, size_t len) {
    beginTransaction();
    _spi->transfer(nullptr, data, len);
}

void TFT_Interface_SPI::readDataBlock16(uint16_t* data, size_t len) {
    beginTransaction();
    while (len--) {
        *data++ = _spi->transfer16(0);
    }
}

void TFT_Interface_SPI::setCS(bool level) {
    if (_csPin >= 0) {
        digitalWrite(_csPin, level ? HIGH : LOW);
    }
}

void TFT_Interface_SPI::setDC(bool level) {
    if (_dcPin >= 0) {
        digitalWrite(_dcPin, level ? HIGH : LOW);
    }
}

void TFT_Interface_SPI::setRST(bool level) {
    if (_rstPin >= 0) {
        digitalWrite(_rstPin, level ? HIGH : LOW);
    }
}

bool TFT_Interface_SPI::supportsDMA() const {
    #if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_ARCH_RP2040) || \
        (defined(CORE_TEENSY) && (defined(__IMXRT1062__) || defined(__MK66FX1M0__) || defined(__MK64FX512__)))
        return true;
    #else
        return false;
    #endif
}

bool TFT_Interface_SPI::startDMAWrite(const uint8_t* data, size_t len) {
    #if defined(ESP32)
        _spi->writeBytes(data, len);
        return true;
    #elif defined(ESP8266)
        _spi->writePattern(data, len, 1);
        return true;
    #elif defined(ARDUINO_ARCH_RP2040)
        _spi->transfer(data, nullptr, len);
        return true;
    #elif defined(CORE_TEENSY)
        return writeDMA(data, len);
    #else
        return false;
    #endif
}

bool TFT_Interface_SPI::startDMAWrite16(const uint16_t* data, size_t len) {
    return startDMAWrite(reinterpret_cast<const uint8_t*>(data), len * 2);
}

bool TFT_Interface_SPI::isDMABusy() const {
    #if defined(ESP32)
        return _spi->busy();
    #elif defined(CORE_TEENSY)
        #if defined(__IMXRT1062__)
            return !_dmaChannel->complete();
        #elif defined(__MK66FX1M0__) || defined(__MK64FX512__)
            return !_dmaChannel->complete();
        #endif
    #else
        return false;
    #endif
}

void TFT_Interface_SPI::waitDMAComplete() {
    while (isDMABusy()) {
        yield();
    }
}

bool TFT_Interface_SPI::supportsBuffering() const {
    return true;
}

void TFT_Interface_SPI::startBuffer() {
    _buffering = true;
    _bufferIndex = 0;
}

void TFT_Interface_SPI::endBuffer() {
    flushBuffer();
    _buffering = false;
}

void TFT_Interface_SPI::flushBuffer() {
    if (_bufferIndex > 0) {
        writeDataBlock(_buffer, _bufferIndex);
        _bufferIndex = 0;
    }
}

void TFT_Interface_SPI::beginTransaction() {
    if (!_inTransaction) {
        _spi->beginTransaction(_spiSettings);
        setCS(false);
        _inTransaction = true;
    }
}

void TFT_Interface_SPI::endTransaction() {
    if (_inTransaction) {
        setCS(true);
        _spi->endTransaction();
        _inTransaction = false;
    }
}

void TFT_Interface_SPI::beginRead() {
    if (_spi) {
        // Switch to slower clock for read operations
        _spi->beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
        digitalWrite(_csPin, LOW);
    }
}

void TFT_Interface_SPI::endRead() {
    if (_spi) {
        digitalWrite(_csPin, HIGH);
        _spi->endTransaction();
    }
}

void TFT_Interface_SPI::begin_nin_write() {
    if (_spi) {
        _spi->beginTransaction(_spiSettings);
        digitalWrite(_csPin, LOW);
    }
}

void TFT_Interface_SPI::end_nin_write() {
    if (_spi) {
        digitalWrite(_csPin, HIGH);
        _spi->endTransaction();
    }
}

void TFT_Interface_SPI::setViewport(int32_t x, int32_t y, int32_t w, int32_t h, bool vpDatum) {
    // Save the viewport data
    _xDatum = x;
    _yDatum = y;
    _vpX = x;
    _vpY = y;
    _vpW = w;
    _vpH = h;
    _vpDatum = vpDatum;
    
    // Check if viewport is entirely outside display
    _vpOoB = (_vpX >= _width) || (_vpY >= _height) || (w <= 0) || (h <= 0);
    
    // Clip viewport to display boundaries
    if (_vpX < 0) { _vpW += _vpX; _vpX = 0; }
    if (_vpY < 0) { _vpH += _vpY; _vpY = 0; }
    
    if (_vpX + _vpW > _width)  _vpW = _width  - _vpX;
    if (_vpY + _vpH > _height) _vpH = _height - _vpY;
    
    // Check if viewport is entirely outside display
    if (_vpW <= 0 || _vpH <= 0) _vpOoB = true;
}

void TFT_Interface_SPI::resetViewport() {
    // Reset viewport to default values
    _vpX = 0;
    _vpY = 0;
    _vpW = _width;
    _vpH = _height;
    _xDatum = 0;
    _yDatum = 0;
    _vpDatum = false;
    _vpOoB = false;
}

bool TFT_Interface_SPI::checkViewport(int32_t x, int32_t y, int32_t w, int32_t h) {
    if (_vpOoB) return false; // Viewport is outside display area
    
    x += _xDatum;
    y += _yDatum;
    
    if ((x >= _vpX + _vpW) || (y >= _vpY + _vpH)) return false;
    if ((x + w <= _vpX) || (y + h <= _vpY)) return false;
    
    return true;
}

bool TFT_Interface_SPI::clipAddrWindow(int32_t* x, int32_t* y, int32_t* w, int32_t* h) {
    if (_vpOoB) return false;
    
    *x += _xDatum;
    *y += _yDatum;
    
    if (*x >= _vpX + _vpW) return false;
    if (*y >= _vpY + _vpH) return false;
    if (*x + *w <= _vpX) return false;
    if (*y + *h <= _vpY) return false;
    
    // Clip window to viewport
    if (*x < _vpX) { *w -= (_vpX - *x); *x = _vpX; }
    if (*y < _vpY) { *h -= (_vpY - *y); *y = _vpY; }
    
    if (*x + *w > _vpX + _vpW) *w = _vpX + _vpW - *x;
    if (*y + *h > _vpY + _vpH) *h = _vpY + _vpH - *y;
    
    return true;
}

bool TFT_Interface_SPI::clipWindow(int32_t* xs, int32_t* ys, int32_t* xe, int32_t* ye) {
    if (_vpOoB) return false;
    
    *xs += _xDatum;
    *ys += _yDatum;
    *xe += _xDatum;
    *ye += _yDatum;
    
    if (*xs >= _vpX + _vpW) return false;
    if (*ys >= _vpY + _vpH) return false;
    if (*xe <= _vpX) return false;
    if (*ye <= _vpY) return false;
    
    // Clip coordinates to viewport
    if (*xs < _vpX) *xs = _vpX;
    if (*ys < _vpY) *ys = _vpY;
    
    if (*xe > _vpX + _vpW) *xe = _vpX + _vpW;
    if (*ye > _vpY + _vpH) *ye = _vpY + _vpH;
    
    return true;
}

void TFT_Interface_SPI::setupPins() {
    if (_csPin >= 0) {
        pinMode(_csPin, OUTPUT);
        digitalWrite(_csPin, HIGH);
    }
    
    if (_dcPin >= 0) {
        pinMode(_dcPin, OUTPUT);
        digitalWrite(_dcPin, HIGH);
    }
    
    if (_rstPin >= 0) {
        pinMode(_rstPin, OUTPUT);
        digitalWrite(_rstPin, HIGH);
    }
}

void TFT_Interface_SPI::initDMA() {
    #if defined(ESP32)
        _spi->setHwCs(false);
        _dmaEnabled = true;
    #elif defined(ESP8266)
        _dmaEnabled = true;
    #elif defined(ARDUINO_ARCH_RP2040)
        _dmaEnabled = true;
    #elif defined(CORE_TEENSY)
        initTeensy();
    #endif
}

#if defined(CORE_TEENSY)

bool TFT_Interface_SPI::initTeensy() {
    #if defined(__IMXRT1062__) // Teensy 4.0/4.1
        // Determine which LPSPI bus to use based on SCK pin
        if (_config.spi.sck_pin == 13) {       // LPSPI4 default pins
            _spi = new SPIClass(LPSPI4);
            _hardware = &IMXRT_LPSPI4_S;
        } else if (_config.spi.sck_pin == 27) { // LPSPI1 alternate pins
            _spi = new SPIClass(LPSPI1);
            _hardware = &IMXRT_LPSPI1_S;
        } else if (_config.spi.sck_pin == 46) { // LPSPI3 alternate pins
            _spi = new SPIClass(LPSPI3);
            _hardware = &IMXRT_LPSPI3_S;
        } else {
            // Default to LPSPI4 if no matching pin configuration
            _spi = new SPIClass(LPSPI4);
            _hardware = &IMXRT_LPSPI4_S;
        }
        _spiClock = LPSPI_CLOCK_SPEED;
        
    #elif defined(__MK66FX1M0__) || defined(__MK64FX512__) // Teensy 3.5/3.6
        _spi = new SPIClass();  // Uses standard hardware SPI
        _spiClock = SPI_CLOCK_DIV4;
        
    #elif defined(__MK20DX256__) // Teensy 3.2
        _spi = new SPIClass();  // Uses standard hardware SPI
        _spiClock = SPI_CLOCK_DIV4;
        
    #endif
    
    return true;
}

#if defined(__IMXRT1062__)
void TFT_Interface_SPI::configureTeensy4DirectSPI() {
    // Enable LPSPI clock
    *_clock_gate_register |= _clock_gate_mask;
    
    // Configure LPSPI
    _hardware->CR = LPSPI_CR_RST;
    _hardware->CR = 0;
    
    // Set clock configuration
    _hardware->CCR = LPSPI_CCR_SCKDIV(1) | LPSPI_CCR_DBT(1) | LPSPI_CCR_PCSSCK(1);
    
    // Configure for 8-bit transfers
    _hardware->TCR = LPSPI_TCR_FRAMESZ(7) | LPSPI_TCR_RXMSK;
    
    // Enable module
    _hardware->CR = LPSPI_CR_MEN;
}

void TFT_Interface_SPI::initTeensyDMA() {
    _dmaChannel = new DMAChannel();
    _dmaSettings = new DMASetting();
    
    // Allocate DMA buffer
    _dmaBufferSize = 1024;
    _dmaBuffer = (uint8_t*)malloc(_dmaBufferSize);
    
    // Configure DMA
    _dmaChannel->destination(_hardware->TDR);
    _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_LPSPI4_TX);
    _dmaChannel->enable();
}

void TFT_Interface_SPI::writeTeensy4(uint8_t data) {
    while (!(_hardware->SR & LPSPI_SR_TDF)) ; // Wait for TX FIFO
    _hardware->TDR = data;
    while (!(_hardware->SR & LPSPI_SR_TCF)) ; // Wait for transfer complete
}

void TFT_Interface_SPI::writeTeensy4Direct(uint8_t data) {
    _hardware->TDR = data;
    while (!(_hardware->SR & LPSPI_SR_TCF)) ;
}

void TFT_Interface_SPI::writeTeensy4DMA(const uint8_t* data, size_t len) {
    size_t remaining = len;
    const uint8_t* src = data;
    
    while (remaining > 0) {
        size_t chunk = min(remaining, _dmaBufferSize);
        memcpy(_dmaBuffer, src, chunk);
        
        _dmaChannel->transferCount(chunk);
        _dmaChannel->sourceBuffer(_dmaBuffer, chunk);
        _dmaChannel->enable();
        
        while (!_dmaChannel->complete()) ;
        
        remaining -= chunk;
        src += chunk;
    }
}

#elif defined(__MK66FX1M0__) || defined(__MK64FX512__)
void TFT_Interface_SPI::configureTeensy3DirectSPI() {
    // Configure SPI for fastest possible speed
    *_spiBaseReg = SPI_MCR_MSTR | SPI_MCR_PCSIS(0x1F);
    *_spiBaseReg &= ~(SPI_MCR_MDIS | SPI_MCR_HALT);
}

void TFT_Interface_SPI::initTeensyDMA() {
    _dmaChannel = new DMAChannel();
    
    // Allocate DMA buffer
    _dmaBufferSize = 1024;
    _dmaBuffer = (uint8_t*)malloc(_dmaBufferSize);
    
    // Configure DMA
    _dmaChannel->destination(*_spiDataReg);
    _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_SPI0_TX);
    _dmaChannel->enable();
}

void TFT_Interface_SPI::writeTeensy3(uint8_t data) {
    while (!(*_spiStatusReg & SPI_SR_TFFF)) ; // Wait for TX FIFO
    *_spiDataReg = data;
    while (!(*_spiStatusReg & SPI_SR_TCF)) ; // Wait for transfer complete
}

void TFT_Interface_SPI::writeTeensy3Direct(uint8_t data) {
    *_spiDataReg = data;
    while (!(*_spiStatusReg & SPI_SR_TCF)) ;
}

void TFT_Interface_SPI::writeTeensy3DMA(const uint8_t* data, size_t len) {
    size_t remaining = len;
    const uint8_t* src = data;
    
    while (remaining > 0) {
        size_t chunk = min(remaining, _dmaBufferSize);
        memcpy(_dmaBuffer, src, chunk);
        
        _dmaChannel->transferCount(chunk);
        _dmaChannel->sourceBuffer(_dmaBuffer, chunk);
        _dmaChannel->enable();
        
        while (!_dmaChannel->complete()) ;
        
        remaining -= chunk;
        src += chunk;
    }
}

#elif defined(__MK20DX256__)
void TFT_Interface_SPI::configureTeensy32DirectSPI() {
    // Configure SPI for fastest possible speed
    *_spiBaseReg = SPI_MCR_MSTR | SPI_MCR_PCSIS(0x1F);
    *_spiBaseReg &= ~(SPI_MCR_MDIS | SPI_MCR_HALT);
}

void TFT_Interface_SPI::writeTeensy32(uint8_t data) {
    while (!(*_spiStatusReg & SPI_SR_TFFF)) ; // Wait for TX FIFO
    *_spiDataReg = data;
    while (!(*_spiStatusReg & SPI_SR_TCF)) ; // Wait for transfer complete
}

void TFT_Interface_SPI::writeTeensy32Direct(uint8_t data) {
    *_spiDataReg = data;
    while (!(*_spiStatusReg & SPI_SR_TCF)) ;
}
#endif

#endif // CORE_TEENSY

// Main write method that calls platform-specific implementations
void TFT_Interface_SPI::write(uint8_t data) {
    #if defined(ESP32)
        writeESP32(data);
    #elif defined(ESP8266)
        writeESP8266(data);
    #elif defined(CORE_TEENSY)
        #if defined(__IMXRT1062__)
            writeTeensy4Direct(data);
        #elif defined(__MK66FX1M0__) || defined(__MK64FX512__)
            writeTeensy3Direct(data);
        #elif defined(__MK20DX256__)
            writeTeensy32Direct(data);
        #endif
    #elif defined(ARDUINO_ARCH_RP2040)
        writeRP2040(data);
    #elif defined(__SAM3X8E__) || defined(__SAM3X8H__)
        writeSAMDUE(data);
    #elif defined(__AVR__)
        writeAVR(data);
    #else
        _spi->transfer(data);
    #endif
}

bool TFT_Interface_SPI::writeDMA(const uint8_t* data, size_t len) {
    #if defined(ESP32)
        writeESP32DMA(data, len);
        return true;
    #elif defined(ARDUINO_ARCH_RP2040)
        writeRP2040DMA(data, len);
        return true;
    #elif defined(CORE_TEENSY)
        #if defined(__IMXRT1062__)
            writeTeensy4DMA(data, len);
            return true;
        #elif defined(__MK66FX1M0__) || defined(__MK64FX512__)
            writeTeensy3DMA(data, len);
            return true;
        #endif
    #endif
    return false;
}

bool TFT_Interface_SPI::writeDMA16(const uint16_t* data, size_t len) {
    if (!_dmaEnabled) return false;
    
    #if defined(__IMXRT1062__)  // Teensy 4.0/4.1
        writeTeensy4DMA(reinterpret_cast<const uint8_t*>(data), len * 2);
        return true;
    #elif defined(__MK66FX1M0__) || defined(__MK64FX512__)  // Teensy 3.6/3.5
        writeTeensy3DMA(reinterpret_cast<const uint8_t*>(data), len * 2);
        return true;
    #elif defined(ESP32)
        return startDMAWrite16(data, len);
    #else
        return false;
    #endif
}

void TFT_Interface_SPI::write16(uint16_t data) {
    write((uint8_t)(data >> 8));  // Write high byte first
    write((uint8_t)data);         // Write low byte
}

} // namespace TFT_Runtime
