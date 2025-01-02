#ifndef _TFT_INTERFACE_PARALLEL_CPP_
#define _TFT_INTERFACE_PARALLEL_CPP_

#include "TFT_Interface_Parallel.h"
#include <Arduino.h>

namespace TFT_Runtime {

// Forward declarations
void TFT_Interface_Parallel::dmaInterrupt() {
#if defined(CORE_TEENSY)
    // TODo add Teensy suuport
#elif defined(ESP32)
    if (_dmaInitialized) {
        // Check if DMA transfer is complete
        if (_dmaBufferReady) {
            // Set up next transfer
            i2s_write_expand(I2S_NUM_0, _dmaBuf, DMA_BUFFER_SIZE, 8, 8, 0, 0);
            _dmaBufferReady = false;
        }
    }
#elif defined(ARDUINO_ARCH_RP2040)
    // RP2040-specific DMA handling
    if (_dmaBufferReady) {
        _dmaBufferReady = false;
        // RP2040 uses its own DMA system
    }
#elif defined(STM32F1) || defined(STM32F4) || defined(STM32F7)
    // STM32-specific DMA handling
    if (_dmaBufferReady) {
        _dmaBufferReady = false;
        // STM32 uses its own DMA system
    }
#endif

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
    // ESP32 parallel interface using I2S - optimized settings
    
    // Enhanced DMA settings for better performance
    const size_t dma_buf_count = 4;  // Optimal buffer count for ESP32
    const size_t dma_buf_len = 4096; // Increased for better throughput
    const size_t total_dma_size = dma_buf_count * dma_buf_len;
    
    i2s_config_t i2s_config = {
        .mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
        .sample_rate = 40000000,  // Maximum reliable speed for parallel interface
        .bits_per_sample = I2S_BITS_PER_SAMPLE_8BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1 | ESP_INTR_FLAG_IRAM,  // Added IRAM flag for better performance
        .dma_buf_count = dma_buf_count,
        .dma_buf_len = dma_buf_len,
        .use_apll = true,  // Use APLL for better clock accuracy
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    // Initialize I2S with parallel mode
    esp_err_t err = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    if (err != ESP_OK) {
        log_e("I2S driver installation failed with error: %d", err);
        return false;
    }

    // Configure GPIO matrix for optimal performance
    err = i2s_set_pin(I2S_NUM_0, NULL);
    if (err != ESP_OK) {
        log_e("I2S pin configuration failed with error: %d", err);
        i2s_driver_uninstall(I2S_NUM_0);
        return false;
    }

    // Configure GPIO pins for parallel interface with optimized settings
    gpio_config_t io_conf = {
        .pin_bit_mask = 0,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    // Set up data pins with consecutive GPIO pins when possible for better performance
    uint64_t pin_mask = 0;
    for (int i = 0; i < 8; i++) {
        pin_mask |= (1ULL << _dataPins[i]);
    }
    io_conf.pin_bit_mask = pin_mask;
    
    err = gpio_config(&io_conf);
    if (err != ESP_OK) {
        log_e("GPIO configuration failed with error: %d", err);
        i2s_driver_uninstall(I2S_NUM_0);
        return false;
    }

    // Allocate DMA buffer with alignment
    _dmaBuf = (uint8_t*)heap_caps_aligned_alloc(16, total_dma_size, MALLOC_CAP_DMA);
    if (!_dmaBuf) {
        log_e("DMA buffer allocation failed");
        i2s_driver_uninstall(I2S_NUM_0);
        return false;
    }

    // Create DMA descriptor with proper alignment
    _dmadesc = (lldesc_t*)heap_caps_aligned_alloc(16, sizeof(lldesc_t), MALLOC_CAP_DMA);
    if (!_dmadesc) {
        log_e("DMA descriptor allocation failed");
        heap_caps_free(_dmaBuf);
        _dmaBuf = nullptr;
        i2s_driver_uninstall(I2S_NUM_0);
        return false;
    }

    // Configure DMA descriptor with size-limited chunks
    const size_t max_chunk_size = 4095; // Maximum size for 12-bit field
    size_t remaining_size = total_dma_size;
    size_t chunk_size = (remaining_size > max_chunk_size) ? max_chunk_size : remaining_size;
    
    _dmadesc->size = chunk_size;
    _dmadesc->length = chunk_size;
    _dmadesc->buf = _dmaBuf;
    _dmadesc->eof = (remaining_size <= max_chunk_size) ? 1 : 0;
    _dmadesc->sosf = 0;
    _dmadesc->owner = 1;
    _dmadesc->qe.stqe_next = nullptr;
    _dmadesc->offset = 0;

    // If we need more chunks, allocate additional descriptors
    lldesc_t* current_desc = _dmadesc;
    remaining_size -= chunk_size;
    size_t offset = chunk_size;
    
    while (remaining_size > 0) {
        lldesc_t* next_desc = (lldesc_t*)heap_caps_malloc(sizeof(lldesc_t), MALLOC_CAP_DMA);
        if (!next_desc) {
            log_e("Failed to allocate additional DMA descriptor");
            // Clean up already allocated descriptors
            cleanupDMA();
            return false;
        }
        
        chunk_size = (remaining_size > max_chunk_size) ? max_chunk_size : remaining_size;
        
        next_desc->size = chunk_size;
        next_desc->length = chunk_size;
        next_desc->buf = _dmaBuf + offset;
        next_desc->eof = (remaining_size <= max_chunk_size) ? 1 : 0;
        next_desc->sosf = 0;
        next_desc->owner = 1;
        next_desc->qe.stqe_next = nullptr;
        next_desc->offset = 0;
        
        current_desc->qe.stqe_next = next_desc;
        current_desc = next_desc;
        
        remaining_size -= chunk_size;
        offset += chunk_size;
    }

    // Use optimal DMA channel
    _dmaChannel = 1;
    _dmaInitialized = true;
    _dmaBufferReady = true;

    log_i("ESP32 parallel interface initialized successfully");
    return true;
#elif defined(CORE_TEENSY)
    //TODO Implement parallel interface for Teensy
#elif defined(ARDUINO_ARCH_ESP8266)
    // ESP8266 doesn't support DMA for this purpose
    return true;
#elif defined(ARDUINO_ARCH_STM32)
    // STM32 specific DMA setup would go here
    return true;
#elif defined(ARDUINO_ARCH_RP2040)
    // RP2040 specific DMA setup would go here
    return true;
#elif defined(ARDUINO_SAM_DUE)
    // Due specific DMA setup would go here
    return true;
#elif defined(ARDUINO_AVR_MEGA2560)
    // Mega doesn't support DMA
    return true;
#else
    #warning "Platform not specifically supported for DMA"
    return true;
#endif

    // Common initialization for non-DMA platforms
    if (!supportsDMA()) {
        _dmaInitialized = false;
        _dmaBufferReady = false;
        return true;
    }

    return false;  // Should not reach here
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
        data = read8();
    #elif defined(ESP8266)
        data = read8();
    #elif defined(ARDUINO_ARCH_RP2040)
        data = read8();
    #elif defined(ARDUINO_SAM_DUE)
        data = read8();
    #elif defined(__AVR__)
        data = read8();
    #elif defined(CORE_TEENSY)
        //TODO add Teensy support
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
            data = read16();
        #elif defined(ESP8266)
            data = read16();
        #elif defined(ARDUINO_ARCH_RP2040)
            data = read16();
        #elif defined(ARDUINO_SAM_DUE)
            data = read16();
        #elif defined(__AVR__)
            data = read16();
        #elif defined(CORE_TEENSY)
            //TODO add Teensy support
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
        // Use direct register manipulation for ESP32
        gpio_set_level((gpio_num_t)_wrPin, 0);
        for (int i = 0; i < 8; i++) {
            if (_dataPins[i] >= 0) {
                gpio_set_level((gpio_num_t)_dataPins[i], (data >> i) & 0x01);
            }
        }
        gpio_set_level((gpio_num_t)_wrPin, 1);
    #elif defined(ESP8266)
        // ESP8266 specific implementation
        digitalWrite(_wrPin, LOW);
        for (int i = 0; i < 8; i++) {
            if (_dataPins[i] >= 0) {
                digitalWrite(_dataPins[i], (data >> i) & 0x01);
            }
        }
        digitalWrite(_wrPin, HIGH);
    #elif defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41)
        // Teensy 4.x specific implementation
        digitalWriteFast(_wrPin, LOW);
        for (int i = 0; i < 8; i++) {
            if (_dataPins[i] >= 0) {
                digitalWriteFast(_dataPins[i], (data >> i) & 0x01);
            }
        }
        digitalWriteFast(_wrPin, HIGH);
    #else
        // Generic implementation for other platforms
        digitalWrite(_wrPin, LOW);
        for (int i = 0; i < 8; i++) {
            if (_dataPins[i] >= 0) {
                digitalWrite(_dataPins[i], (data >> i) & 0x01);
            }
        }
        digitalWrite(_wrPin, HIGH);
    #endif
    delayWrite();
}

void TFT_Interface_Parallel::write16(uint16_t data) {
    #if defined(ESP32)
        // ESP32 specific fast GPIO using direct register access
        #if ESP_IDF_VERSION_MAJOR >= 4
            gpio_set_level((gpio_num_t)_wrPin, 0);
            for(uint8_t i = 0; i < 8; i++) {
                if(_dataPins[i] >= 0) gpio_set_level((gpio_num_t)_dataPins[i], (data >> (8 + i)) & 0x01);
            }
            gpio_set_level((gpio_num_t)_wrPin, 1);
            gpio_set_level((gpio_num_t)_wrPin, 0);
            for(uint8_t i = 0; i < 8; i++) {
                if(_dataPins[i] >= 0) gpio_set_level((gpio_num_t)_dataPins[i], (data >> i) & 0x01);
            }
            gpio_set_level((gpio_num_t)_wrPin, 1);
        #else
            // Legacy ESP32 GPIO handling
            GPIO.out_w1tc = (1 << _wrPin);
            for(uint8_t i = 0; i < 8; i++) {
                if(_dataPins[i] >= 0) {
                    if(data & (0x8000 >> i)) GPIO.out_w1ts = (1 << _dataPins[i]);
                    else GPIO.out_w1tc = (1 << _dataPins[i]);
                }
            }
            GPIO.out_w1ts = (1 << _wrPin);
            GPIO.out_w1tc = (1 << _wrPin);
            for(uint8_t i = 0; i < 8; i++) {
                if(_dataPins[i] >= 0) {
                    if(data & (0x80 >> i)) GPIO.out_w1ts = (1 << _dataPins[i]);
                    else GPIO.out_w1tc = (1 << _dataPins[i]);
                }
            }
            GPIO.out_w1ts = (1 << _wrPin);
        #endif
    #elif defined(ESP8266)
        // ESP8266 specific GPIO handling
        GPOC = (1 << _wrPin);  // WR low
        for(uint8_t i = 0; i < 8; i++) {
            if(_dataPins[i] >= 0) {
                if(data & (0x8000 >> i)) GPOS = (1 << _dataPins[i]);
                else GPOC = (1 << _dataPins[i]);
            }
        }
        GPOS = (1 << _wrPin);  // WR high
        GPOC = (1 << _wrPin);  // WR low
        for(uint8_t i = 0; i < 8; i++) {
            if(_dataPins[i] >= 0) {
                if(data & (0x80 >> i)) GPOS = (1 << _dataPins[i]);
                else GPOC = (1 << _dataPins[i]);
            }
        }
        GPOS = (1 << _wrPin);  // WR high
    #else
        // Generic implementation for other platforms (Teensy, STM32, RP2040, Due, Mega)
        digitalWrite(_wrPin, LOW);
        // Write high byte
        for(uint8_t i = 0; i < 8; i++) {
            if(_dataPins[i] >= 0) {
                digitalWrite(_dataPins[i], (data >> (15 - i)) & 0x01);
            }
        }
        digitalWrite(_wrPin, HIGH);
        digitalWrite(_wrPin, LOW);
        // Write low byte
        for(uint8_t i = 0; i < 8; i++) {
            if(_dataPins[i] >= 0) {
                digitalWrite(_dataPins[i], (data >> (7 - i)) & 0x01);
            }
        }
        digitalWrite(_wrPin, HIGH);
    #endif
    delayWrite();
}

void TFT_Interface_Parallel::writeDataBlock(const uint8_t* data, size_t len) {
    if (len == 0) return;

    // Default implementation - write bytes one at a time
    while (len--) {
        write8(*data++);
    }
}

void TFT_Interface_Parallel::setupPins() {
    
}

void TFT_Interface_Parallel::setupParallelBus() {
    #if defined(CORE_TEENSY)
        //TODO add Teensy support
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
    if (_dmaInitialized) {
        #if defined(ESP32)
            if (_dmadesc) {
                // Free all DMA descriptors in the linked list
                lldesc_t* current = _dmadesc;
                while (current != nullptr) {
                    lldesc_t* next = current->qe.stqe_next;
                    heap_caps_free(current);
                    current = next;
                }
                _dmadesc = nullptr;
            }
            
            if (_dmaBuf) {
                heap_caps_free(_dmaBuf);
                _dmaBuf = nullptr;
            }
            
            i2s_driver_uninstall(I2S_NUM_0);
        #elif defined(ARDUINO_ARCH_RP2040)
            if (_dmaBuf) {
                free(_dmaBuf);
                _dmaBuf = nullptr;
            }
            if (_dmaChannel >= 0) {
                dma_channel_abort(_dmaChannel);
                dma_channel_unclaim(_dmaChannel);
                _dmaChannel = -1;
            }
        #endif
        _dmaInitialized = false;
    }
}

void TFT_Interface_Parallel::waitDMAComplete() {
#if defined(CORE_TEENSY)
    #if defined(__IMXRT1062__) || defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__) || defined(__MK20DX128__)  // Teensy 4.x or 3.6 or 3.5
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