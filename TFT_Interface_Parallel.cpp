#include "TFT_Interface_Parallel.h"

// Platform-specific includes
#if defined(ARDUINO_ARCH_RP2040)
    #include "hardware/pio.h"
    #include "hardware/dma.h"
    #include "hardware/clocks.h"
#endif

#if defined(CORE_TEENSY) && defined(__IMXRT1062__)
    #include <imxrt.h>
    #include <FlexIO_t4.h>
#endif

namespace TFT_Runtime {

// PIO program for RP2040 parallel output
#if defined(ARDUINO_ARCH_RP2040)
static const uint16_t parallel_program_instructions[] = {
    0x6008, //  0: out    pins, 8
    0x0001, //  1: jmp    1            ; Loop forever
};

static const struct pio_program parallel_program = {
    .instructions = parallel_program_instructions,
    .length = 2,
    .origin = -1,
};
#endif

TFT_Interface_Parallel::TFT_Interface_Parallel(const Config& config)
    : TFT_Interface(config)
    , _csPin(config.parallel.cs_pin)
    , _wrPin(config.parallel.wr_pin)
    , _rdPin(config.parallel.rd_pin)
    , _rstPin(config.rst_pin)
    , _latchPin(config.parallel.latch_pin)
    , _is16Bit(config.parallel.is_16bit)
    , _useLatch(config.parallel.use_latch)
    , _writeDelay(config.parallel.write_delay)
    , _dmaEnabled(false)
    , _buffering(false)
    , _buffer(nullptr)
    , _bufferIndex(0)
{
    // Initialize data pins from config
    for (int i = 0; i < (_is16Bit ? 16 : 8); i++) {
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
        _flexIO = nullptr;
        _flexIOShifter = 0;
        _flexIOTimer = 0;
        _dmaChannel = nullptr;
        _dmaSettings = nullptr;
        _dmaBuf = nullptr;
        _dmaBufSize = 0;
    #endif
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
        setupPins();  // Default pin setup for other platforms
    #endif

    if (supportsBuffering() && !_buffer) {
        _buffer = new uint8_t[BUFFER_SIZE];
    }

    return success;
}

// Platform-specific implementations
#if defined(ESP32)
bool TFT_Interface_Parallel::initESP32() {
    setupPins();

    if (_is16Bit) {
        // Setup lower 8 bits
        _dataPortIdx[0] = digitalPinToPort(_dataPins[0]);
        _dataPortMask[0] = 0;
        for (int i = 0; i < 8; i++) {
            _dataPortMask[0] |= (1UL << digitalPinToBitMask(_dataPins[i]));
        }
        
        // Setup upper 8 bits
        _dataPortIdx[1] = digitalPinToPort(_dataPins[8]);
        _dataPortMask[1] = 0;
        for (int i = 8; i < 16; i++) {
            _dataPortMask[1] |= (1UL << digitalPinToBitMask(_dataPins[i]));
        }
        
        _dataPortSet[0] = &GPIO.out_w1ts;
        _dataPortClr[0] = &GPIO.out_w1tc;
        _dataPortSet[1] = &GPIO.out1_w1ts;
        _dataPortClr[1] = &GPIO.out1_w1tc;
    } else {
        _dataPortIdx[0] = digitalPinToPort(_dataPins[0]);
        _dataPortMask[0] = 0;
        for (int i = 0; i < 8; i++) {
            _dataPortMask[0] |= (1UL << digitalPinToBitMask(_dataPins[i]));
        }
        _dataPortSet[0] = &GPIO.out_w1ts;
        _dataPortClr[0] = &GPIO.out_w1tc;
    }
    
    _wrPortSet = &GPIO.out_w1ts;
    _wrPortClr = &GPIO.out_w1tc;
    _rdPortSet = &GPIO.out_w1ts;
    _rdPortClr = &GPIO.out_w1tc;
    
    if (_useLatch) {
        _latchPortSet = &GPIO.out_w1ts;
        _latchPortClr = &GPIO.out_w1tc;
    }

    if (supportsDMA()) {
        initESP32DMA();
    }

    return true;
}

void TFT_Interface_Parallel::initESP32DMA() {
    if (_dmaInitialized) return;

    _dmaChannel = 1;  // Use DMA channel 1
    _dmaBufSize = 4096;
    _dmaBuf = heap_caps_malloc(_dmaBufSize, MALLOC_CAP_DMA);
    
    if (_dmaBuf) {
        _dmadesc = (lldesc_t*)heap_caps_malloc(sizeof(lldesc_t), MALLOC_CAP_DMA);
        if (_dmadesc) {
            _dmaInitialized = true;
            _dmaEnabled = true;
        }
    }
}

void TFT_Interface_Parallel::writeESP32_8(uint8_t data) {
    *_dataPortClr[0] = _dataPortMask[0];
    *_dataPortSet[0] = ((uint32_t)data << (_dataPins[0] % 32)) & _dataPortMask[0];
    pulseWR();
}

void TFT_Interface_Parallel::writeESP32_16(uint16_t data) {
    *_dataPortClr[0] = _dataPortMask[0];
    *_dataPortClr[1] = _dataPortMask[1];
    
    *_dataPortSet[0] = ((uint32_t)(data & 0xFF) << (_dataPins[0] % 32)) & _dataPortMask[0];
    *_dataPortSet[1] = ((uint32_t)(data >> 8) << (_dataPins[8] % 32)) & _dataPortMask[1];
    
    if (_useLatch) pulseLatch();
    pulseWR();
}

void TFT_Interface_Parallel::writeESP32DMA(const uint8_t* data, size_t len) {
    if (!_dmaInitialized || !data || len == 0) return;

    size_t remaining = len;
    const uint8_t* buf = data;

    while (remaining > 0) {
        size_t chunk = min(remaining, _dmaBufSize);
        memcpy(_dmaBuf, buf, chunk);

        _dmadesc[0].size = chunk;
        _dmadesc[0].length = chunk;
        _dmadesc[0].buf = _dmaBuf;
        _dmadesc[0].eof = 1;
        _dmadesc[0].empty = (uint32_t)&_dmadesc[0];
        
        gdma_start(_dmaChannel, (uint32_t)_dmadesc);
        
        remaining -= chunk;
        buf += chunk;
    }
}

#endif

#if defined(ESP8266)
bool TFT_Interface_Parallel::initESP8266() {
    setupPins();
    
    _dataPortMask = 0;
    _wrBit = 1 << _wrPin;
    _rdBit = 1 << _rdPin;
    
    for (int i = 0; i < (_is16Bit ? 16 : 8); i++) {
        if (_dataPins[i] >= 0) {
            _dataBits[i] = 1 << _dataPins[i];
            _dataPortMask |= _dataBits[i];
        }
    }
    
    return true;
}

void TFT_Interface_Parallel::writeESP8266_8(uint8_t data) {
    uint32_t gpio = GPI;
    gpio &= ~_dataPortMask;
    
    for (int i = 0; i < 8; i++) {
        if (_dataPins[i] >= 0 && (data & (1 << i))) {
            gpio |= _dataBits[i];
        }
    }
    
    GPIO_REG_WRITE(GPIO_OUT_REG, gpio);
    GPIO_REG_WRITE(GPIO_OUT_REG, gpio | _wrBit);
}

void TFT_Interface_Parallel::writeESP8266_16(uint16_t data) {
    uint32_t gpio = GPI;
    gpio &= ~_dataPortMask;
    
    for (int i = 0; i < 16; i++) {
        if (_dataPins[i] >= 0 && (data & (1 << i))) {
            gpio |= _dataBits[i];
        }
    }
    
    GPIO_REG_WRITE(GPIO_OUT_REG, gpio);
    if (_useLatch) pulseLatch();
    GPIO_REG_WRITE(GPIO_OUT_REG, gpio | _wrBit);
}

#endif

#if defined(ARDUINO_ARCH_RP2040)
bool TFT_Interface_Parallel::initRP2040() {
    setupPins();
    
    // Initialize PIO for parallel output
    _pio = pio0;
    _sm = pio_claim_unused_sm(_pio, true);
    
    if (_sm == -1) {
        _pio = pio1;
        _sm = pio_claim_unused_sm(_pio, true);
        if (_sm == -1) return false;
    }
    
    _program_offset = pio_add_program(_pio, &parallel_program);
    
    float div = clock_get_hz(clk_sys) / 100000000.0f;  // 100MHz
    pio_sm_set_consecutive_pindirs(_pio, _sm, _dataPins[0], _is16Bit ? 16 : 8, true);
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_out_pins(&c, _dataPins[0], _is16Bit ? 16 : 8);
    sm_config_set_clkdiv(&c, div);
    
    pio_sm_init(_pio, _sm, _program_offset, &c);
    pio_sm_set_enabled(_pio, _sm, true);
    
    // Initialize DMA
    _dma_chan = dma_claim_unused_channel(true);
    if (_dma_chan != -1) {
        dma_channel_config c = dma_channel_get_default_config(_dma_chan);
        channel_config_set_transfer_data_size(&c, _is16Bit ? DMA_SIZE_16 : DMA_SIZE_8);
        channel_config_set_dreq(&c, pio_get_dreq(_pio, _sm, true));
        dma_channel_configure(_dma_chan, &c,
            &_pio->txf[_sm],  // Write to PIO TX FIFO
            NULL,             // Don't provide a read buffer yet
            0,               // Don't provide a count yet
            false           // Don't start yet
        );
    }
    
    _pioInitialized = true;
    return true;
}

void TFT_Interface_Parallel::writeRP2040_8(uint8_t data) {
    if (_pioInitialized) {
        pio_sm_put_blocking(_pio, _sm, data);
        pulseWR();
    } else {
        // Fallback to bit-banging
        for (int i = 0; i < 8; i++) {
            if (_dataPins[i] >= 0) {
                gpio_put(_dataPins[i], (data >> i) & 0x01);
            }
        }
        pulseWR();
    }
}

void TFT_Interface_Parallel::writeRP2040_16(uint16_t data) {
    if (_pioInitialized) {
        pio_sm_put_blocking(_pio, _sm, data);
        if (_useLatch) pulseLatch();
        pulseWR();
    } else {
        // Fallback to bit-banging
        for (int i = 0; i < 16; i++) {
            if (_dataPins[i] >= 0) {
                gpio_put(_dataPins[i], (data >> i) & 0x01);
            }
        }
        if (_useLatch) pulseLatch();
        pulseWR();
    }
}

void TFT_Interface_Parallel::writeRP2040DMA(const uint8_t* data, size_t len) {
    if (!_pioInitialized || _dma_chan == -1) return;
    
    dma_channel_set_read_addr(_dma_chan, data, false);
    dma_channel_set_trans_count(_dma_chan, len, true);
    
    dma_channel_wait_for_finish_blocking(_dma_chan);
}

#endif

#if defined(ARDUINO_SAM_DUE)
bool TFT_Interface_Parallel::initSAMDUE() {
    setupPins();
    
    // Configure data pins for direct port access
    for (int i = 0; i < (_is16Bit ? 16 : 8); i++) {
        if (_dataPins[i] >= 0) {
            _dataPio[i/8] = g_APinDescription[_dataPins[i]].pPort;
            _dataPinMask[i/8] |= g_APinDescription[_dataPins[i]].ulPin;
        }
    }
    
    // Configure control pins
    if (_wrPin >= 0) {
        _wrPio = g_APinDescription[_wrPin].pPort;
        _wrPinMask = g_APinDescription[_wrPin].ulPin;
    }
    
    if (_rdPin >= 0) {
        _rdPio = g_APinDescription[_rdPin].pPort;
        _rdPinMask = g_APinDescription[_rdPin].ulPin;
    }
    
    return true;
}

void TFT_Interface_Parallel::writeSAMDUE_8(uint8_t data) {
    // Clear all data pins
    _dataPio[0]->PIO_CODR = _dataPinMask[0];
    
    // Set data pins according to value
    uint32_t value = 0;
    for (int i = 0; i < 8; i++) {
        if (data & (1 << i)) {
            value |= g_APinDescription[_dataPins[i]].ulPin;
        }
    }
    _dataPio[0]->PIO_SODR = value;
    
    pulseWR();
}

void TFT_Interface_Parallel::writeSAMDUE_16(uint16_t data) {
    // Clear all data pins
    _dataPio[0]->PIO_CODR = _dataPinMask[0];
    if (_is16Bit) _dataPio[1]->PIO_CODR = _dataPinMask[1];
    
    // Set data pins according to value
    uint32_t value_low = 0, value_high = 0;
    for (int i = 0; i < 8; i++) {
        if (data & (1 << i)) {
            value_low |= g_APinDescription[_dataPins[i]].ulPin;
        }
        if (_is16Bit && (data & (1 << (i + 8)))) {
            value_high |= g_APinDescription[_dataPins[i + 8]].ulPin;
        }
    }
    
    _dataPio[0]->PIO_SODR = value_low;
    if (_is16Bit) _dataPio[1]->PIO_SODR = value_high;
    
    if (_useLatch) pulseLatch();
    pulseWR();
}

#endif

#if defined(__AVR__)
bool TFT_Interface_Parallel::initAVR() {
    setupPins();
    
    // Group pins by port for faster access
    for (int i = 0; i < (_is16Bit ? 16 : 8); i++) {
        if (_dataPins[i] >= 0) {
            _dataPort[i/8] = digitalPinToPort(_dataPins[i]);
            _dataDdr[i/8] = portModeRegister(_dataPort[i/8]);
            _dataPin[i/8] = portOutputRegister(_dataPort[i/8]);
        }
    }
    
    if (_wrPin >= 0) {
        _wrPort = digitalPinToPort(_wrPin);
        _wrDdr = portModeRegister(_wrPort);
        _wrPin = portOutputRegister(_wrPort);
    }
    
    if (_rdPin >= 0) {
        _rdPort = digitalPinToPort(_rdPin);
        _rdDdr = portModeRegister(_rdPort);
        _rdPin = portOutputRegister(_rdPort);
    }
    
    return true;
}

void TFT_Interface_Parallel::writeAVR_8(uint8_t data) {
    *_dataPin[0] = data;
    pulseWR();
}

void TFT_Interface_Parallel::writeAVR_16(uint16_t data) {
    *_dataPin[0] = data & 0xFF;
    if (_is16Bit) *_dataPin[1] = data >> 8;
    
    if (_useLatch) pulseLatch();
    pulseWR();
}
#endif


#if defined(__IMXRT1062__)
#include <imxrt.h>
#include <FlexIO_t4.h>
#include <DMAChannel.h>

// DMA request sources for IMXRT1062 (Teensy 4.x)
#ifndef DMAMUX_SOURCE_FLEXIO2_REQUEST
#define DMAMUX_SOURCE_FLEXIO2_REQUEST 27
#endif
#ifndef DMAMUX_SOURCE_GPIO
#define DMAMUX_SOURCE_GPIO 89
#endif
#endif

bool TFT_Interface_Parallel::initTeensy() {
#if defined(__IMXRT1062__) // Teensy 4.0/4.1
    // Setup GPIO ports for fast access
    for (int i = 0; i < (_is16Bit ? 16 : 8); i++) {
        int portNum = _dataPins[i] / 32;
        _dataPorts[i/8] = portNum == 0 ? (IMXRT_GPIO_t*)&GPIO1_DR : 
                         (portNum == 1 ? (IMXRT_GPIO_t*)&GPIO2_DR : (IMXRT_GPIO_t*)&GPIO3_DR);
        _dataPinMasks[i/8] |= 1 << (_dataPins[i] % 32);
    }
    
    // Setup WR and RD pins
    int wrPortNum = _wrPin / 32;
    _wrPort = wrPortNum == 0 ? (IMXRT_GPIO_t*)&GPIO1_DR : 
              (wrPortNum == 1 ? (IMXRT_GPIO_t*)&GPIO2_DR : (IMXRT_GPIO_t*)&GPIO3_DR);
    _wrPinMask = 
    1 << (_wrPin % 32);
    
    int rdPortNum = _rdPin / 32;
    _rdPort = rdPortNum == 0 ? (IMXRT_GPIO_t*)&GPIO1_DR : 
              (rdPortNum == 1 ? (IMXRT_GPIO_t*)&GPIO2_DR : (IMXRT_GPIO_t*)&GPIO3_DR);
    _rdPinMask = 
    1 << (_rdPin % 32);
    
    // Initialize FlexIO if available
    if (_useFlexIO) initTeensyFlexIO();
    
    // Initialize DMA
    initTeensyDMA();
    
#elif defined(__MK66FX1M0__) || defined(__MK64FX512__)
    // Setup GPIO ports for fast access
    for (int i = 0; i < (_is16Bit ? 16 : 8); i++) {
        _dataPorts[i/8] = &GPIOD_PDOR + (_dataPins[i] / 32);
        _dataPinMasks[i/8] |= 1 << (_dataPins[i] % 32);
    }
    
    _wrPort = &GPIOD_PDOR + (_wrPin / 32);
    _wrPinMask = 1 << (_wrPin % 32);
    
    _rdPort = &GPIOD_PDOR + (_rdPin / 32);
    _rdPinMask = 1 << (_rdPin % 32);
    
    // Initialize DMA
    initTeensyDMA();
    
#elif defined(__MK20DX256__)
    // Setup GPIO ports for fast access
    for (int i = 0; i < (_is16Bit ? 16 : 8); i++) {
        _dataPorts[i/8] = &GPIOD_PDOR + (_dataPins[i] / 32);
        _dataPinMasks[i/8] |= 1 << (_dataPins[i] % 32);
    }
    
    _wrPort = &GPIOD_PDOR + (_wrPin / 32);
    _wrPinMask = 1 << (_wrPin % 32);
    
    _rdPort = &GPIOD_PDOR + (_rdPin / 32);
    _rdPinMask = 1 << (_rdPin % 32);
#endif
    
    return true;
}

void TFT_Interface_Parallel::initTeensyFlexIO() {
#if defined(__IMXRT1062__) // Teensy 4.0/4.1
    // Configure FlexIO for parallel output
    _flexIO = (IMXRT_FLEXIO_t*)&IMXRT_FLEXIO1;
    _flexIOShifter = 0;
    _flexIOTimer = 0;
    
    // Configure shifter for parallel output
    _flexIO->SHIFTCTL[_flexIOShifter] = 
        FLEXIO_SHIFTCTL_TIMSEL(_flexIOTimer) |
        FLEXIO_SHIFTCTL_PINCFG(3) |         // Output
        FLEXIO_SHIFTCTL_PINSEL(0) |         // Start from pin 0
        FLEXIO_SHIFTCTL_SMOD(2);            // Parallel output
#endif
}

void TFT_Interface_Parallel::initTeensyDMA() {
    _dmaChannel = new DMAChannel();
    _dmaSettings = new DMASetting();
    
    // Allocate DMA buffer
    _dmaBufSize = 1024; // Adjust size as needed
    _dmaBuf = (uint32_t*)malloc(_dmaBufSize * sizeof(uint32_t));
    
    if (_useFlexIO) {
        // Configure DMA for FlexIO
        _dmaChannel->destination(*(volatile uint32_t*)&_flexIO->SHIFTBUF[_flexIOShifter]);
        _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_FLEXIO2_REQUEST);
    } else {
        // Configure DMA for direct GPIO
        _dmaChannel->destination(*(volatile uint32_t*)&_dataPorts[0]->DR);
        _dmaChannel->triggerAtHardwareEvent(DMAMUX_SOURCE_GPIO);
    }
    
    _dmaChannel->enable();
}

#if defined(CORE_TEENSY) && (defined(__MK66FX1M0__) || defined(__MK64FX512__))
void TFT_Interface_Parallel::writeTeensy3_8(uint8_t data) {
    *_dataPorts[0] = (*_dataPorts[0] & ~_dataPinMasks[0]) | 
                     ((uint32_t)data << (_dataPins[0] % 32) & _dataPinMasks[0]);
    
    if (_useLatch) pulseLatch();
    pulseWR();
}

void TFT_Interface_Parallel::writeTeensy3_16(uint16_t data) {
    *_dataPorts[0] = (*_dataPorts[0] & ~_dataPinMasks[0]) | 
                     ((uint32_t)(data & 0xFF) << (_dataPins[0] % 32) & _dataPinMasks[0]);
    *_dataPorts[1] = (*_dataPorts[1] & ~_dataPinMasks[1]) | 
                     ((uint32_t)(data >> 8) << (_dataPins[8] % 32) & _dataPinMasks[1]);
    
    if (_useLatch) pulseLatch();
    pulseWR();
}

void TFT_Interface_Parallel::writeTeensy3DMA(const uint8_t* data, size_t len) {
    if (!_dmaChannel || !_dmaBuf || len == 0) return;

    size_t remaining = len;
    const uint8_t* src = data;
    while (remaining > 0) {
        size_t chunk = min(remaining, _dmaBufSize);
        memcpy(_dmaBuf, src, chunk);
        
        _dmaChannel->sourceBuffer(_dmaBuf, chunk);
        _dmaChannel->destination(*_dataPorts[0]);
        _dmaChannel->transferSize(1);
        _dmaChannel->transferCount(chunk);
        _dmaChannel->enable();
        
        while (_dmaChannel->complete() == false) {
            yield();
        }
        _dmaChannel->clearComplete();
        _dmaChannel->disable();
        
        remaining -= chunk;
        src += chunk;
    }
}
#endif

#if defined(CORE_TEENSY) && defined(__MK20DX256__)
void TFT_Interface_Parallel::writeTeensy32_8(uint8_t data) {
    *_dataPorts[0] = (*_dataPorts[0] & ~_dataPinMasks[0]) | 
                     ((uint32_t)data << (_dataPins[0] % 32) & _dataPinMasks[0]);
    
    if (_useLatch) pulseLatch();
    pulseWR();
}

void TFT_Interface_Parallel::writeTeensy32_16(uint16_t data) {
    *_dataPorts[0] = (*_dataPorts[0] & ~_dataPinMasks[0]) | 
                     ((uint32_t)(data & 0xFF) << (_dataPins[0] % 32) & _dataPinMasks[0]);
    *_dataPorts[1] = (*_dataPorts[1] & ~_dataPinMasks[1]) | 
                     ((uint32_t)((data >> 8) & 0xFF) << (_dataPins[8] % 32) & _dataPinMasks[1]);
    
    if (_useLatch) pulseLatch();
    pulseWR();
}
#endif

#if defined(CORE_TEENSY) && defined(__IMXRT1062__)
void TFT_Interface_Parallel::writeTeensy4_8(uint8_t data) {
    _dataPorts[0]->DR = (_dataPorts[0]->DR & ~_dataPinMasks[0]) | 
                        ((uint32_t)data << (_dataPins[0] % 32) & _dataPinMasks[0]);
    
    if (_useLatch) pulseLatch();
    pulseWR();
}

void TFT_Interface_Parallel::writeTeensy4_16(uint16_t data) {
    _dataPorts[0]->DR = (_dataPorts[0]->DR & ~_dataPinMasks[0]) | 
                        ((uint32_t)(data & 0xFF) << (_dataPins[0] % 32) & _dataPinMasks[0]);
    _dataPorts[1]->DR = (_dataPorts[1]->DR & ~_dataPinMasks[1]) | 
                        ((uint32_t)(data >> 8) << (_dataPins[8] % 32) & _dataPinMasks[1]);
    
    if (_useLatch) pulseLatch();
    pulseWR();
}

void TFT_Interface_Parallel::writeTeensy4FlexIO(const uint8_t* data, size_t len) {
    while (len--) {
        while (!(_flexIO->SHIFTSTAT & (1 << _flexIOShifter))) ; // Wait for shifter
        _flexIO->SHIFTBUF[_flexIOShifter] = *data++;
    }
}

void TFT_Interface_Parallel::writeTeensy4DMA(const uint8_t* data, size_t len) {
    size_t remaining = len;
    const uint8_t* src = data;
    
    while (remaining > 0) {
        size_t chunk = min(remaining, _dmaBufSize);
        memcpy(_dmaBuf, src, chunk);
        
        _dmaChannel->transferCount(chunk);
        _dmaChannel->sourceBuffer(_dmaBuf, chunk * sizeof(uint32_t));
        _dmaChannel->enable();
        
        while (!_dmaChannel->complete()) ; // Wait for transfer
        
        remaining -= chunk;
        src += chunk;
    }
}
#endif

void TFT_Interface_Parallel::writeParallelBus8(uint8_t data) {
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
        #if defined(__IMXRT1062__)
            writeTeensy4_8(data);
        #elif defined(__MK66FX1M0__) || defined(__MK64FX512__)
            writeTeensy3_8(data);
        #elif defined(__MK20DX256__)
            writeTeensy32_8(data);
        #endif
    #else
        // Generic implementation
        for (int i = 0; i < 8; i++) {
            if (_dataPins[i] >= 0) {
                digitalWrite(_dataPins[i], (data >> i) & 0x01);
            }
        }
        pulseWR();
    #endif
    delayWrite();
}

void TFT_Interface_Parallel::writeParallelBus16(uint16_t data) {
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
        #if defined(__IMXRT1062__)
            writeTeensy4_16(data);
        #elif defined(__MK66FX1M0__) || defined(__MK64FX512__)
            writeTeensy3_16(data);
        #elif defined(__MK20DX256__)
            writeTeensy32_16(data);
        #endif
    #else
        // Generic implementation for 16-bit mode
        for (int i = 0; i < 16; i++) {
            if (_dataPins[i] >= 0) {
                digitalWrite(_dataPins[i], (data >> i) & 0x01);
            }
        }
        if (_useLatch) pulseLatch();
        pulseWR();
    #endif
    delayWrite();
}

bool TFT_Interface_Parallel::supportsDMA() const {
    #if defined(ESP32) || defined(ARDUINO_ARCH_RP2040) || defined(CORE_TEENSY)
        return true;
    #else
        return false;
    #endif
}

bool TFT_Interface_Parallel::startDMAWrite(const uint8_t* data, size_t len) {
    #if defined(ESP32)
        writeESP32DMA(data, len);
        return true;
    #elif defined(ARDUINO_ARCH_RP2040)
        writeRP2040DMA(data, len);
        return true;
    #elif defined(CORE_TEENSY)
        #if defined(__IMXRT1062__)
            writeTeensy4DMA(data, len);
        #elif defined(__MK66FX1M0__) || defined(__MK64FX512__)
            writeTeensy3DMA(data, len);
        #endif
        return true;
    #else
        return false;
    #endif
}

void TFT_Interface_Parallel::writeCommand(uint8_t cmd) {
    setDC(false);  // Command mode
    if (_is16Bit) {
        writeParallelBus16(cmd);  // In 16-bit mode, pad with 0
    } else {
        writeParallelBus8(cmd);
    }
    setDC(true);   // Back to data mode
}

void TFT_Interface_Parallel::writeData(uint8_t data) {
    if (_is16Bit) {
        writeParallelBus16(data);  // In 16-bit mode, pad with 0
    } else {
        writeParallelBus8(data);
    }
}

void TFT_Interface_Parallel::writeData16(uint16_t data) {
    if (_is16Bit) {
        writeParallelBus16(data);
    } else {
        writeParallelBus8(data >> 8);    // High byte
        writeParallelBus8(data & 0xFF);  // Low byte
    }
}

uint8_t TFT_Interface_Parallel::readData() {
    setDataPinsInput();
    uint8_t data;
    if (_is16Bit) {
        data = readParallelBus16() & 0xFF;  // Read lower 8 bits in 16-bit mode
    } else {
        data = readParallelBus8();
    }
    setDataPinsOutput();
    return data;
}

uint16_t TFT_Interface_Parallel::readData16() {
    setDataPinsInput();
    uint16_t data;
    if (_is16Bit) {
        data = readParallelBus16();
    } else {
        data = (readParallelBus8() << 8);  // High byte
        data |= readParallelBus8();        // Low byte
    }
    setDataPinsOutput();
    return data;
}

void TFT_Interface_Parallel::writeDataBlock(const uint8_t* data, size_t len) {
    if (_buffering) {
        size_t remaining = BUFFER_SIZE - _bufferIndex;
        if (len <= remaining) {
            memcpy(_buffer + _bufferIndex, data, len);
            _bufferIndex += len;
            return;
        }
        flushBuffer();
    }
    
    if (supportsDMA() && len > 32) {
        startDMAWrite(data, len);
        waitDMAComplete();
    } else {
        if (_is16Bit) {
            // In 16-bit mode, we need to pad each byte
            while (len--) {
                writeParallelBus16(*data++);
            }
        } else {
            while (len--) {
                writeParallelBus8(*data++);
            }
        }
    }
}

void TFT_Interface_Parallel::writeDataBlock16(const uint16_t* data, size_t len) {
    if (supportsDMA() && len > 16) {
        startDMAWrite16(data, len);
        waitDMAComplete();
    } else {
        while (len--) {
            writeData16(*data++);
        }
    }
}

void TFT_Interface_Parallel::readDataBlock(uint8_t* data, size_t len) {
    setDataPinsInput();
    if (_is16Bit) {
        while (len--) {
            *data++ = readParallelBus16() & 0xFF;
        }
    } else {
        while (len--) {
            *data++ = readParallelBus8();
        }
    }
    setDataPinsOutput();
}

void TFT_Interface_Parallel::readDataBlock16(uint16_t* data, size_t len) {
    setDataPinsInput();
    if (_is16Bit) {
        while (len--) {
            *data++ = readParallelBus16();
        }
    } else {
        while (len--) {
            *data = (readParallelBus8() << 8);  // High byte
            *data |= readParallelBus8();        // Low byte
            data++;
        }
    }
    setDataPinsOutput();
}

void TFT_Interface_Parallel::setupPins() {
    // Setup control pins
    if (_csPin >= 0) {
        pinMode(_csPin, OUTPUT);
        digitalWrite(_csPin, HIGH);
    }
    
    if (_wrPin >= 0) {
        pinMode(_wrPin, OUTPUT);
        digitalWrite(_wrPin, HIGH);
    }
    
    if (_rdPin >= 0) {
        pinMode(_rdPin, OUTPUT);
        digitalWrite(_rdPin, HIGH);
    }
    
    if (_rstPin >= 0) {
        pinMode(_rstPin, OUTPUT);
        digitalWrite(_rstPin, HIGH);
    }

    if (_config.parallel.dc_pin >= 0) {
        pinMode(_config.parallel.dc_pin, OUTPUT);
        digitalWrite(_config.parallel.dc_pin, HIGH);
    }
    
    if (_useLatch && _latchPin >= 0) {
        pinMode(_latchPin, OUTPUT);
        digitalWrite(_latchPin, HIGH);
    }
    
    // Setup data pins
    setDataPinsOutput();
}

uint8_t TFT_Interface_Parallel::readParallelBus8() {
    uint8_t data = 0;
    pulseRD();
    
    #if defined(ESP32)
        uint32_t gpio = GPIO.in;
        for (int i = 0; i < 8; i++) {
            if (_dataPins[i] >= 0) {
                data |= ((gpio >> _dataPins[i]) & 0x01) << i;
            }
        }
    #else
        for (int i = 0; i < 8; i++) {
            if (_dataPins[i] >= 0) {
                data |= digitalRead(_dataPins[i]) << i;
            }
        }
    #endif
    
    return data;
}

uint16_t TFT_Interface_Parallel::readParallelBus16() {
    uint16_t data = 0;
    pulseRD();
    
    if (_is16Bit) {
        #if defined(ESP32)
            uint32_t gpio = GPIO.in;
            uint32_t gpio1 = GPIO.in1;
            
            // Read lower 8 bits
            for (int i = 0; i < 8; i++) {
                if (_dataPins[i] >= 0) {
                    data |= ((gpio >> _dataPins[i]) & 0x01) << i;
                }
            }
            
            // Read upper 8 bits
            for (int i = 8; i < 16; i++) {
                if (_dataPins[i] >= 0) {
                    data |= ((gpio1 >> (_dataPins[i] - 32)) & 0x01) << i;
                }
            }
        #else
            for (int i = 0; i < 16; i++) {
                if (_dataPins[i] >= 0) {
                    data |= digitalRead(_dataPins[i]) << i;
                }
            }
        #endif
    } else {
        // In 8-bit mode, read two bytes
        data = (readParallelBus8() << 8);
        data |= readParallelBus8();
    }
    
    return data;
}

void TFT_Interface_Parallel::setDataPinsOutput() {
    for (int i = 0; i < (_is16Bit ? 16 : 8); i++) {
        if (_dataPins[i] >= 0) {
            pinMode(_dataPins[i], OUTPUT);
        }
    }
}

void TFT_Interface_Parallel::setDataPinsInput() {
    for (int i = 0; i < (_is16Bit ? 16 : 8); i++) {
        if (_dataPins[i] >= 0) {
            pinMode(_dataPins[i], INPUT);
        }
    }
}

inline void TFT_Interface_Parallel::pulseWR() {
    #if defined(ESP32)
        *_wrPortClr = (1UL << _wrPin);
        *_wrPortSet = (1UL << _wrPin);
    #else
        digitalWrite(_wrPin, LOW);
        digitalWrite(_wrPin, HIGH);
    #endif
}

inline void TFT_Interface_Parallel::pulseRD() {
    #if defined(ESP32)
        *_rdPortClr = (1UL << _rdPin);
        *_rdPortSet = (1UL << _rdPin);
    #else
        digitalWrite(_rdPin, LOW);
        digitalWrite(_rdPin, HIGH);
    #endif
}

inline void TFT_Interface_Parallel::pulseLatch() {
    if (_useLatch && _latchPin >= 0) {
        #if defined(ESP32)
            *_latchPortClr = (1UL << _latchPin);
            *_latchPortSet = (1UL << _latchPin);
        #else
            digitalWrite(_latchPin, LOW);
            digitalWrite(_latchPin, HIGH);
        #endif
    }
}

inline void TFT_Interface_Parallel::delayWrite() {
    if (_writeDelay > 0) {
        delayMicroseconds(_writeDelay / 1000);
    }
}

// Transaction Management
void TFT_Interface_Parallel::beginTransaction() {
    digitalWrite(_csPin, LOW);
    #if defined(ESP32) || defined(ESP8266)
        GPIO.out_w1tc = _dcPinMask | _wrPinMask;  // Set DC and WR low
    #else
        digitalWrite(_dcPin, LOW);
        digitalWrite(_wrPin, LOW);
    #endif
}

void TFT_Interface_Parallel::endTransaction() {
    #if defined(ESP32) || defined(ESP8266)
        GPIO.out_w1ts = _dcPinMask | _wrPinMask;  // Set DC and WR high
    #else
        digitalWrite(_dcPin, HIGH);
        digitalWrite(_wrPin, HIGH);
    #endif
    digitalWrite(_csPin, HIGH);
}

void TFT_Interface_Parallel::beginRead() {
    digitalWrite(_csPin, LOW);
    #if defined(ESP32) || defined(ESP8266)
        GPIO.out_w1tc = _dcPinMask;  // Set DC low
        GPIO.out_w1ts = _rdPinMask;  // Set RD high
    #else
        digitalWrite(_dcPin, LOW);
        digitalWrite(_rdPin, HIGH);
    #endif
}

void TFT_Interface_Parallel::endRead() {
    #if defined(ESP32) || defined(ESP8266)
        GPIO.out_w1ts = _dcPinMask;  // Set DC high
        GPIO.out_w1tc = _rdPinMask;  // Set RD low
    #else
        digitalWrite(_dcPin, HIGH);
        digitalWrite(_rdPin, LOW);
    #endif
    digitalWrite(_csPin, HIGH);
}

void TFT_Interface_Parallel::begin_nin_write() {
    digitalWrite(_csPin, LOW);
    #if defined(ESP32) || defined(ESP8266)
        GPIO.out_w1tc = _dcPinMask | _wrPinMask;  // Set DC and WR low
    #else
        digitalWrite(_dcPin, LOW);
        digitalWrite(_wrPin, LOW);
    #endif
}

void TFT_Interface_Parallel::end_nin_write() {
    #if defined(ESP32) || defined(ESP8266)
        GPIO.out_w1ts = _dcPinMask | _wrPinMask;  // Set DC and WR high
    #else
        digitalWrite(_dcPin, HIGH);
        digitalWrite(_wrPin, HIGH);
    #endif
    digitalWrite(_csPin, HIGH);
}

// Viewport Management
void TFT_Interface_Parallel::setViewport(int32_t x, int32_t y, int32_t w, int32_t h, bool vpDatum) {
    // Save the viewport data
    _xDatum = x;
    _yDatum = y;
    _vpX = x;
    _vpY = y;
    _vpW = w;
    _vpH = h;
    _vpDatum = vpDatum;
    
    // Check if viewport is entirely outside display
    _vpOoB = (_vpX >= _config.width) || (_vpY >= _config.height) || (w <= 0) || (h <= 0);
    
    // Clip viewport to display boundaries
    if (_vpX < 0) { _vpW += _vpX; _vpX = 0; }
    if (_vpY < 0) { _vpH += _vpY; _vpY = 0; }
    
    if (_vpX + _vpW > _config.width)  _vpW = _config.width  - _vpX;
    if (_vpY + _vpH > _config.height) _vpH = _config.height - _vpY;
    
    // Check if viewport is entirely outside display
    if (_vpW <= 0 || _vpH <= 0) _vpOoB = true;
}

void TFT_Interface_Parallel::resetViewport() {
    // Reset viewport to default values
    _vpX = 0;
    _vpY = 0;
    _vpW = _config.width;
    _vpH = _config.height;
    _xDatum = 0;
    _yDatum = 0;
    _vpDatum = false;
    _vpOoB = false;
}

bool TFT_Interface_Parallel::checkViewport(int32_t x, int32_t y, int32_t w, int32_t h) {
    if (_vpOoB) return false; // Viewport is outside display area
    
    x += _xDatum;
    y += _yDatum;
    
    if ((x >= _vpX + _vpW) || (y >= _vpY + _vpH)) return false;
    if ((x + w <= _vpX) || (y + h <= _vpY)) return false;
    
    return true;
}

bool TFT_Interface_Parallel::clipAddrWindow(int32_t* x, int32_t* y, int32_t* w, int32_t* h) {
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

bool TFT_Interface_Parallel::clipWindow(int32_t* xs, int32_t* ys, int32_t* xe, int32_t* ye) {
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

} // namespace TFT_Runtime
