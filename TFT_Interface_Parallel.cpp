#include "TFT_Interface_Parallel.h"
#include <Arduino.h>
#if defined(__IMXRT1062__)
#include "core_pins.h"
#include "imxrt.h"
#endif

namespace TFT_Runtime {

TFT_Interface_Parallel::TFT_Interface_Parallel(const Config& config)
    : TFT_Interface(config)
    , _csPin(config.parallel.cs_pin)
    , _dcPin(config.parallel.dc_pin)
    , _wrPin(config.parallel.wr_pin)
    , _rdPin(config.parallel.rd_pin)
    , _rstPin(config.rst_pin)
    , _latchPin(-1)  // Not used in base implementation
    , _is16Bit(false)  // 8-bit mode by default
    , _useLatch(false)
    , _writeDelay(0)
    , _vpX(0)
    , _vpY(0)
    , _vpW(config.width)
    , _vpH(config.height)
    , _vpDatum(false)
    , _vpActive(false)
{
    // Copy data pins from config
    for (int i = 0; i < 8; i++) {
        _dataPins[i] = config.parallel.d0_pin + i;
    }

#if defined(ESP32)
    _dmaChannel = -1;
    _dmaBuf = nullptr;
    _dmadesc = nullptr;
    _dmaInitialized = false;
#elif defined(ARDUINO_ARCH_RP2040)
    _pio = pio0;
    _sm = 0;
    _dma_chan = 0;
    _pioInitialized = false;
#elif defined(CORE_TEENSY)
    #if defined(__IMXRT1062__)
        _flexIO = nullptr;
        _flexIOShifter = 0;
        _flexIOTimer = 0;
        _dmaChannel = nullptr;
    #endif
#endif
}

TFT_Interface_Parallel::~TFT_Interface_Parallel() {
    cleanupDMA();
}

bool TFT_Interface_Parallel::begin() {
    bool success = true;
    
#if defined(ESP32)
    success = initESP32();
#elif defined(ESP8266)
    success = initESP8266();
#elif defined(ARDUINO_ARCH_RP2040)
    success = initRP2040();
#elif defined(ARDUINO_SAM_DUE)
    success = initSAMDUE();
#elif defined(__AVR__)
    success = initAVR();
#elif defined(CORE_TEENSY)
    success = initTeensy();
#else
    setupPins();
#endif

    return success;
}

void TFT_Interface_Parallel::writeCommand(uint8_t cmd) {
    digitalWrite(_csPin, LOW);
    digitalWrite(_dcPin, LOW);  // Command mode
    #if defined(ESP32)
        writeData(cmd);  // ESP32 specific implementation
    #elif defined(ESP8266)
        writeData(cmd);  // ESP8266 specific implementation
    #elif defined(ARDUINO_ARCH_RP2040)
        writeData(cmd);  // RP2040 specific implementation
    #elif defined(ARDUINO_SAM_DUE)
        writeData(cmd);  // SAM DUE specific implementation
    #elif defined(__AVR__)
        writeData(cmd);  // AVR specific implementation
    #elif defined(CORE_TEENSY)
        writeTeensy_8(cmd);  // Teensy specific implementation
    #else
        writeData(cmd);  // Default implementation
    #endif
    digitalWrite(_csPin, HIGH);
}

void TFT_Interface_Parallel::writeData(uint8_t data) {
    digitalWrite(_csPin, LOW);
    digitalWrite(_dcPin, HIGH);  // Data mode
    #if defined(ESP32)
        // ESP32 specific implementation
        // TODO: Implement platform-specific data write
    #elif defined(ESP8266)
        // ESP8266 specific implementation
        // TODO: Implement platform-specific data write
    #elif defined(ARDUINO_ARCH_RP2040)
        // RP2040 specific implementation
        // TODO: Implement platform-specific data write
    #elif defined(ARDUINO_SAM_DUE)
        // SAM DUE specific implementation
        // TODO: Implement platform-specific data write
    #elif defined(__AVR__)
        // AVR specific implementation
        // TODO: Implement platform-specific data write
    #elif defined(CORE_TEENSY)
        writeTeensy_8(data);  // Teensy specific implementation
    #else
        // Default implementation
        // TODO: Implement generic data write
    #endif
    digitalWrite(_csPin, HIGH);
}

void TFT_Interface_Parallel::writeData16(uint16_t data) {
    digitalWrite(_csPin, LOW);
    digitalWrite(_dcPin, HIGH);  // Data mode
    #if defined(ESP32)
        // ESP32 specific 16-bit implementation
        writeData(data >> 8);
        writeData(data & 0xFF);
    #elif defined(ESP8266)
        // ESP8266 specific 16-bit implementation
        writeData(data >> 8);
        writeData(data & 0xFF);
    #elif defined(ARDUINO_ARCH_RP2040)
        // RP2040 specific 16-bit implementation
        writeData(data >> 8);
        writeData(data & 0xFF);
    #elif defined(ARDUINO_SAM_DUE)
        // SAM DUE specific 16-bit implementation
        writeData(data >> 8);
        writeData(data & 0xFF);
    #elif defined(__AVR__)
        // AVR specific 16-bit implementation
        writeData(data >> 8);
        writeData(data & 0xFF);
    #elif defined(CORE_TEENSY)
        writeTeensy_16(data);  // Teensy specific 16-bit implementation
    #else
        // Default 16-bit implementation
        writeData(data >> 8);
        writeData(data & 0xFF);
    #endif
    digitalWrite(_csPin, HIGH);
}

uint8_t TFT_Interface_Parallel::readData() {
    digitalWrite(_csPin, LOW);
    digitalWrite(_dcPin, HIGH);  // Data mode
    setDataPinsInput();
    
    // Read data using platform-specific implementation
    uint8_t data = 0;
    #if defined(ESP32)
        data = readESP32_8();
    #elif defined(ESP8266)
        data = readESP8266_8();
    #elif defined(ARDUINO_ARCH_RP2040)
        data = readRP2040_8();
    #elif defined(ARDUINO_SAM_DUE)
        data = readSAMDUE_8();
    #elif defined(__AVR__)
        data = readAVR_8();
    #elif defined(CORE_TEENSY)
        #if defined(__IMXRT1062__)
            // Teensy 4.0/4.1 specific implementation
            for (int i = 0; i < 8; i++) {
                if (digitalRead(_dataPins[i])) {
                    data |= (1 << i);
                }
            }
        #elif defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__)
            // Teensy 3.x specific implementation
            for (int i = 0; i < 8; i++) {
                if (digitalRead(_dataPins[i])) {
                    data |= (1 << i);
                }
            }
        #endif
    #else
        // Default implementation
        for (int i = 0; i < 8; i++) {
            if (digitalRead(_dataPins[i])) {
                data |= (1 << i);
            }
        }
    #endif

    setDataPinsOutput();
    digitalWrite(_csPin, HIGH);
    return data;
}

uint16_t TFT_Interface_Parallel::readData16() {
    digitalWrite(_csPin, LOW);
    digitalWrite(_dcPin, HIGH);  // Data mode
    setDataPinsInput();
    
    uint16_t data;
    if (_is16Bit) {
        #if defined(ESP32)
            data = readESP32_16();
        #elif defined(ESP8266)
            data = readESP8266_16();
        #elif defined(ARDUINO_ARCH_RP2040)
            data = readRP2040_16();
        #elif defined(ARDUINO_SAM_DUE)
            data = readSAMDUE_16();
        #elif defined(__AVR__)
            data = readAVR_16();
        #elif defined(CORE_TEENSY)
            #if defined(__IMXRT1062__)
                // Teensy 4.0/4.1 specific implementation
                for (int i = 0; i < 16; i++) {
                    if (digitalRead(_dataPins[i])) {
                        data |= (1 << i);
                    }
                }
            #elif defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__)
                // Teensy 3.x specific implementation
                for (int i = 0; i < 16; i++) {
                    if (digitalRead(_dataPins[i])) {
                        data |= (1 << i);
                    }
                }
            #endif
        #else
            // Default implementation
            data = 0;
            for (int i = 0; i < 16; i++) {
                if (digitalRead(_dataPins[i])) {
                    data |= (1 << i);
                }
            }
        #endif
    } else {
        // For 8-bit mode, read high byte then low byte
        data = (readData() << 8) | readData();
    }

    setDataPinsOutput();
    digitalWrite(_csPin, HIGH);
    return data;
}

void TFT_Interface_Parallel::writeDataBlock(const uint8_t* data, size_t len) {
    if (len == 0) return;

    digitalWrite(_csPin, LOW);
    digitalWrite(_dcPin, HIGH);  // Data mode

    if (supportsDMA() && len > 32) {
        startDMAWrite(data, len);
    } else {
        for (size_t i = 0; i < len; i++) {
            writeData(data[i]);
        }
    }

    digitalWrite(_csPin, HIGH);
}

void TFT_Interface_Parallel::writeDataBlock16(const uint16_t* data, size_t len) {
    if (len == 0) return;

    digitalWrite(_csPin, LOW);
    digitalWrite(_dcPin, HIGH);  // Data mode

    if (_is16Bit) {
        if (supportsDMA() && len > 16) {
            startDMAWrite(reinterpret_cast<const uint8_t*>(data), len * 2);
        } else {
            for (size_t i = 0; i < len; i++) {
                writeData16(data[i]);
            }
        }
    } else {
        for (size_t i = 0; i < len; i++) {
            writeData(data[i] >> 8);
            writeData(data[i]);
        }
    }

    digitalWrite(_csPin, HIGH);
}

void TFT_Interface_Parallel::readDataBlock(uint8_t* data, size_t len) {
    if (len == 0) return;

    digitalWrite(_csPin, LOW);
    digitalWrite(_dcPin, HIGH);  // Data mode
    setDataPinsInput();

    for (size_t i = 0; i < len; i++) {
        data[i] = readData();
    }

    setDataPinsOutput();
    digitalWrite(_csPin, HIGH);
}

void TFT_Interface_Parallel::readDataBlock16(uint16_t* data, size_t len) {
    if (len == 0) return;

    digitalWrite(_csPin, LOW);
    digitalWrite(_dcPin, HIGH);  // Data mode
    setDataPinsInput();

    if (_is16Bit) {
        for (size_t i = 0; i < len; i++) {
            data[i] = readData16();
        }
    } else {
        for (size_t i = 0; i < len; i++) {
            data[i] = (readData() << 8) | readData();
        }
    }

    setDataPinsOutput();
    digitalWrite(_csPin, HIGH);
}

// Platform-specific implementations will be added in separate files
#if defined(ESP32)
bool TFT_Interface_Parallel::initESP32() {
    setupPins();
    return true;
}
#endif

#if defined(ESP8266)
bool TFT_Interface_Parallel::initESP8266() {
    setupPins();
    return true;
}
#endif

#if defined(ARDUINO_ARCH_RP2040)
bool TFT_Interface_Parallel::initRP2040() {
    setupPins();
    return true;
}
#endif

#if defined(ARDUINO_SAM_DUE)
bool TFT_Interface_Parallel::initSAMDUE() {
    setupPins();
    return true;
}
#endif

#if defined(__AVR__)
bool TFT_Interface_Parallel::initAVR() {
    setupPins();
    return true;
}
#endif

#if defined(CORE_TEENSY)
bool TFT_Interface_Parallel::initTeensy() {
    setupPins();
    
    #if defined(__IMXRT1062__)  // Teensy 4.0/4.1
    // Configure FlexIO for parallel interface
    if (_is16Bit) {
        // 16-bit mode setup
        _flexIO = &IMXRT_FLEXIO1_S;  // Use the correct peripheral naming for Teensy 4.1
        _flexIOShifter = 0;
        _flexIOTimer = 0;
        
        // Configure FlexIO pins
        for (int i = 0; i < 16; i++) {
            pinMode(_dataPins[i], OUTPUT);
            *(portConfigRegister(_dataPins[i])) = 6; // FlexIO function
        }
        
        // Configure FlexIO shifter for 16-bit parallel output
        _flexIO->SHIFTCTL[_flexIOShifter] = 
            FLEXIO_SHIFTCTL_TIMSEL(_flexIOTimer) |
            FLEXIO_SHIFTCTL_PINCFG(3) |         // Output on rising edge
            FLEXIO_SHIFTCTL_PINSEL(0) |         // Start from pin 0
            FLEXIO_SHIFTCTL_SMOD(2);            // 16-bit parallel output
            
        // Configure FlexIO timer
        _flexIO->TIMCTL[_flexIOTimer] = 
            FLEXIO_TIMCTL_TRGSEL(1) |           // Trigger on shifter status flag
            FLEXIO_TIMCTL_TIMOD(1);             // Dual 8-bit counters
            
        // Enable FlexIO
        _flexIO->CTRL = FLEXIO_CTRL_FLEXEN;
        
        // Setup DMA if supported
        if (supportsDMA()) {
            _dmaChannel = new DMAChannel();
            if (_dmaChannel) {
                _dmaChannel->begin();
                _dmaChannel->destination(_flexIO->SHIFTBUF[_flexIOShifter]);
            }
        }
    } else {
        // 8-bit mode setup (simpler GPIO)
        for (int i = 0; i < 8; i++) {
            pinMode(_dataPins[i], OUTPUT);
        }
    }
    #else
    // Other Teensy boards - basic GPIO setup
    for (int i = 0; i < (_is16Bit ? 16 : 8); i++) {
        pinMode(_dataPins[i], OUTPUT);
    }
    #endif
    
    pinMode(_csPin, OUTPUT);
    pinMode(_dcPin, OUTPUT);
    pinMode(_wrPin, OUTPUT);
    if (_rdPin >= 0) pinMode(_rdPin, OUTPUT);
    if (_rstPin >= 0) pinMode(_rstPin, OUTPUT);
    
    // Initial states
    digitalWrite(_csPin, HIGH);
    digitalWrite(_dcPin, HIGH);
    digitalWrite(_wrPin, HIGH);
    if (_rdPin >= 0) digitalWrite(_rdPin, HIGH);
    
    return true;
}
#endif

void TFT_Interface_Parallel::setupPins() {
    // Configure control pins
    if (_csPin >= 0) pinMode(_csPin, OUTPUT);
    if (_wrPin >= 0) pinMode(_wrPin, OUTPUT);
    if (_rdPin >= 0) pinMode(_rdPin, OUTPUT);
    if (_rstPin >= 0) pinMode(_rstPin, OUTPUT);
    if (_latchPin >= 0) pinMode(_latchPin, OUTPUT);

    // Configure data pins
    for (int i = 0; i < (_is16Bit ? 16 : 8); i++) {
        if (_dataPins[i] >= 0) pinMode(_dataPins[i], OUTPUT);
    }

    // Set default pin states
    if (_csPin >= 0) digitalWrite(_csPin, HIGH);
    if (_wrPin >= 0) digitalWrite(_wrPin, HIGH);
    if (_rdPin >= 0) digitalWrite(_rdPin, HIGH);
    if (_rstPin >= 0) {
        digitalWrite(_rstPin, HIGH);
        delay(5);
        digitalWrite(_rstPin, LOW);
        delay(20);
        digitalWrite(_rstPin, HIGH);
        delay(150);
    }
    if (_latchPin >= 0) digitalWrite(_latchPin, HIGH);
}

bool TFT_Interface_Parallel::supportsDMA() {
#if defined(ESP32) || defined(ARDUINO_ARCH_RP2040) || \
    (defined(CORE_TEENSY) && defined(__IMXRT1062__))
    return true;
#else
    return false;
#endif
}

bool TFT_Interface_Parallel::startDMAWrite(const uint8_t* data, size_t len) {
    // Platform-specific DMA implementation
    return false;
}

void TFT_Interface_Parallel::cleanupDMA() {
    // Platform-specific DMA cleanup
}

void TFT_Interface_Parallel::waitDMAComplete() {
    // Platform-specific DMA wait
}

void TFT_Interface_Parallel::setDataPinsOutput() {
    for (int i = 0; i < (_is16Bit ? 16 : 8); i++) {
        if (_dataPins[i] >= 0) pinMode(_dataPins[i], OUTPUT);
    }
}

void TFT_Interface_Parallel::setDataPinsInput() {
    for (int i = 0; i < (_is16Bit ? 16 : 8); i++) {
        if (_dataPins[i] >= 0) pinMode(_dataPins[i], INPUT);
    }
}

void TFT_Interface_Parallel::pulseWR() {
    digitalWrite(_wrPin, LOW);
    if (_writeDelay > 0) delayMicroseconds(_writeDelay);
    digitalWrite(_wrPin, HIGH);
}

void TFT_Interface_Parallel::pulseRD() {
    digitalWrite(_rdPin, LOW);
    if (_writeDelay > 0) delayMicroseconds(_writeDelay);
    digitalWrite(_rdPin, HIGH);
}

void TFT_Interface_Parallel::pulseLatch() {
    if (_latchPin >= 0) {
        digitalWrite(_latchPin, HIGH);
        digitalWrite(_latchPin, LOW);
    }
}

void TFT_Interface_Parallel::delayWrite() {
    if (_writeDelay > 0) delayMicroseconds(_writeDelay);
}

// Transaction Management
void TFT_Interface_Parallel::beginTransaction() {
    if (_csPin >= 0) digitalWrite(_csPin, LOW);
}

void TFT_Interface_Parallel::endTransaction() {
    if (_csPin >= 0) digitalWrite(_csPin, HIGH);
}

void TFT_Interface_Parallel::beginRead() {
    if (_csPin >= 0) digitalWrite(_csPin, LOW);
    setDataPinsInput();
}

void TFT_Interface_Parallel::endRead() {
    setDataPinsOutput();
    if (_csPin >= 0) digitalWrite(_csPin, HIGH);
}

void TFT_Interface_Parallel::begin_nin_write() {
    if (_csPin >= 0) digitalWrite(_csPin, LOW);
}

void TFT_Interface_Parallel::end_nin_write() {
    if (_csPin >= 0) digitalWrite(_csPin, HIGH);
}

// Viewport Management
void TFT_Interface_Parallel::setViewport(int32_t x, int32_t y, int32_t w, int32_t h, bool vpDatum) {
    _vpX = x;
    _vpY = y;
    _vpW = w;
    _vpH = h;
    _vpDatum = vpDatum;
    _vpActive = true;
}

void TFT_Interface_Parallel::resetViewport() {
    _vpX = 0;
    _vpY = 0;
    _vpW = _config.width;
    _vpH = _config.height;
    _vpDatum = false;
    _vpActive = false;
}

bool TFT_Interface_Parallel::checkViewport(int32_t x, int32_t y, int32_t w, int32_t h) {
    if (!_vpActive) return true;

    int32_t xe = x + w - 1;
    int32_t ye = y + h - 1;
    
    if (x < _vpX) return false;
    if (y < _vpY) return false;
    if (xe > (_vpX + _vpW - 1)) return false;
    if (ye > (_vpY + _vpH - 1)) return false;

    return true;
}

bool TFT_Interface_Parallel::clipAddrWindow(int32_t* x, int32_t* y, int32_t* w, int32_t* h) {
    if (!_vpActive) return true;

    if (*x < _vpX) {
        *w -= (_vpX - *x);
        *x = _vpX;
    }
    if (*y < _vpY) {
        *h -= (_vpY - *y);
        *y = _vpY;
    }
    if ((*x + *w) > (_vpX + _vpW)) *w = _vpX + _vpW - *x;
    if ((*y + *h) > (_vpY + _vpH)) *h = _vpY + _vpH - *y;

    return (*w > 0 && *h > 0);
}

bool TFT_Interface_Parallel::clipWindow(int32_t* xs, int32_t* ys, int32_t* xe, int32_t* ye) {
    if (!_vpActive) return true;

    if (*xs < _vpX) *xs = _vpX;
    if (*ys < _vpY) *ys = _vpY;
    if (*xe > (_vpX + _vpW - 1)) *xe = _vpX + _vpW - 1;
    if (*ye > (_vpY + _vpH - 1)) *ye = _vpY + _vpH - 1;

    return (*xs <= *xe && *ys <= *ye);
}

#if defined(CORE_TEENSY)
void TFT_Interface_Parallel::writeTeensy_8(uint8_t data) {
    #if defined(__IMXRT1062__)
    if (_is16Bit) {
        _flexIO->SHIFTBUF[_flexIOShifter] = data;
        while (!(_flexIO->SHIFTSTAT & (1 << _flexIOShifter))) ; // Wait for completion
    } else {
        for (int i = 0; i < 8; i++) {
            digitalWrite(_dataPins[i], (data >> i) & 0x01);
        }
    }
    #else
    // Direct GPIO for other Teensy boards
    for (int i = 0; i < 8; i++) {
        digitalWrite(_dataPins[i], (data >> i) & 0x01);
    }
    #endif
    pulseWR();
}

void TFT_Interface_Parallel::writeTeensy_16(uint16_t data) {
    #if defined(__IMXRT1062__)
    if (_is16Bit) {
        _flexIO->SHIFTBUF[_flexIOShifter] = data;
        while (!(_flexIO->SHIFTSTAT & (1 << _flexIOShifter))) ; // Wait for completion
    } else {
        // Split into two 8-bit writes
        writeTeensy_8(data >> 8);
        writeTeensy_8(data & 0xFF);
    }
    #else
    // Direct GPIO for other Teensy boards
    if (_is16Bit) {
        for (int i = 0; i < 16; i++) {
            digitalWrite(_dataPins[i], (data >> i) & 0x01);
        }
        pulseWR();
    } else {
        // Split into two 8-bit writes
        writeTeensy_8(data >> 8);
        writeTeensy_8(data & 0xFF);
    }
    #endif
}

bool TFT_Interface_Parallel::startDMAWrite(const uint8_t* data, size_t len) {
    #if defined(__IMXRT1062__)
    if (_is16Bit && _dmaChannel && len >= 32) {  // Only use DMA for larger transfers
        _dmaChannel->sourceBuffer((volatile uint16_t*)data, len/2);
        _dmaChannel->enable();
        return true;
    }
    #endif
    return false;
}

void TFT_Interface_Parallel::cleanupDMA() {
    #if defined(__IMXRT1062__)
    if (_dmaChannel) {
        _dmaChannel->disable();
    }
    #endif
}

void TFT_Interface_Parallel::waitDMAComplete() {
    #if defined(__IMXRT1062__)
    if (_dmaChannel) {
        while (!_dmaChannel->complete()) {
            yield();
        }
    }
    #endif
}
#endif

} // namespace TFT_Runtime
