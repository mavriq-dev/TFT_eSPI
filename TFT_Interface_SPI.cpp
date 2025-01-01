#include "TFT_Interface_SPI.h"
#include <Arduino.h>

namespace TFT_Runtime {

TFT_Interface_SPI::TFT_Interface_SPI(const Config& config)
    : TFT_Interface(config)
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
    #endif
}

TFT_Interface_SPI::~TFT_Interface_SPI() {
    cleanupDMA();
}

bool TFT_Interface_SPI::begin() {
    setupPins();
    setSPISettings();

    // Platform-specific initialization
    #if defined(ESP32)
        return initESP32();
    #elif defined(ESP8266)
        return initESP8266();
    #elif defined(ARDUINO_ARCH_RP2040)
        return initRP2040();
    #elif defined(ARDUINO_SAM_DUE)
        return initSAMDUE();
    #elif defined(__AVR__)
        return initAVR();
    #elif defined(CORE_TEENSY)
        return initTeensy();
    #else
        return true;
    #endif
}

void TFT_Interface_SPI::writeCommand(uint8_t cmd) {
    beginTransaction();
    digitalWrite(_dcPin, LOW);
    digitalWrite(_csPin, LOW);
    
    #if defined(ESP32)
        writeESP32_8(cmd);
    #elif defined(ESP8266)
        writeESP8266_8(cmd);
    #elif defined(ARDUINO_ARCH_RP2040)
        writeRP2040_8(cmd);
    #elif defined(ARDUINO_SAM_DUE)
        writeSAMDUE_8(cmd);
    #elif defined(__AVR__)
        writeAVR_8(cmd);
    #elif defined(CORE_TEENSY)
        writeTeensy_8(cmd);
    #else
        _spi->transfer(cmd);
    #endif
    
    digitalWrite(_csPin, HIGH);
    endTransaction();
}

void TFT_Interface_SPI::writeData(uint8_t data) {
    digitalWrite(_dcPin, HIGH);
    digitalWrite(_csPin, LOW);
    
    #if defined(ESP32)
        writeESP32_8(data);
    #elif defined(ESP8266)
        writeESP8266_8(data);
    #elif defined(ARDUINO_ARCH_RP2040)
        writeRP2040_8(data);
    #elif defined(ARDUINO_SAM_DUE)
        writeSAMDUE_8(data);
    #elif defined(__AVR__)
        writeAVR_8(data);
    #elif defined(CORE_TEENSY)
        writeTeensy_8(data);
    #else
        _spi->transfer(data);
    #endif
    
    digitalWrite(_csPin, HIGH);
}

void TFT_Interface_SPI::writeData16(uint16_t data) {
    digitalWrite(_dcPin, HIGH);
    digitalWrite(_csPin, LOW);
    
    #if defined(ESP32)
        writeESP32_16(data);
    #elif defined(ESP8266)
        writeESP8266_16(data);
    #elif defined(ARDUINO_ARCH_RP2040)
        writeRP2040_16(data);
    #elif defined(ARDUINO_SAM_DUE)
        writeSAMDUE_16(data);
    #elif defined(__AVR__)
        writeAVR_16(data);
    #elif defined(CORE_TEENSY)
        writeTeensy_16(data);
    #else
        _spi->transfer16(data);
    #endif
    
    digitalWrite(_csPin, HIGH);
}

void TFT_Interface_SPI::writeDataBlock(const uint8_t* data, size_t len) {
    if (len == 0) return;

    digitalWrite(_dcPin, HIGH);
    digitalWrite(_csPin, LOW);

    if (supportsDMA() && startDMAWrite(data, len)) {
        waitDMAComplete();
    } else {
        for(size_t i = 0; i < len; i++) {
            _spi->transfer(data[i]);
        }
    }

    digitalWrite(_csPin, HIGH);
}

void TFT_Interface_SPI::writeDataBlock16(const uint16_t* data, size_t len) {
    if (len == 0) return;

    digitalWrite(_dcPin, HIGH);
    digitalWrite(_csPin, LOW);

    for (size_t i = 0; i < len; i++) {
        writeData16(data[i]);
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
    #if defined(ESP32) || defined(ARDUINO_ARCH_RP2040) || (defined(CORE_TEENSY) && defined(__IMXRT1062__))
        return true;
    #else
        return false;
    #endif
}

// Platform-specific implementations
#if defined(CORE_TEENSY)
bool TFT_Interface_SPI::initTeensy() {
    #if defined(__IMXRT1062__)
    if (supportsDMA()) {
        _dmaChannel = new DMAChannel();
        if (_dmaChannel) {
            _dmaChannel->begin();
            // Instead of directly accessing hardware, we'll use the transfer method
            // The SPI class will handle the DMA setup internally
            _dmaInitialized = true;
        }
    }
    #endif
    return true;
}

void TFT_Interface_SPI::writeTeensy_8(uint8_t data) {
    _spi->transfer(data);
}

void TFT_Interface_SPI::writeTeensy_16(uint16_t data) {
    _spi->transfer16(data);
}

bool TFT_Interface_SPI::startDMAWrite(const uint8_t* data, size_t len) {
    #if defined(__IMXRT1062__)
    if (_dmaInitialized && len >= 32) {
        _dmaChannel->sourceBuffer((volatile uint8_t*)data, len);
        _dmaChannel->enable();
        return true;
    }
    #endif
    return false;
}

void TFT_Interface_SPI::cleanupDMA() {
    #if defined(__IMXRT1062__)
    if (_dmaChannel) {
        _dmaChannel->disable();
        delete _dmaChannel;
        _dmaChannel = nullptr;
    }
    #endif
}

void TFT_Interface_SPI::waitDMAComplete() {
    #if defined(ESP32)
    if (_dmaInitialized) {
        while (!_spi->DMAbsy()) {
            yield();
        }
    }
    #elif defined(CORE_TEENSY) && defined(__IMXRT1062__)
    if (_dmaChannel) {
        while (!_dmaChannel->complete()) {
            yield();
        }
    }
    #endif
}
#endif

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
