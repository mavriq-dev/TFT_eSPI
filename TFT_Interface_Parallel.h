#ifndef _TFT_INTERFACE_PARALLEL_H_
#define _TFT_INTERFACE_PARALLEL_H_

#include "TFT_Interface.h"

// Platform-specific includes
#if defined(ESP32)
    #include "soc/spi_struct.h"
    #include "esp32/rom/lldesc.h"
#elif defined(ARDUINO_ARCH_RP2040)
    #include "hardware/pio.h"
    #include "hardware/dma.h"
    #include "hardware/clocks.h"
#endif

#if defined(CORE_TEENSY) && defined(__IMXRT1062__)
    #include <imxrt.h>
    #include <FlexIO_t4.h>
    #include <DMAChannel.h>
#endif

namespace TFT_Runtime {

class TFT_Interface_Parallel : public TFT_Interface {
public:
    explicit TFT_Interface_Parallel(const Config& config);
    ~TFT_Interface_Parallel() override;

    bool begin() override;
    void writeCommand(uint8_t cmd) override;
    void writeData(uint8_t data) override;
    void writeData16(uint16_t data) override;
    uint8_t readData() override;
    uint16_t readData16() override;
    void writeDataBlock(const uint8_t* data, size_t len) override;
    void writeDataBlock16(const uint16_t* data, size_t len) override;
    void readDataBlock(uint8_t* data, size_t len) override;
    void readDataBlock16(uint16_t* data, size_t len) override;

    void beginTransaction() override;
    void endTransaction() override;
    void beginRead() override;
    void endRead() override;
    void begin_nin_write() override;
    void end_nin_write() override;

    bool supportsDMA() override;
    bool startDMAWrite(const uint8_t* data, size_t len) override;

    void setViewport(int32_t x, int32_t y, int32_t w, int32_t h, bool vpDatum = false) override;
    void resetViewport() override;
    bool checkViewport(int32_t x, int32_t y, int32_t w, int32_t h) override;
    bool clipAddrWindow(int32_t* x, int32_t* y, int32_t* w, int32_t* h) override;
    bool clipWindow(int32_t* xs, int32_t* ys, int32_t* xe, int32_t* ye) override;

protected:
    // Platform-specific initialization
    bool initESP32();
    bool initESP8266();
    bool initRP2040();
    bool initSAMDUE();
    bool initAVR();
    bool initTeensy();

    // Platform-specific write methods
    void writeESP32_8(uint8_t data);
    void writeESP32_16(uint16_t data);
    void writeESP8266_8(uint8_t data);
    void writeESP8266_16(uint16_t data);
    void writeRP2040_8(uint8_t data);
    void writeRP2040_16(uint16_t data);
    void writeSAMDUE_8(uint8_t data);
    void writeSAMDUE_16(uint16_t data);
    void writeAVR_8(uint8_t data);
    void writeAVR_16(uint16_t data);
    void writeTeensy_8(uint8_t data);
    void writeTeensy_16(uint16_t data);

    // Platform-specific read methods
    uint8_t readESP32_8();
    uint16_t readESP32_16();
    uint8_t readESP8266_8();
    uint16_t readESP8266_16();
    uint8_t readRP2040_8();
    uint16_t readRP2040_16();
    uint8_t readSAMDUE_8();
    uint16_t readSAMDUE_16();
    uint8_t readAVR_8();
    uint16_t readAVR_16();

    // DMA support methods
    void initDMA();
    void cleanupDMA();
    void waitDMAComplete();

    // Helper methods
    void setupPins();
    void setDataPinsOutput();
    void setDataPinsInput();
    void pulseWR();
    void pulseRD();
    void pulseLatch();
    void delayWrite();

private:
    // Pin configuration
    const int8_t _csPin;
    const int8_t _dcPin;    // Data/Command pin
    const int8_t _wrPin;
    const int8_t _rdPin;
    const int8_t _rstPin;
    const int8_t _latchPin;
    const bool _is16Bit;
    const bool _useLatch;
    const uint8_t _writeDelay;

    // Data pins array
    int8_t _dataPins[16];  // Support up to 16-bit parallel

    // Platform-specific members
#if defined(ESP32)
    int8_t _dmaChannel;
    uint8_t* _dmaBuf;
    lldesc_t* _dmadesc;
    bool _dmaInitialized;
#elif defined(ARDUINO_ARCH_RP2040)
    PIO _pio;
    uint _sm;
    uint _dma_chan;
    bool _pioInitialized;
#elif defined(CORE_TEENSY)
    #if defined(__IMXRT1062__)
        IMXRT_FLEXIO_t* _flexIO;
        uint8_t _flexIOShifter;
        uint8_t _flexIOTimer;
        DMAChannel* _dmaChannel;
    #endif
#endif

    // Viewport management
    int32_t _vpX, _vpY, _vpW, _vpH;
    bool _vpDatum;
    bool _vpActive;
};

} // namespace TFT_Runtime

#endif // _TFT_INTERFACE_PARALLEL_H_
