#ifndef _TFT_INTERFACE_PARALLEL_CPP_
#define _TFT_INTERFACE_PARALLEL_CPP_

#include "TFT_Interface_Parallel.h"
#include <Arduino.h>

#if defined(CORE_TEENSY)
#include <DMAChannel.h>
#if defined(__MK66FX1M0__)
#include <kinetis.h>
#endif
#endif

namespace TFT_Runtime {

// Forward declarations
static void dmaInterruptHandler(void);

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
    success = setupParallelBus();
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
    write8(cmd);
    digitalWrite(_csPin, HIGH);
}

void TFT_Interface_Parallel::writeData(uint8_t data) {
    digitalWrite(_csPin, LOW);
    digitalWrite(_dcPin, HIGH);  // Data mode
    write8(data);
    digitalWrite(_csPin, HIGH);
}

void TFT_Interface_Parallel::writeData16(uint16_t data) {
    digitalWrite(_csPin, LOW);
    digitalWrite(_dcPin, HIGH);  // Data mode
    write16(data);
    digitalWrite(_csPin, HIGH);
}

uint8_t TFT_Interface_Parallel::readData() {
    digitalWrite(_csPin, LOW);
    digitalWrite(_dcPin, HIGH);  // Data mode
    setDataPinsInput();
    
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



void TFT_Interface_Parallel::writeDataBlock16(const uint16_t* data, size_t len) {
    if (len == 0) return;

    digitalWrite(_csPin, LOW);
    digitalWrite(_dcPin, HIGH);  // Data mode

    if (_is16Bit) {
        for (size_t i = 0; i < len; i++) {
            write16(data[i]);
        }
    } else {
        for (size_t i = 0; i < len; i++) {
            writeData((data[i] >> 8) & 0xFF);
            writeData(data[i] & 0xFF);
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
        data[i] = read8();
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
            data[i] = read16();
        }
    } else {
        for (size_t i = 0; i < len; i++) {
            data[i] = (readData() << 8) | readData();
        }
    }

    setDataPinsOutput();
    digitalWrite(_csPin, HIGH);
}

void TFT_Interface_Parallel::write8(uint8_t data) {
    #if defined(ESP32)
        GPIO.out_w1tc = (1 << _wrPin);
        GPIO.out_w1tc.val = _dataMask;
        GPIO.out_w1ts.val = ((uint32_t)data << _dataShift) & _dataMask;
        GPIO.out_w1ts = (1 << _wrPin);
    #elif defined(ESP8266)
        GPOC = _wrPinMask;
        GPOC = _dataMask;
        GPOS = ((uint32_t)data << _dataShift) & _dataMask;
        GPOS = _wrPinMask;
    #elif defined(ARDUINO_ARCH_RP2040)
        pio_sm_put_blocking(_pio, _sm, data);
    #elif defined(ARDUINO_SAM_DUE)
        REG_PIOC_CODR = _dataMask;
        REG_PIOC_SODR = ((uint32_t)data << _dataShift) & _dataMask;
        REG_PIOC_CODR = _wrPinMask;
        REG_PIOC_SODR = _wrPinMask;
    #elif defined(__AVR__)
        *_dataPort = (*_dataPort & ~_dataMask) | ((data << _dataShift) & _dataMask);
        *_wrPort &= ~_wrPinMask;
        *_wrPort |= _wrPinMask;
    #elif defined(CORE_TEENSY)
    #if defined(__IMXRT1062__)
        // Teensy 4.0/4.1
        _flexIO->SHIFTBUF[_flexIOShifter] = data;
        while (!(_flexIO->SHIFTSTAT & (1 << _flexIOShifter))) ; // Wait for shift complete
    #elif defined(__MK66FX1M0__)
        // Teensy 3.6 - Optimized using direct port manipulation
        #if defined(KINETISK)
        *_wrPortClear = _wrPinMask;  // WR low
        *_dataPortClr = _dataMask;   // Clear all data bits
        *_dataPortSet = ((uint32_t)data << _dataShift) & _dataMask;  // Set new data bits
        *_wrPortSet = _wrPinMask;    // WR high
        #else
        // Fallback for non-KINETISK architecture (shouldn't occur on Teensy 3.6)
        *_dataPort = (*_dataPort & ~_dataMask) | ((data << _dataShift) & _dataMask);
        *_wrPortClear = _wrPinMask;
        *_wrPortSet = _wrPinMask;
        #endif
    #elif defined(__MK64FX512__)
        // Teensy 3.5
        digitalWriteFast(_wrPin, LOW);
        for(uint8_t i = 0; i < 8; i++) {
            digitalWriteFast(_dataPins[i], (data >> i) & 0x01);
        }
        digitalWriteFast(_wrPin, HIGH);
    #elif defined(__MK20DX256__) || defined(__MK20DX128__)
        // Teensy 3.2/3.1/3.0
        digitalWriteFast(_wrPin, LOW);
        for(uint8_t i = 0; i < 8; i++) {
            digitalWriteFast(_dataPins[i], (data >> i) & 0x01);
        }
        digitalWriteFast(_wrPin, HIGH);
    #endif
    #endif
    
    if (_writeDelay > 0) delayMicroseconds(_writeDelay);
}

void TFT_Interface_Parallel::write16(uint16_t data) {
    #if defined(ESP32)
        GPIO.out_w1tc = (1 << _wrPin);
        GPIO.out_w1tc.val = _dataMask;
        GPIO.out_w1ts.val = ((uint32_t)(data >> 8) << _dataShift) & _dataMask;
        GPIO.out_w1ts = (1 << _wrPin);
        GPIO.out_w1tc = (1 << _wrPin);
        GPIO.out_w1tc.val = _dataMask;
        GPIO.out_w1ts.val = ((uint32_t)(data & 0xFF) << _dataShift) & _dataMask;
        GPIO.out_w1ts = (1 << _wrPin);
    #elif defined(ESP8266)
        GPOC = _wrPinMask;
        GPOC = _dataMask;
        GPOS = ((uint32_t)(data >> 8) << _dataShift) & _dataMask;
        GPOS = _wrPinMask;
        GPOC = _wrPinMask;
        GPOC = _dataMask;
        GPOS = ((uint32_t)(data & 0xFF) << _dataShift) & _dataMask;
        GPOS = _wrPinMask;
    #elif defined(ARDUINO_ARCH_RP2040)
        pio_sm_put_blocking(_pio, _sm, data >> 8);
        pio_sm_put_blocking(_pio, _sm, data & 0xFF);
    #elif defined(ARDUINO_SAM_DUE)
        REG_PIOC_CODR = _dataMask;
        REG_PIOC_SODR = ((uint32_t)(data >> 8) << _dataShift) & _dataMask;
        REG_PIOC_CODR = _wrPinMask;
        REG_PIOC_SODR = _wrPinMask;
        REG_PIOC_CODR = _dataMask;
        REG_PIOC_SODR = ((uint32_t)(data & 0xFF) << _dataShift) & _dataMask;
        REG_PIOC_CODR = _wrPinMask;
        REG_PIOC_SODR = _wrPinMask;
    #elif defined(__AVR__)
        *_dataPort = (*_dataPort & ~_dataMask) | (((data >> 8) << _dataShift) & _dataMask);
        *_wrPort &= ~_wrPinMask;
        *_wrPort |= _wrPinMask;
        *_dataPort = (*_dataPort & ~_dataMask) | (((data & 0xFF) << _dataShift) & _dataMask);
        *_wrPort &= ~_wrPinMask;
        *_wrPort |= _wrPinMask;
    #elif defined(CORE_TEENSY)
    #if defined(__IMXRT1062__)
        // Teensy 4.0/4.1
        _flexIO->SHIFTBUF[_flexIOShifter] = data >> 8;
        while (!(_flexIO->SHIFTSTAT & (1 << _flexIOShifter))) ;
        _flexIO->SHIFTBUF[_flexIOShifter] = data & 0xFF;
        while (!(_flexIO->SHIFTSTAT & (1 << _flexIOShifter))) ;
    #elif defined(__MK66FX1M0__)
        // Teensy 3.6 - Optimized using direct port manipulation
        #if defined(KINETISK)
        *_wrPortClear = _wrPinMask;  // WR low
        *_dataPortClr = _dataMask;   // Clear all data bits
        *_dataPortSet = ((uint32_t)(data >> 8) << _dataShift) & _dataMask;  // Set new data bits
        *_wrPortSet = _wrPinMask;    // WR high
        *_wrPortClear = _wrPinMask;  // WR low
        *_dataPortClr = _dataMask;   // Clear all data bits
        *_dataPortSet = ((uint32_t)(data & 0xFF) << _dataShift) & _dataMask;  // Set new data bits
        *_wrPortSet = _wrPinMask;    // WR high
        #else
        // Fallback for non-KINETISK architecture (shouldn't occur on Teensy 3.6)
        *_dataPort = (*_dataPort & ~_dataMask) | (((data >> 8) << _dataShift) & _dataMask);
        *_wrPortClear = _wrPinMask;
        *_wrPortSet = _wrPinMask;
        *_dataPort = (*_dataPort & ~_dataMask) | (((data & 0xFF) << _dataShift) & _dataMask);
        *_wrPortClear = _wrPinMask;
        *_wrPortSet = _wrPinMask;
        #endif
    #elif defined(__MK64FX512__)
        // Teensy 3.5
        digitalWriteFast(_wrPin, LOW);
        for(uint8_t i = 0; i < 8; i++) {
            digitalWriteFast(_dataPins[i], (data >> (i + 8)) & 0x01);
        }
        digitalWriteFast(_wrPin, HIGH);
        digitalWriteFast(_wrPin, LOW);
        for(uint8_t i = 0; i < 8; i++) {
            digitalWriteFast(_dataPins[i], (data >> i) & 0x01);
        }
        digitalWriteFast(_wrPin, HIGH);
    #elif defined(__MK20DX256__) || defined(__MK20DX128__)
        // Teensy 3.2/3.1/3.0
        digitalWriteFast(_wrPin, LOW);
        for(uint8_t i = 0; i < 8; i++) {
            digitalWriteFast(_dataPins[i], (data >> (i + 8)) & 0x01);
        }
        digitalWriteFast(_wrPin, HIGH);
        digitalWriteFast(_wrPin, LOW);
        for(uint8_t i = 0; i < 8; i++) {
            digitalWriteFast(_dataPins[i], (data >> i) & 0x01);
        }
        digitalWriteFast(_wrPin, HIGH);
    #endif
    #endif
    
    if (_writeDelay > 0) delayMicroseconds(_writeDelay);
}

void TFT_Interface_Parallel::writeDataBlock(const uint8_t* data, size_t len) {
    if (len == 0) return;

    digitalWrite(_csPin, LOW);
    digitalWrite(_dcPin, HIGH);  // Data mode

    #if defined(CORE_TEENSY) && defined(__MK66FX1M0__)
    if (supportsDMA() && startDMAWrite(data, len)) {
        return;  // DMA transfer started successfully
    }
    #endif

    // Fallback to bit-banging if DMA is not available or failed
    for (size_t i = 0; i < len; i++) {
        write8(data[i]);
    }

    digitalWrite(_csPin, HIGH);
}

void TFT_Interface_Parallel::setupPins() {
    #if defined(__MK66FX1M0__)  // Teensy 3.6
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
    #else

    #endif
}

void TFT_Interface_Parallel::setupParallelBus() {
    #if defined(CORE_TEENSY)
        #if defined(__IMXRT1062__)  // Teensy 4.0
            // Configure FlexIO for parallel interface
            uint32_t clock = F_CPU_ACTUAL / 2; // Start with half CPU clock
            
            // Initialize data pins using FlexIO
            for (uint8_t i = 0; i < _config.parallel.bus_width; i++) {
                if (_config.parallel.data_pins[i] != -1) {
                    _dataPins[i] = _config.parallel.data_pins[i];
                }
            }
        #elif defined(__MK66FX1M0__) // Teensy 3.6
        // Configure data pins using direct port access
            for (uint8_t i = 0; i < _config.parallel.bus_width; i++) {
                if (_config.parallel.data_pins[i] != -1) {
                    uint8_t pin = _config.parallel.data_pins[i];
                    *portConfigRegister(pin) = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
                    *portModeRegister(pin) |= digitalPinToBitMask(pin);  // Set to output
                }
            }

            // Configure control pins for maximum speed
            if (_wrPin >= 0) {
                *portConfigRegister(_wrPin) = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
                *portModeRegister(_wrPin) |= digitalPinToBitMask(_wrPin);
            }

            // Initialize DMA if supported
            if (_config.dma_enabled) {
                _dmaChannel = new DMAChannel();
                if (_dmaChannel) {
                    _dmaChannel->disable();
                    _dmaChannel->destination((volatile uint8_t&)GPIOD_PDOR);
                    _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_PORTD);
                    _dmaInitialized = true;
                }
            }
        #elif defined(__MK64FX512__) // Teensy 3.5
            setupTeensy35Parallel();
        #elif defined(__MK20DX256__) // Teensy 3.2/3.1
            setupTeensy32Parallel();
        #elif defined(__MK20DX128__) // Teensy 3.0
            setupTeensy30Parallel();
        #endif
    #elif defined(ESP32)
        // Configure GPIO matrix for parallel interface
        for (uint8_t i = 0; i < _config.parallel.bus_width; i++) {
            if (_config.parallel.data_pins[i] != -1) {
                gpio_pad_select_gpio(_config.parallel.data_pins[i]);
                gpio_set_direction((gpio_num_t)_config.parallel.data_pins[i], GPIO_MODE_OUTPUT);
            }
        }
    #elif defined(ESP8266)
        // Configure GPIO registers for parallel interface
        for (uint8_t i = 0; i < _config.parallel.bus_width; i++) {
            if (_config.parallel.data_pins[i] != -1) {
                pinMode(_config.parallel.data_pins[i], OUTPUT);
                GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, 1 << _config.parallel.data_pins[i]);
            }
        }
    #elif defined(ARDUINO_ARCH_RP2040)
        // Configure GPIO for parallel interface using PIO
        for (uint8_t i = 0; i < _config.parallel.bus_width; i++) {
            if (_config.parallel.data_pins[i] != -1) {
                gpio_init(_config.parallel.data_pins[i]);
                gpio_set_dir(_config.parallel.data_pins[i], GPIO_OUT);
            }
        }
    #elif defined(STM32)
        // Configure GPIO for parallel interface on STM32
        for (uint8_t i = 0; i < _config.parallel.bus_width; i++) {
            if (_config.parallel.data_pins[i] != -1) {
                pinMode(_config.parallel.data_pins[i], OUTPUT);
            }
        }
    #elif defined(ARDUINO_SAM_DUE)
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
    #elif defined(ARDUINO_AVR_MEGA2560)
        // Configure parallel interface for Mega
        for (uint8_t i = 0; i < _config.parallel.bus_width; i++) {
            if (_config.parallel.data_pins[i] != -1) {
                pinMode(_config.parallel.data_pins[i], OUTPUT);
            }
        }
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

bool TFT_Interface_Parallel::supportsDMA() {
#if defined(ESP32) || defined(ARDUINO_ARCH_RP2040) || \
    (defined(CORE_TEENSY) && defined(__IMXRT1062__))
    return true;
#else
    return false;
#endif
}

#if defined(CORE_TEENSY)
TFT_Interface_Parallel* TFT_Interface_Parallel::_dmaActiveInstance = nullptr;

static void dmaInterruptHandler(void) {
    if (TFT_Interface_Parallel::getDMAActiveInstance()) {
        TFT_Interface_Parallel::getDMAActiveInstance()->dmaInterrupt();
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
                memcpy(_dmaBuf, (uint8_t*)(_dmaBuf + _dmaSent), blockSize);
                
                _dmaChannel->disable();
                _dmaChannel->transferCount(blockSize);
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

bool TFT_Interface_Parallel::startDMAWrite(const uint8_t* data, size_t len) {
    if (!_dmaInitialized || !data || len == 0) {
        return false;
    }

    #if defined(CORE_TEENSY) && defined(__MK66FX1M0__)  // Teensy 3.6
        if (_dmaStatus == DMA_ACTIVE) {
            waitDMAComplete();  // Wait for any previous transfer to complete
        }

        // Optimize buffer size - Teensy 3.6 has 256KB RAM
        size_t requiredSize = min(len, (size_t)65536);  // Use 64KB buffer for better performance
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
        
        // Configure DMA for optimal performance
        _dmaChannel->disable();
        _dmaChannel->sourceBuffer(_dmaBuf, blockSize);
        _dmaChannel->destination(*_dataPort);
        _dmaChannel->transferSize(1);  // 8-bit transfers
        _dmaChannel->transferCount(blockSize);
        
        // Enable interrupt for completion and error handling
        _dmaChannel->interruptAtCompletion();
        _dmaChannel->attachInterrupt(dmaInterruptHandler);
        
        // Initialize transfer tracking
        _dmaRemaining = len - blockSize;
        _dmaSent = 0;
        _dmaStatus = DMA_ACTIVE;
        _dmaActiveInstance = this;
        
        // Start transfer with optimized timing
        _dmaChannel->enable();
        return true;
    #endif

    return false;
}

#endif

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

} // namespace TFT_Runtime

#endif