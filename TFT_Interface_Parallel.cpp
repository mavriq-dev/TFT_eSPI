#include "TFT_Interface_Parallel.h"
#include <Arduino.h>
#if defined(CORE_TEENSY)
#include <DMAChannel.h>
#if defined(__MK66FX1M0__)
#include <kinetis.h>
#endif
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
        _dataPins[i] = config.parallel.data_pins[i];
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
    
    uint16_t data = 0;
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
    
    #if defined(__IMXRT1062__)
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
                _dmaChannel->disableOnCompletion();
#if defined(__IMXRT1062__)  // Teensy 4.x
                _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_ALWAYS);
#else  // Teensy 3.x
                _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_PORTA);
#endif
                _dmaActive = false;  // Initialize as not active
                _dmaInitialized = true;
            }
        }
    } else {
        // 8-bit mode setup (simpler GPIO)
        for (int i = 0; i < 8; i++) {
            pinMode(_dataPins[i], OUTPUT);
        }
    }
    #elif defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__)
    // Teensy 3.x specific implementation
    setupParallelBus();
    if (supportsDMA()) {
        initTeensyParallelDMA();
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
    setupParallelBus();
    // Configure control pins
    if (_csPin >= 0) pinMode(_csPin, OUTPUT);
    if (_wrPin >= 0) pinMode(_wrPin, OUTPUT);
    if (_rdPin >= 0) pinMode(_rdPin, OUTPUT);
    if (_rstPin >= 0) pinMode(_rstPin, OUTPUT);
    if (_latchPin >= 0) pinMode(_latchPin, OUTPUT);

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

void TFT_Interface_Parallel::setupParallelBus() {
    #if defined(CORE_TEENSY)
        #if defined(__IMXRT1062__)  // Teensy 4.0
            setupTeensy40Parallel();
        #elif defined(__MK66FX1M0__) // Teensy 3.6
            setupTeensy36Parallel();
        #elif defined(__MK64FX512__) // Teensy 3.5
            setupTeensy35Parallel();
        #elif defined(__MK20DX256__) // Teensy 3.2/3.1
            setupTeensy32Parallel();
        #elif defined(__MK20DX128__) // Teensy 3.0
            setupTeensy30Parallel();
        #endif
    #elif defined(ESP32)
        setupESP32Parallel();
    #elif defined(ESP8266)
        setupESP8266Parallel();
    #elif defined(ARDUINO_ARCH_RP2040)
        setupRP2040Parallel();
    #elif defined(STM32)
        setupSTM32Parallel();
    #elif defined(ARDUINO_SAM_DUE)
        setupDueParallel();
    #elif defined(ARDUINO_AVR_MEGA2560)
        setupMegaParallel();
    #endif

    setupCommonPins();
}

void TFT_Interface_Parallel::setupCommonPins() {
    // Configure control pins (common for all platforms)
    pinMode(_config.parallel.wr_pin, OUTPUT);
    pinMode(_config.parallel.rd_pin, OUTPUT);
    pinMode(_config.parallel.cs_pin, OUTPUT);
    pinMode(_config.parallel.dc_pin, OUTPUT);
    
    // Configure data pins
    for (uint8_t i = 0; i < _config.parallel.bus_width; i++) {
        if (_config.parallel.data_pins[i] != -1) {
            pinMode(_config.parallel.data_pins[i], OUTPUT);
        }
    }
    
    // Configure optional control pins
    if (_config.parallel.ale_pin != -1) {
        pinMode(_config.parallel.ale_pin, OUTPUT);
    }
    if (_config.parallel.te_pin != -1) {
        pinMode(_config.parallel.te_pin, INPUT);
    }
}

uint8_t TFT_Interface_Parallel::getPinBitPosition(uint8_t pin) {
#if defined(__IMXRT1062__)  // Teensy 4.0
    return (pin & 0x1F);  // 32 bits per port
#elif defined(CORE_TEENSY)  // Teensy 3.x series (3.0, 3.1, 3.2, 3.5, 3.6)
    return (pin & 0xF);   // 16 bits per port for MK series
#elif defined(ESP32)
    return (pin & 0x1F);  // ESP32 has 32 GPIO pins
#elif defined(ESP8266)
    return (pin & 0xF);   // ESP8266 has 17 GPIO pins
#elif defined(ARDUINO_ARCH_RP2040)
    return (pin & 0x1F);  // RP2040 has 30 GPIO pins
#elif defined(STM32)
    return (pin & 0xF);   // Most STM32 ports are 16-bit
#elif defined(ARDUINO_SAM_DUE)
    return (pin & 0x1F);  // SAM3X8E has 32-bit ports
#elif defined(ARDUINO_AVR_MEGA2560)
    return (pin & 0x7);   // ATmega2560 has 8-bit ports
#else
    #error "Platform not supported for parallel interface"
#endif
}

#if defined(CORE_TEENSY)
#if defined(__IMXRT1062__)  // Teensy 4.0/4.1
void TFT_Interface_Parallel::setupTeensy40Parallel() {
    // Configure FlexIO for parallel interface
    uint32_t clock = F_CPU_ACTUAL / 2; // Start with half CPU clock
    
    // Initialize data pins using FlexIO
    for (uint8_t i = 0; i < _config.parallel.bus_width; i++) {
        if (_config.parallel.data_pins[i] != -1) {
            _dataPins[i] = _config.parallel.data_pins[i];
        }
    }
}
#endif
#endif

#if defined(ESP32)
void TFT_Interface_Parallel::setupESP32Parallel() {
    // Configure GPIO matrix for parallel interface
    for (uint8_t i = 0; i < _config.parallel.bus_width; i++) {
        if (_config.parallel.data_pins[i] != -1) {
            gpio_pad_select_gpio(_config.parallel.data_pins[i]);
            gpio_set_direction((gpio_num_t)_config.parallel.data_pins[i], GPIO_MODE_OUTPUT);
        }
    }
}
#endif

#if defined(ESP8266)
void TFT_Interface_Parallel::setupESP8266Parallel() {
    // Configure GPIO registers for parallel interface
    for (uint8_t i = 0; i < _config.parallel.bus_width; i++) {
        if (_config.parallel.data_pins[i] != -1) {
            pinMode(_config.parallel.data_pins[i], OUTPUT);
            GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, 1 << _config.parallel.data_pins[i]);
        }
    }
}
#endif

#if defined(ARDUINO_ARCH_RP2040)
void TFT_Interface_Parallel::setupRP2040Parallel() {
    // Configure GPIO for parallel interface using PIO
    for (uint8_t i = 0; i < _config.parallel.bus_width; i++) {
        if (_config.parallel.data_pins[i] != -1) {
            gpio_init(_config.parallel.data_pins[i]);
            gpio_set_dir(_config.parallel.data_pins[i], GPIO_OUT);
        }
    }
}
#endif

#if defined(STM32)
void TFT_Interface_Parallel::setupSTM32Parallel() {
    // Configure GPIO for parallel interface on STM32
    for (uint8_t i = 0; i < _config.parallel.bus_width; i++) {
        if (_config.parallel.data_pins[i] != -1) {
            pinMode(_config.parallel.data_pins[i], OUTPUT);
        }
    }
}
#endif

#if defined(ARDUINO_SAM_DUE)
void TFT_Interface_Parallel::setupDueParallel() {
    // Configure parallel interface for Due
    for (uint8_t i = 0; i < _config.parallel.bus_width; i++) {
        if (_config.parallel.data_pins[i] != -1) {
            pinMode(_config.parallel.data_pins[i], OUTPUT);
            PIO_Configure(g_APinDescription[_config.parallel.data_pins[i]].pPort,
                         PIO_OUTPUT_0,
                         g_APinDescription[_config.parallel.data_pins[i]].ulPin,
                         PIO_DEFAULT);
        }
    }
}
#endif

#if defined(ARDUINO_AVR_MEGA2560)
void TFT_Interface_Parallel::setupMegaParallel() {
    // Configure parallel interface for Mega
    for (uint8_t i = 0; i < _config.parallel.bus_width; i++) {
        if (_config.parallel.data_pins[i] != -1) {
            pinMode(_config.parallel.data_pins[i], OUTPUT);
        }
    }
}
#endif

bool TFT_Interface_Parallel::supportsDMA() {
#if defined(ESP32) || defined(ARDUINO_ARCH_RP2040) || \
    (defined(CORE_TEENSY) && defined(__IMXRT1062__))
    return true;
#else
    return false;
#endif
}

bool TFT_Interface_Parallel::startDMAWrite(const uint8_t* data, size_t len) {
    if (!_dmaInitialized || !data || len == 0) {
        return false;
    }

    #if defined(CORE_TEENSY)
    #if defined(__MK66FX1M0__)  // Teensy 3.6
    if (_dmaStatus == DMA_ACTIVE) {
        waitDMAComplete();  // Wait for any previous transfer to complete
    }

    // Allocate or reallocate buffer if needed
    size_t requiredSize = min(len, (size_t)32768);  // Max 32KB buffer
    if (_dmaBufSize < requiredSize) {
        if (_dmaBuf) {
            delete[] _dmaBuf;
        }
        _dmaBuf = new uint8_t[requiredSize];
        _dmaBufSize = requiredSize;
    }

    if (!_dmaBuf) {
        return false;  // Memory allocation failed
    }

    // Copy initial data to DMA buffer
    size_t blockSize = min(len, _dmaBufSize);
    memcpy(_dmaBuf, data, blockSize);
    
    // Set up transfer
    _dmaChannel->disable();
    #if defined(CORE_TEENSY) && defined(__MK66FX1M0__)
        _dmaChannel->source((volatile const uint8_t&)*_dmaBuf);
        _dmaChannel->transferSize(1);  // 8-bit transfers
        _dmaChannel->transferCount(blockSize);  // Set transfer count to block size
    #else
        _dmaChannel->source(_dmaBuf);
        _dmaChannel->transferCount(blockSize);
    #endif
    
    // Initialize transfer tracking
    _dmaRemaining = len - blockSize;
    _dmaSent = 0;
    _dmaStatus = DMA_ACTIVE;
    
    // Start transfer
    _dmaChannel->enable();
    return true;
    #endif
    #endif

    return false;
}

void TFT_Interface_Parallel::cleanupDMA() {
    #if defined(__IMXRT1062__)
    if (_dmaChannel) {
        _dmaChannel->disable();
        delete _dmaChannel;
        _dmaChannel = nullptr;
    }
    #if defined(__MK66FX1M0__)  // Teensy 3.6
    if (_dmaBuf) {
        delete[] _dmaBuf;
        _dmaBuf = nullptr;
    }
    _dmaBufSize = 0;
    _dmaRemaining = 0;
    _dmaSent = 0;
    #endif
    #endif
    _dmaInitialized = false;
    _dmaStatus = DMA_INACTIVE;
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
TFT_Interface_Parallel* TFT_Interface_Parallel::_dmaActiveInstance = nullptr;

static void dmaInterruptHandler(void) {
    if (TFT_Interface_Parallel* instance = TFT_Interface_Parallel::getDMAActiveInstance()) {
        instance->dmaInterrupt();
    }
}

void TFT_Interface_Parallel::dmaInterrupt() {
    #if defined(CORE_TEENSY) && defined(__MK66FX1M0__)
    if (_dmaChannel) {
        if (_dmaChannel->error()) {
            _dmaStatus = DMA_ERROR;
            _dmaChannel->clearError();
            _dmaChannel->disable();
        } else {
            _dmaSent += _dmaChannel->TCD->CITER;  // Get transfer count from TCD
            if (_dmaRemaining > 0) {
                // More data to send
                size_t blockSize = min(_dmaRemaining, _dmaBufSize);
                _dmaChannel->TCD->SADDR = (volatile const void*)(_dmaBuffer + _dmaSent); // Update source address
                _dmaChannel->transferCount(blockSize);  // Set new transfer count
                _dmaChannel->enable();
                _dmaRemaining -= blockSize;
            } else {
                // Transfer complete
                _dmaStatus = DMA_COMPLETE;
                _dmaChannel->disable();
            }
        }
    }
    #endif
}

void TFT_Interface_Parallel::initTeensy36DMA() {
    if (!_dmaInitialized) {
        _dmaChannel = new DMAChannel();
        if (_dmaChannel) {
            _dmaActiveInstance = this;  // Set this instance as the active one
            _dmaChannel->disable();
            _dmaChannel->destination(*_portSet);
            _dmaChannel->disableOnCompletion();
            _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_PORTA);
            _dmaChannel->interruptAtCompletion();
            _dmaChannel->attachInterrupt(dmaInterruptHandler);
            
            // Configure for fast parallel transfers
            setupParallelDMA();
            _dmaInitialized = true;
        }
    }
}

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
    #elif defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__)
    // Direct GPIO for Teensy 3.x
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
    #elif defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__)
    // Direct GPIO for Teensy 3.x
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

void TFT_Interface_Parallel::initTeensyParallelDMA() {
    if (!_dmaInitialized) {
        _dmaChannel = new DMAChannel();
        if (_dmaChannel) {
            _dmaChannel->disable();
            _dmaChannel->destination(*_dataPort);
            _dmaChannel->disableOnCompletion();
#if defined(__IMXRT1062__)  // Teensy 4.x
            _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_ALWAYS);
#elif defined(__MK66FX1M0__)  // Teensy 3.6
            _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_PORTA);
#endif
            _dmaActive = false;  // Initialize as not active
            _dmaInitialized = true;
        }
    }
}

#if defined(__MK66FX1M0__)  // Teensy 3.6
void TFT_Interface_Parallel::setupTeensy36Parallel() {
    // Initialize data port registers
    _dataPort = portOutputRegister(digitalPinToPort(_dataPins[0]));
    _dataMask = 0;
    for (int i = 0; i < (_is16Bit ? 16 : 8); i++) {
        _dataMask |= digitalPinToBitMask(_dataPins[i]);
    }
    _dataPortMask = _dataMask;
    
    // Initialize port manipulation registers
    _portMode = portModeRegister(digitalPinToPort(_dataPins[0]));
    _portSet = portSetRegister(digitalPinToPort(_dataPins[0]));
    _portClear = portClearRegister(digitalPinToPort(_dataPins[0]));
    
    // Initialize WR pin registers
    _wrPortSet = portSetRegister(digitalPinToPort(_wrPin));
    _wrPortClear = portClearRegister(digitalPinToPort(_wrPin));
    _wrPinMask = digitalPinToBitMask(_wrPin);
    
    // Configure pins
    for (int i = 0; i < (_is16Bit ? 16 : 8); i++) {
        pinMode(_dataPins[i], OUTPUT);
    }
    pinMode(_wrPin, OUTPUT);
    digitalWrite(_wrPin, HIGH);
}

void TFT_Interface_Parallel::writeTeensy36_16(uint16_t data) {
    // Fast direct port manipulation for Teensy 3.6
    *_portSet = _dataPortMask;  // Set all data pins high
    *_portClear = ~data & _dataPortMask;  // Clear bits that should be 0
    
    // Fast WR pulse using direct port manipulation
    *_wrPortClear = _wrPinMask;
    __asm__ volatile("nop\n\t");  // Small delay
    *_wrPortSet = _wrPinMask;
}

void TFT_Interface_Parallel::writeTeensy36_8(uint8_t data) {
    // Fast direct port manipulation for 8-bit writes
    *_portSet = _dataPortMask & 0xFF;  // Set lower 8 bits high
    *_portClear = (~data & 0xFF) & _dataPortMask;  // Clear bits that should be 0
    
    // Fast WR pulse
    *_wrPortClear = _wrPinMask;
    __asm__ volatile("nop\n\t");
    *_wrPortSet = _wrPinMask;
}


void TFT_Interface_Parallel::setupParallelDMA() {
    // Configure port directions
    *_portMode |= _dataPortMask;  // Set data pins as output
    
    // Configure DMA transfer settings
    _dmaChannel->transferSize(2);  // 16-bit transfers
    _dmaChannel->transferCount(1);
    
    // Enable port interrupts for DMA triggering
    NVIC_SET_PRIORITY(IRQ_PORTA, 64);
    NVIC_ENABLE_IRQ(IRQ_PORTA);
}

void TFT_Interface_Parallel::dmaInterrupt() {
    if (_dmaStatus == DMA_ACTIVE && _dmaChannel) {
        _dmaStatus = DMA_COMPLETE;
        _dmaChannel->clearInterrupt();
        
        // Chain next transfer if available
        if (_dmaRemaining > 0) {
            size_t transferSize = min(_dmaRemaining, (size_t)32768);
            #if defined(__IMXRT1062__) // Teensy 4.0/4.1
                _dmaChannel->sourceBuffer(_dmaBuffer + _dmaSent);
                _dmaChannel->transferSize(transferSize);
            #elif defined(__MK66FX1M0__) || defined(__MK64FX512__) // Teensy 3.6/3.5
                _dmaChannel->TCD->SADDR = (volatile const void*)(_dmaBuffer + _dmaSent);
                _dmaChannel->transferCount(transferSize);
            #elif defined(__MK20DX256__) || defined(__MK20DX128__) // Teensy 3.2/3.1/3.0
                _dmaChannel->TCD->SADDR = (volatile const void*)(_dmaBuffer + _dmaSent);
                _dmaChannel->transferCount(transferSize);
            #endif
            _dmaChannel->enable();
            
            _dmaRemaining -= transferSize;
            _dmaSent += transferSize;
        }
    }
}
#endif

#endif

} // namespace TFT_Runtime
