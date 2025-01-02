#ifndef _TFT_INTERFACE_PARALLEL_CPP_
#define _TFT_INTERFACE_PARALLEL_CPP_

#include "TFT_Interface_Parallel.h"
#include <Arduino.h>

#if defined(CORE_TEENSY)
    #if defined(__MK64FX512__) || defined(__MK66FX1M0__)
        // DMA register definitions for Teensy 3.5/3.6
        #ifndef DMA_CSR_INTMAJOR
        #define DMA_CSR_INTMAJOR    ((uint16_t)(1<<2))
        #endif
        #ifndef DMA_ATTR_SSIZE
        #define DMA_ATTR_SSIZE(n)   (((n) & 0x7)<<0)
        #endif
        #ifndef DMA_ATTR_DSIZE
        #define DMA_ATTR_DSIZE(n)   (((n) & 0x7)<<3)
        #endif
    #endif
#endif

#if defined(CORE_TEENSY)
    #if defined(__IMXRT1062__)
        // Teensy 4.x DMA interrupt handler is defined by the core library
        extern "C" void dmaInterruptHandler(void);
    #endif
#endif

namespace TFT_Runtime {

// Static member initialization
TFT_Interface_Parallel* TFT_Interface_Parallel::_dmaActiveInstance = nullptr;

#if defined(CORE_TEENSY) && (defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__))
void dmaInterruptHandler(void) {  // Implementation without namespace qualification
    if (TFT_Runtime::TFT_Interface_Parallel::_dmaActiveInstance) {
        TFT_Runtime::TFT_Interface_Parallel::_dmaActiveInstance->dmaInterrupt();
    }
}
#endif

// Forward declarations
void TFT_Interface_Parallel::dmaInterrupt() {
#if defined(CORE_TEENSY)
    #if defined(__IMXRT1062__)  // Teensy 4.x
        if (_dmaChannel && _dmaChannel->complete()) {
            _dmaChannel->clearComplete();
            _dmaChannel->clearInterrupt();
            
            // Switch DMA buffers if double buffering
            if (_dmaBuffer1 && _dmaBuffer2) {
                _currentDmaBuffer = (_currentDmaBuffer == _dmaBuffer1) ? _dmaBuffer2 : _dmaBuffer1;
                _dmaBufferReady = true;
            }
        }
    #elif defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__)  // All Teensy 3.x
        if (_dmaChannel && _dmaChannel->complete()) {
            _dmaChannel->clearInterrupt();
            _dmaChannel->clearComplete();
            
            // Common DMA handling for all Teensy 3.x series
            if (_dmaBuffer1 && _dmaBuffer2) {
                _currentDmaBuffer = (_currentDmaBuffer == _dmaBuffer1) ? _dmaBuffer2 : _dmaBuffer1;
                _dmaBufferReady = true;
                
                // Configure next transfer if needed
                if (_dmaBufferReady) {
                    _dmaChannel->TCD->SADDR = _currentDmaBuffer;
                    _dmaChannel->TCD->DADDR = _dataPort;
                }
            }
        }
    #endif
#endif

    if (supportsDMA()) {
        if (!_dmaInitialized) {
            // Allocate DMA channel
            _dmaChannel = new DMAChannel();
            if (_dmaChannel == nullptr) return;

            _dmaChannel->disable();
            
            #if defined(__IMXRT1062__)
                // Teensy 4.x specific DMA setup
                _dmaChannel->destination(*(volatile uint32_t*)(&FLEXIO2_SHIFTBUF0 + _flexIOShifter));
                _dmaChannel->triggerAtHardwareEvent(45); // FlexIO2 DMA request channel
                FLEXIO2_SHIFTSDEN |= (1 << _flexIOShifter);  // Enable DMA request
            #elif defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__)  // Teensy 3.6/3.5 / 3.2 / 3.0
                // Teensy 3.6/3.5 specific DMA setup
                _dmaChannel->destination(*(volatile uint32_t*)&GPIOD_PDOR);
                _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_FTM0_CH7);
            #elif defined(ARDUINO_ARCH_ESP32)
                // ESP32 specific setup - using I2S parallel mode
                // ESP32 doesn't use the same DMA setup, handled elsewhere
                return;
            #elif defined(ARDUINO_ARCH_ESP8266)
                // ESP8266 doesn't support DMA for this purpose
                return;
            #elif defined(ARDUINO_ARCH_STM32)
                // STM32 specific DMA setup would go here
                // Currently not implemented
                return;
            #elif defined(ARDUINO_ARCH_RP2040)
                // RP2040 specific DMA setup would go here
                // Currently not implemented
                return;
            #elif defined(ARDUINO_SAM_DUE)
                // Due specific DMA setup would go here
                // Currently not implemented
                return;
            #elif defined(ARDUINO_AVR_MEGA2560)
                // Mega doesn't support DMA
                return;
            #else
                #warning "Platform not specifically supported for DMA"
                return;
            #endif

            _dmaChannel->disableOnCompletion();
            
            // Set up double buffering (common for all Teensy platforms)
            _dmaBuffer1 = (uint8_t*)malloc(DMA_BUFFER_SIZE);
            _dmaBuffer2 = (uint8_t*)malloc(DMA_BUFFER_SIZE);
            if (_dmaBuffer1 && _dmaBuffer2) {
                _currentDmaBuffer = _dmaBuffer1;
                _dmaBufferReady = true;
            }
            
            // Enable DMA interrupt for buffer switching
            _dmaChannel->attachInterrupt(dmaInterruptHandler);
            
            _dmaInitialized = true;
            _dmaActiveInstance = this;
        }
    }
}

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
    setupPins();  // Common pin setup for all platforms
    return initInterface();  // Platform-specific initialization
}

bool TFT_Interface_Parallel::initInterface() {
#if defined(ESP32)
    //TODO Implement ESP32 parallel interface
#elif defined(ESP8266)
    //TODO Implement ESP8266 parallel interface
#elif defined(ARDUINO_ARCH_RP2040)
    //TODO Implement RP2040 parallel interface
#elif defined(ARDUINO_SAM_DUE)
    //TODO Implement DUE parallel interface
#elif defined(__AVR__)
    //TODO Implement AVR parallel interface
#elif defined(CORE_TEENSY)
    #if defined(__IMXRT1062__)
        // Teensy 4.0/4.1
        // Initialize FlexIO for parallel interface
        _flexIO = new FlexIOHandler();
        if (!_flexIO) return false;

        // Configure FlexIO for optimal performance
        if (!_flexIO->begin()) return false;

        // Configure shifters for data pins
        uint8_t dataShifter = _flexIO->addShifter(FlexIOHandler::ShifterMode::Output);
        if (dataShifter == 0xFF) return false;

        // Configure timer for control signals
        uint8_t controlTimer = _flexIO->addTimer(FlexIOHandler::TimerMode::Trigger);
        if (controlTimer == 0xFF) return false;

        // Configure shifter for parallel data output
        _flexIO->setShifterConfig(dataShifter, 
            static_cast<uint32_t>(FlexIOHandler::ShifterConfig::Width8Bit | 
                                 FlexIOHandler::ShifterConfig::PinOutput));

        // Configure timer for control signals
        _flexIO->setTimerConfig(controlTimer,
            static_cast<uint32_t>(FlexIOHandler::TimerConfig::Trigger | 
                                 FlexIOHandler::TimerConfig::PinOutput));

        // Set up optimal timing parameters
        uint32_t clock = F_CPU_ACTUAL;  // Start with CPU clock
        uint32_t targetFreq = 80000000;  // Target 80MHz operation
        uint32_t divider = ((clock + targetFreq - 1) / targetFreq);
        if (divider < 1) divider = 1;

        _flexIO->setTimerConfig(
            _flexIOTimer,
            static_cast<uint32_t>(FlexIOHandler::TimerConfig::Trigger | 
                                 FlexIOHandler::TimerConfig::PinOutput));

        // Configure DMA for optimal performance
        if (_dmaChannel) {
            _dmaChannel->disable();

            // Use the correct register type for DMA destination
        }
    #elif defined(__MK66FX1M0__) || defined(__MK64FX512__)  // Teensy 3.6/3.5
        // Initialize GPIO port for data pins
        _basePort = digitalPinToPort(_dataPins[0]);
        _pinsOnSamePort = true;
        
        // Verify all pins are on the same port
        for (int i = 1; i < 8; i++) {
            if (digitalPinToPort(_dataPins[i]) != _basePort) {
                _pinsOnSamePort = false;
                return false;  // All pins must be on the same port
            }
        }
        
        _dataPort = reinterpret_cast<volatile uint32_t*>(portOutputRegister(_basePort));
        _dataPortSet = reinterpret_cast<volatile uint32_t*>(portSetRegister(_basePort));
        _dataPortClr = reinterpret_cast<volatile uint32_t*>(portClearRegister(_basePort));
        
        // Calculate data pin mask and shift
        _dataPinMask = 0;
        uint8_t minPin = 32, maxPin = 0;
        for (int i = 0; i < 8; i++) {
            uint8_t pin = _dataPins[i] % 32;  // Get pin number within the port
            _dataPinMask |= (1UL << pin);
            minPin = min(minPin, pin);
            maxPin = max(maxPin, pin);
        }
        _dataPinShift = minPin;

        // Configure FTM0 for DMA triggering with optimized settings
        FTM0_SC = 0;
        FTM0_CNT = 0;
        #if defined(__MK66FX1M0__)  // Teensy 3.6
            FTM0_MOD = F_BUS / 60000000;  // Target 60MHz operation for Teensy 3.6 (180MHz CPU)
        #elif defined(__MK64FX512__)  // Teensy 3.5
            FTM0_MOD = F_BUS / 40000000;  // Target 40MHz operation for Teensy 3.5 (120MHz CPU)
        #else
            FTM0_MOD = F_BUS / 20000000;  // Standard 20MHz for other boards
        #endif
        FTM0_SC = FTM_SC_CLKS(1) | FTM_SC_PS(0);  // Bus clock, no prescale
        FTM0_C7SC = FTM_CSC_MSA | FTM_CSC_DMA;

        // Set up DMA with improved error handling
        if (!_dmaChannel) {
            _dmaChannel = new DMAChannel();
        }
        if (!_dmaChannel) {
            return false;  // DMA channel allocation failed
        }

        _dmaChannel->disable();
        _dmaChannel->destination(*_dataPort);
        _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_FTM0_CH7);
        _dmaChannel->transferSize(1);  // 8-bit transfers
        _dmaChannel->transferCount(DMA_BUFFER_SIZE);
        _dmaChannel->disableOnCompletion();
        _dmaChannel->interruptAtCompletion();
        _dmaChannel->attachInterrupt(dmaInterruptHandler);

        // Allocate aligned DMA buffers with proper error handling
        if (_dmaBuffer1) free(_dmaBuffer1);
        if (_dmaBuffer2) free(_dmaBuffer2);
        
        _dmaBuffer1 = (uint8_t*)malloc(DMA_BUFFER_SIZE + 32);
        _dmaBuffer2 = (uint8_t*)malloc(DMA_BUFFER_SIZE + 32);
        
        if (!_dmaBuffer1 || !_dmaBuffer2) {
            if (_dmaBuffer1) free(_dmaBuffer1);
            if (_dmaBuffer2) free(_dmaBuffer2);
            _dmaBuffer1 = _dmaBuffer2 = nullptr;
            return false;
        }
        
        // Align buffers to 32-byte boundary
        _dmaBuffer1 = (uint8_t*)(((uintptr_t)_dmaBuffer1 + 31) & ~31);
        _dmaBuffer2 = (uint8_t*)(((uintptr_t)_dmaBuffer2 + 31) & ~31);
        _currentDmaBuffer = _dmaBuffer1;
        _dmaBufferReady = true;
        
        // Enable DMA optimizations for Teensy 3.5
        #if defined(__MK64FX512__)
            if (_dmaChannel) {
                _dmaChannel->TCD->CSR |= DMA_CSR_INTMAJOR;  // Enable interrupt at end of major loop
                _dmaChannel->TCD->ATTR |= DMA_ATTR_SSIZE(0) | DMA_ATTR_DSIZE(0);  // 8-bit transfers
                _dmaChannel->TCD->NBYTES_MLNO = 1;  // 1 byte per minor loop
                _dmaChannel->TCD->SLAST = 0;  // Don't adjust source address at end of major loop
                _dmaChannel->TCD->DLASTSGA = 0;  // Don't adjust dest address at end of major loop
                _dmaChannel->TCD->BITER_ELINKNO = DMA_BUFFER_SIZE;  // Major loop count
                _dmaChannel->TCD->CITER_ELINKNO = DMA_BUFFER_SIZE;  // Current iteration count
            }
        #endif
        
        _dmaInitialized = true;
        _dmaActiveInstance = this;
        return true;
    #endif
#endif

    if (supportsDMA()) {
        if (!_dmaInitialized) {
            // Allocate DMA channel
            _dmaChannel = new DMAChannel();
            if (_dmaChannel == nullptr) return false;

            _dmaChannel->disable();
            
            #if defined(__IMXRT1062__)
                // Teensy 4.x specific DMA setup
                _dmaChannel->destination(*(volatile uint32_t*)(&FLEXIO2_SHIFTBUF0 + _flexIOShifter));
                _dmaChannel->triggerAtHardwareEvent(45); // FlexIO2 DMA request channel
                FLEXIO2_SHIFTSDEN |= (1 << _flexIOShifter);  // Enable DMA request
            #elif defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__)  // Teensy 3.6/3.5 / 3.2 / 3.0
                // Teensy 3.6/3.5 specific DMA setup
                _dmaChannel->destination(*(volatile uint32_t*)&GPIOD_PDOR);
                _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_FTM0_CH7);
            #elif defined(ARDUINO_ARCH_ESP32)
                // ESP32 specific setup - using I2S parallel mode
                // ESP32 doesn't use the same DMA setup, handled elsewhere
                return true;
            #elif defined(ARDUINO_ARCH_ESP8266)
                // ESP8266 doesn't support DMA for this purpose
                return true;
            #elif defined(ARDUINO_ARCH_STM32)
                // STM32 specific DMA setup would go here
                // Currently not implemented
                return true;
            #elif defined(ARDUINO_ARCH_RP2040)
                // RP2040 specific DMA setup would go here
                // Currently not implemented
                return true;
            #elif defined(ARDUINO_SAM_DUE)
                // Due specific DMA setup would go here
                // Currently not implemented
                return true;
            #elif defined(ARDUINO_AVR_MEGA2560)
                // Mega doesn't support DMA
                return true;
            #else
                #warning "Platform not specifically supported for DMA"
                return true;
            #endif

            _dmaChannel->disableOnCompletion();
            
            // Set up double buffering (common for all Teensy platforms)
            _dmaBuffer1 = (uint8_t*)malloc(DMA_BUFFER_SIZE);
            _dmaBuffer2 = (uint8_t*)malloc(DMA_BUFFER_SIZE);
            if (_dmaBuffer1 && _dmaBuffer2) {
                _currentDmaBuffer = _dmaBuffer1;
                _dmaBufferReady = true;
            }
            
            // Enable DMA interrupt for buffer switching
            _dmaChannel->attachInterrupt(dmaInterruptHandler);
            
            _dmaInitialized = true;
            _dmaActiveInstance = this;
        }
    }
    return true;
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
    #elif defined(__MK66FX1M0__) || defined(__MK64FX512__)  // Teensy 3.6/3.5
        // Fast port reading for Teensy 3.5/3.6
        uint32_t port_value = *(volatile uint32_t*)portInputRegister(digitalPinToPort(_dataPins[0]));
        port_value &= _dataPinMask;
        port_value >>= _dataPinShift;
        data = (uint8_t)port_value;
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
        // Teensy 4.0/4.1 - Optimized FlexIO implementation
        if (_flexIO == nullptr) return;

        // Fast single byte write using FlexIO
        _flexIO->writeShifter(_flexIOShifter, data);
    #elif defined(__MK66FX1M0__) || defined(__MK64FX512__)  // Teensy 3.6/3.5
        // Fast port writing for Teensy 3.5/3.6
        uint32_t port_value = *(volatile uint32_t*)portOutputRegister(digitalPinToPort(_dataPins[0]));
        port_value &= ~_dataPinMask;  // Clear data pins
        port_value |= ((uint32_t)data << _dataPinShift) & _dataPinMask;  // Set new data
        *(volatile uint32_t*)portOutputRegister(digitalPinToPort(_dataPins[0])) = port_value;
        pulseWR();
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
        _flexIO->writeShifter(_flexIOShifter, data >> 8);
        _flexIO->writeShifter(_flexIOShifter, data & 0xFF);
    #elif defined(__MK66FX1M0__) || defined(__MK64FX512__)  // Teensy 3.6/3.5
        // Fast port writing for Teensy 3.5/3.6
        uint32_t port_value = *(volatile uint32_t*)portOutputRegister(digitalPinToPort(_dataPins[0]));
        port_value &= ~_dataPinMask;  // Clear data pins
        port_value |= ((uint32_t)(data >> 8) << _dataPinShift) & _dataPinMask;  // Set new data
        *(volatile uint32_t*)portOutputRegister(digitalPinToPort(_dataPins[0])) = port_value;
        pulseWR();
        port_value &= ~_dataPinMask;  // Clear data pins
        port_value |= ((uint32_t)(data & 0xFF) << _dataPinShift) & _dataPinMask;  // Set new data
        *(volatile uint32_t*)portOutputRegister(digitalPinToPort(_dataPins[0])) = port_value;
        pulseWR();
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

    #if defined(CORE_TEENSY) && defined(__IMXRT1062__)
    // Teensy 4.0/4.1 - Use DMA for large transfers
    if (_flexIO != nullptr && _dmaInitialized && len > 32) {
        // Configure FlexIO for 8-bit parallel transfer
        _flexIO->setShifterConfig(_flexIOShifter, FLEXIO_SHIFTCFG_PWIDTH(7));
        // Enable DMA for shifter
        _flexIO->writeShifter(_flexIOShifter, 0); // Clear shifter
        // Direct hardware register access for Teensy 4.x
        volatile uint32_t* shifterAddr = &IMXRT_FLEXIO2_S.SHIFTBUF[_flexIOShifter];
        _dmaChannel->destinationBuffer((volatile uint8_t*)shifterAddr, len);
        _dmaChannel->sourceBuffer(data, len);
        _dmaChannel->enable();
        return;
    }
    #endif

    // Default implementation - write bytes one at a time
    while (len--) {
        write8(*data++);
    }
}

void TFT_Interface_Parallel::setupPins() {
    #if defined(__MK66FX1M0__) || defined(__MK64FX512__)  // Teensy 3.6/3.5
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
    #endif
}

void TFT_Interface_Parallel::setupParallelBus() {
    #if defined(CORE_TEENSY)
        #if defined(__IMXRT1062__)  // Teensy 4.0
            // Configure FlexIO for parallel interface
            
            // Initialize FlexIO
            _flexIO = new FlexIOHandler();
            if (!_flexIO->begin()) {
                return; // Failed to initialize FlexIO
            }
            
            // Initialize data pins using FlexIO
            for (uint8_t i = 0; i < _config.parallel.bus_width; i++) {
                if (_config.parallel.data_pins[i] != -1) {
                    _dataPins[i] = _config.parallel.data_pins[i];
                    pinMode(_dataPins[i], OUTPUT);
                }
            }
            
            // Configure FlexIO shifter for parallel output
            _flexIOShifter = _flexIO->addShifter(FlexIOHandler::ShifterMode::Output);
            if (_flexIOShifter == 0xFF) {
                return; // Failed to add shifter
            }
            
            // Configure FlexIO timer for write signal generation
            _flexIOTimer = _flexIO->addTimer(FlexIOHandler::TimerMode::Trigger);
            if (_flexIOTimer == 0xFF) {
                return; // Failed to add timer
            }
            
            // Configure shifter for parallel data output
            _flexIO->setShifterConfig(_flexIOShifter, 
                static_cast<uint32_t>(FlexIOHandler::ShifterConfig::Width8Bit | 
                                    FlexIOHandler::ShifterConfig::TimerTrig | 
                                    FlexIOHandler::ShifterConfig::PinOutput));
            
            // Configure timer for write signal generation
            _flexIO->setTimerConfig(_flexIOTimer,
                static_cast<uint32_t>(FlexIOHandler::TimerConfig::Trigger | 
                                    FlexIOHandler::TimerConfig::PinOutput));
            
            // Configure control pins
            if (_wrPin >= 0) {
                pinMode(_wrPin, OUTPUT);
                digitalWriteFast(_wrPin, HIGH);
            }
            if (_rdPin >= 0) {
                pinMode(_rdPin, OUTPUT);
                digitalWriteFast(_rdPin, HIGH);
            }
            if (_dcPin >= 0) {
                pinMode(_dcPin, OUTPUT);
            }
            if (_csPin >= 0) {
                pinMode(_csPin, OUTPUT);
                digitalWriteFast(_csPin, HIGH);
            }
        #elif defined(__MK66FX1M0__) || defined(__MK64FX512__)  // Teensy 3.6/3.5
            // Configure data pins using direct port access
            _dataMask = 0;
            _dataShift = 0;
            uint8_t firstPin = 255;

            // Configure all data pins for maximum speed
            for (uint8_t i = 0; i < _config.parallel.bus_width; i++) {
                if (_config.parallel.data_pins[i] != -1) {
                    uint8_t pin = _config.parallel.data_pins[i];
                    *portConfigRegister(pin) = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
                    *portModeRegister(pin) |= digitalPinToBitMask(pin);  // Set to output
                    
                    // Track the data mask and first pin for port calculations
                    _dataMask |= digitalPinToBitMask(pin);
                    if (firstPin == 255) firstPin = pin;
                }
            }

            // Set up port manipulation registers based on the first pin
            if (firstPin != 255) {
                _dataPort = reinterpret_cast<volatile uint32_t*>(portOutputRegister(digitalPinToPort(firstPin)));
                _dataPortSet = reinterpret_cast<volatile uint32_t*>(portSetRegister(digitalPinToPort(firstPin)));
                _dataPortClr = reinterpret_cast<volatile uint32_t*>(portClearRegister(digitalPinToPort(firstPin)));
                
                // Calculate data shift based on pin positions
                uint32_t mask = _dataMask;
                while (!(mask & 1)) {
                    _dataShift++;
                    mask >>= 1;
                }
            }

            // Configure write pin for maximum speed
            if (_wrPin >= 0) {
                *portConfigRegister(_wrPin) = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
                *portModeRegister(_wrPin) |= digitalPinToBitMask(_wrPin);
                
                // Set up write port manipulation registers
                _wrPort = reinterpret_cast<volatile uint32_t*>(portOutputRegister(digitalPinToPort(_wrPin)));
                _wrPortSet = reinterpret_cast<volatile uint32_t*>(portSetRegister(digitalPinToPort(_wrPin)));
                _wrPortClear = reinterpret_cast<volatile uint32_t*>(portClearRegister(digitalPinToPort(_wrPin)));
                _wrPinMask = digitalPinToBitMask(_wrPin);
            }

            // Configure read pin if used
            if (_rdPin >= 0) {
                *portConfigRegister(_rdPin) = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
                *portModeRegister(_rdPin) |= digitalPinToBitMask(_rdPin);
                
                // Set up read port manipulation registers
                _rdPort = reinterpret_cast<volatile uint32_t*>(portOutputRegister(digitalPinToPort(_rdPin)));
                _rdPortSet = reinterpret_cast<volatile uint32_t*>(portSetRegister(digitalPinToPort(_rdPin)));
                _rdPortClear = reinterpret_cast<volatile uint32_t*>(portClearRegister(digitalPinToPort(_rdPin)));
                _rdPinMask = digitalPinToBitMask(_rdPin);
                
                digitalWriteFast(_rdPin, HIGH);
            }

            // Configure DC pin
            if (_dcPin >= 0) {
                *portConfigRegister(_dcPin) = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
                *portModeRegister(_dcPin) |= digitalPinToBitMask(_dcPin);
                
                // Set up DC port manipulation registers
                _dcPort = reinterpret_cast<volatile uint32_t*>(portOutputRegister(digitalPinToPort(_dcPin)));
                _dcPortSet = reinterpret_cast<volatile uint32_t*>(portSetRegister(digitalPinToPort(_dcPin)));
                _dcPortClear = reinterpret_cast<volatile uint32_t*>(portClearRegister(digitalPinToPort(_dcPin)));
                _dcPinMask = digitalPinToBitMask(_dcPin);
            }

            // Configure CS pin
            if (_csPin >= 0) {
                *portConfigRegister(_csPin) = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
                *portModeRegister(_csPin) |= digitalPinToBitMask(_csPin);
                
                // Set up CS port manipulation registers
                _csPort = reinterpret_cast<volatile uint32_t*>(portOutputRegister(digitalPinToPort(_csPin)));
                _csPortSet = reinterpret_cast<volatile uint32_t*>(portSetRegister(digitalPinToPort(_csPin)));
                _csPortClear = reinterpret_cast<volatile uint32_t*>(portClearRegister(digitalPinToPort(_csPin)));
                _csPinMask = digitalPinToBitMask(_csPin);
                
                digitalWriteFast(_csPin, HIGH);
            }
        #elif defined(__MK20DX256__) // Teensy 3.2
            //TODO add code for Teensy 3.2
        #elif defined(__MK20DX128__) // Teensy 3.0
            //TODO add code for Teensy 3.0
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

void TFT_Interface_Parallel::cleanupDMA() {
#if defined(CORE_TEENSY) 
    #if defined(__IMXRT1062__)
        if (_flexIO) {
            delete _flexIO;
            _flexIO = nullptr;
        }
    #elif defined(__IMXRT1062__) || defined(__MK66FX1M0__) || defined(__MK64FX512__)  // Teensy 4.x or 3.6 or 3.5
        if (_dmaChannel) {
            _dmaChannel->disable();
            delete _dmaChannel;
            _dmaChannel = nullptr;
        }
        if (_dmaBuffer1) {
            free(_dmaBuffer1);
            _dmaBuffer1 = nullptr;
        }
        if (_dmaBuffer2) {
            free(_dmaBuffer2);
            _dmaBuffer2 = nullptr;
        }
        _currentDmaBuffer = nullptr;
        _dmaBufferReady = false;
        
        // Disable FTM0 DMA request
        FTM0_C7SC &= ~FTM_CSC_DMA;
        
        // Reset port configurations
        if (_pinsOnSamePort && _dataPort) {
            *_dataPort &= ~_dataPinMask;  // Clear all data pins
        }
        
        // Reset member pointers
        _dataPort = nullptr;
        _dataPortSet = nullptr;
        _dataPortClr = nullptr;
        _wrPort = nullptr;
        _wrPortSet = nullptr;
        _wrPortClear = nullptr;
    #endif
#endif
    _dmaInitialized = false;
}

void TFT_Interface_Parallel::waitDMAComplete() {
#if defined(CORE_TEENSY)
    #if defined(__IMXRT1062__) || defined(__MK66FX1M0__) || defined(__MK64FX512__)  // Teensy 4.x or 3.6 or 3.5
        if (_dmaChannel) {
            while (!_dmaChannel->complete()) {
                yield();
            }
        }
    #endif
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