#ifndef _FLEXIO_HANDLER_H_
#define _FLEXIO_HANDLER_H_

#include <Arduino.h>

// FlexIO is only available on Teensy 4.x series
#if defined(CORE_TEENSY) && defined(__IMXRT1062__)

#include "imxrt.h"
#include "core_pins.h"

// FlexIO Control Register bits
#define FLEXIO_CTRL_FLEXEN        (1 << 0)
#define FLEXIO_CTRL_SWRST         (1 << 1)
#define FLEXIO_CTRL_FASTACC       (1 << 2)
#define FLEXIO_CTRL_DBGE          (1 << 3)
#define FLEXIO_CTRL_DOZEN         (1 << 4)

// FlexIO2 clock gate definition
#define CCM_CCGR_ON   3

class FlexIOHandler {
public:
    enum class ShifterMode {
        Input,
        Output
    };

    enum class TimerMode {
        Disabled,
        Trigger
    };

    enum class ShifterConfig {
        Width8Bit = 0x07,
        TimerTrig = 0x08,
        PinOutput = 0x10
    };

    friend constexpr ShifterConfig operator|(ShifterConfig a, ShifterConfig b) {
        return static_cast<ShifterConfig>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    enum class TimerConfig {
        Trigger = 0x01,
        PinOutput = 0x02
    };

    friend constexpr TimerConfig operator|(TimerConfig a, TimerConfig b) {
        return static_cast<TimerConfig>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    FlexIOHandler() {
        // Enable FLEXIO2 clock
        CCM_CCGR3 |= CCM_CCGR3_FLEXIO2(CCM_CCGR_ON);
        
        // Map registers using predefined addresses from imxrt.h
        ctrl = &FLEXIO2_CTRL;
        shiftctl = &FLEXIO2_SHIFTCTL0;
        shiftcfg = &FLEXIO2_SHIFTCFG0;
        shiftbuf = &FLEXIO2_SHIFTBUF0;
        timctl = &FLEXIO2_TIMCTL0;
        timcfg = &FLEXIO2_TIMCFG0;
        timcmp = &FLEXIO2_TIMCMP0;
    }

    bool begin() {
        // Reset FLEXIO
        *ctrl = FLEXIO_CTRL_SWRST;
        delayMicroseconds(1); // Small delay after reset
        *ctrl = 0;
        
        // Configure for fast GPIO operations with debug enabled
        *ctrl = FLEXIO_CTRL_FLEXEN | FLEXIO_CTRL_FASTACC | FLEXIO_CTRL_DBGE;
        return (*ctrl & FLEXIO_CTRL_FLEXEN) != 0;
    }

    uint8_t addShifter(ShifterMode mode) {
        // Find first available shifter
        for (uint8_t i = 0; i < 4; i++) {
            if (!(shifterInUse & (1 << i))) {
                shifterInUse |= (1 << i);
                // Configure basic shifter settings based on mode
                uint32_t config = 0;
                if (mode == ShifterMode::Output) {
                    config |= (1 << 16); // PINCFG = Output
                }
                *(shiftctl + i) = config;
                return i;
            }
        }
        return 0xFF;  // No available shifter
    }

    uint8_t addTimer(TimerMode mode) {
        // Find first available timer
        for (uint8_t i = 0; i < 4; i++) {
            if (!(timerInUse & (1 << i))) {
                timerInUse |= (1 << i);
                // Configure basic timer settings based on mode
                uint32_t config = 0;
                if (mode == TimerMode::Trigger) {
                    config |= (1 << 19); // TRGSEL = Internal Trigger
                }
                *(timctl + i) = config;
                return i;
            }
        }
        return 0xFF;  // No available timer
    }

    void setShifterConfig(uint8_t shifter, uint32_t config) {
        if (shifter < 4) {
            *(shiftctl + shifter) = config;
        }
    }

    void setTimerConfig(uint8_t timer, uint32_t config) {
        if (timer < 4) {
            *(timctl + timer) = config;
        }
    }

    void writeShifter(uint8_t shifter, uint32_t data) {
        if (shifter < 4) {
            *(shiftbuf + shifter) = data;
        }
    }

    uint32_t readShifter(uint8_t shifter) {
        if (shifter < 4) {
            return *(shiftbuf + shifter);
        }
        return 0;
    }

    void releaseShifter(uint8_t shifter) {
        if (shifter < 4) {
            shifterInUse &= ~(1 << shifter);
            *(shiftctl + shifter) = 0; // Reset configuration
        }
    }

    void releaseTimer(uint8_t timer) {
        if (timer < 4) {
            timerInUse &= ~(1 << timer);
            *(timctl + timer) = 0; // Reset configuration
        }
    }

    // Get the data register address for DMA operations
    volatile uint32_t* getDataRegister(uint8_t shifter) const {
        if (shifter < 4) {
            return shiftbuf + shifter;
        }
        return nullptr;
    }

private:
    volatile uint32_t* ctrl;
    volatile uint32_t* shiftctl;
    volatile uint32_t* shiftcfg;
    volatile uint32_t* shiftbuf;
    volatile uint32_t* timctl;
    volatile uint32_t* timcfg;
    volatile uint32_t* timcmp;
    uint8_t shifterInUse = 0;
    uint8_t timerInUse = 0;
};

#else // If not Teensy 4.x, provide a stub implementation

class FlexIOHandler {
public:
    enum class ShifterMode { Input, Output };
    enum class TimerMode { Disabled, Trigger };
    enum class ShifterConfig { Width8Bit = 0x07, TimerTrig = 0x08, PinOutput = 0x10 };
    friend constexpr ShifterConfig operator|(ShifterConfig a, ShifterConfig b) {
        return static_cast<ShifterConfig>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }
    enum class TimerConfig { Trigger = 0x01, PinOutput = 0x02 };
    friend constexpr TimerConfig operator|(TimerConfig a, TimerConfig b) {
        return static_cast<TimerConfig>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    FlexIOHandler() {}
    bool begin() { return false; }
    uint8_t addShifter(ShifterMode mode) { return 0xFF; }
    uint8_t addTimer(TimerMode mode) { return 0xFF; }
    void setShifterConfig(uint8_t shifter, uint32_t config) {}
    void setTimerConfig(uint8_t timer, uint32_t config) {}
    void writeShifter(uint8_t shifter, uint32_t data) {}
    uint32_t readShifter(uint8_t shifter) { return 0; }
    void releaseShifter(uint8_t shifter) {}
    void releaseTimer(uint8_t timer) {}

    // Stub implementation of getDataRegister for non-Teensy 4.x platforms
    volatile uint32_t* getDataRegister(uint8_t shifter) const { return nullptr; }

private:
    uint8_t shifterInUse = 0;
    uint8_t timerInUse = 0;
};

#endif // defined(CORE_TEENSY) && defined(__IMXRT1062__)

#endif // _FLEXIO_HANDLER_H_
