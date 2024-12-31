#ifndef _TFT_COMMAND_TRANSLATOR_H_
#define _TFT_COMMAND_TRANSLATOR_H_

#include "TFT_Command.h"
#include "TFT_Runtime.h"
#include <unordered_map>
#include <functional>

namespace TFT_Runtime {

// Command cache entry with timestamp for LRU tracking
struct CacheEntry {
    CommandSequence sequence;
    uint32_t last_used;
    uint32_t use_count;
};

class CommandTranslator {
public:
    explicit CommandTranslator(const Config& config) : _config(config) {
        initializeCommandMaps();
    }

    // Get optimized command sequence for a specific display operation
    CommandSequence getOptimizedSequence(const CommandSequence& original) {
        // First check cache
        auto cached = _sequenceCache.find(original.name());
        if (cached != _sequenceCache.end()) {
            cached->second.last_used = millis();
            cached->second.use_count++;
            return cached->second.sequence;
        }

        // Optimize sequence
        CommandSequence optimized = optimizeSequence(original);

        // Cache if frequently used
        cacheSequenceIfNeeded(original.name(), optimized);

        return optimized;
    }

    // Translate commands between different display drivers
    CommandSequence translateSequence(const CommandSequence& source, DisplayDriver targetDriver) {
        CommandSequence translated(source.name() + "_translated");
        
        for (const auto& cmd : source.commands()) {
            switch (cmd.type) {
                case CommandType::COMMAND:
                case CommandType::COMMAND_LIST:
                    translateCommand(cmd, targetDriver, translated);
                    break;
                default:
                    translated.addCommand(cmd);
                    break;
            }
        }

        return translated;
    }

private:
    static constexpr size_t MAX_CACHE_SIZE = 32;
    static constexpr uint32_t CACHE_THRESHOLD = 5; // Cache after this many uses

    const Config& _config;
    std::unordered_map<std::string, CacheEntry> _sequenceCache;
    std::unordered_map<uint8_t, std::unordered_map<DisplayDriver, uint8_t>> _commandTranslationMap;

    void initializeCommandMaps() {
        // Common commands that are the same across most displays
        auto& ili9341 = _commandTranslationMap;
        
        // Basic commands - same across all displays
        addCommonTranslation(0x00, 0x00);  // NOP
        addCommonTranslation(0x01, 0x01);  // SWRST
        addCommonTranslation(0x10, 0x10);  // SLPIN
        addCommonTranslation(0x11, 0x11);  // SLPOUT
        addCommonTranslation(0x20, 0x20);  // INVOFF
        addCommonTranslation(0x21, 0x21);  // INVON
        addCommonTranslation(0x28, 0x28);  // DISPOFF
        addCommonTranslation(0x29, 0x29);  // DISPON
        addCommonTranslation(0x2A, 0x2A);  // CASET
        addCommonTranslation(0x2B, 0x2B);  // PASET
        addCommonTranslation(0x2C, 0x2C);  // RAMWR
        addCommonTranslation(0x2E, 0x2E);  // RAMRD
        addCommonTranslation(0x3A, 0x3A);  // COLMOD
        addCommonTranslation(0x36, 0x36);  // MADCTL

        // Initialize all display-specific translations
        initializeILI9341Translations();
        initializeST7789Translations();
        initializeST7735Translations();
        initializeILI948xTranslations();
        initializeHX8357Translations();
        initializeSSD1351Translations();
        initializeGC9A01Translations();
        initializeRM68140Translations();
        initializeR61581Translations();
        initializeSSD1963Translations();
        initializeST7796Translations();
        initializeST7789V2Translations();
        initializeST7735RTranslations();
    }

    // ILI9341 specific translations
    void initializeILI9341Translations() {
        // Power Control
        addTranslation(DisplayDriver::ILI9341, DisplayDriver::ST7789, 0xCB, 0xC3);  // Power Control A
        addTranslation(DisplayDriver::ILI9341, DisplayDriver::ST7789, 0xCF, 0xC7);  // Power Control B
        addTranslation(DisplayDriver::ILI9341, DisplayDriver::ILI9486, 0xC0, 0xC0); // Power Control 1
        addTranslation(DisplayDriver::ILI9341, DisplayDriver::ILI9486, 0xC1, 0xC1); // Power Control 2
        
        // VCOM Control
        addTranslation(DisplayDriver::ILI9341, DisplayDriver::ST7789, 0xC7, 0xC7);
        addTranslation(DisplayDriver::ILI9341, DisplayDriver::ILI9486, 0xC5, 0xC5);
        
        // Memory Access Control
        addTranslation(DisplayDriver::ILI9341, DisplayDriver::HX8357D, 0x36, 0x36);
        addTranslation(DisplayDriver::ILI9341, DisplayDriver::ILI9486, 0x36, 0x36);
    }

    void initializeST7789Translations() {
        // Frame Rate Control
        addTranslation(DisplayDriver::ST7789, DisplayDriver::ILI9341, 0xB2, 0xB1);
        addTranslation(DisplayDriver::ST7789, DisplayDriver::ST7735, 0xB2, 0xB1);
        
        // Display Function Control
        addTranslation(DisplayDriver::ST7789, DisplayDriver::ILI9341, 0xB6, 0xB6);
        addTranslation(DisplayDriver::ST7789, DisplayDriver::ST7735, 0xB6, 0xB6);
        
        // Power Control
        addTranslation(DisplayDriver::ST7789, DisplayDriver::ILI9341, 0xBB, 0xC0);
        addTranslation(DisplayDriver::ST7789, DisplayDriver::ST7735, 0xBB, 0xC0);
    }

    void initializeST7735Translations() {
        // Frame Rate Control
        addTranslation(DisplayDriver::ST7735, DisplayDriver::ILI9341, 0xB1, 0xB1);
        addTranslation(DisplayDriver::ST7735, DisplayDriver::ST7789, 0xB1, 0xB2);
        
        // Display Function Control
        addTranslation(DisplayDriver::ST7735, DisplayDriver::ILI9341, 0xB6, 0xB6);
        addTranslation(DisplayDriver::ST7735, DisplayDriver::ST7789, 0xB6, 0xB7);
    }

    void initializeILI948xTranslations() {
        // ILI9486/ILI9488 specific
        addTranslation(DisplayDriver::ILI9486, DisplayDriver::ILI9488, 0xE9, 0xE9); // Set Image Function
        addTranslation(DisplayDriver::ILI9486, DisplayDriver::ILI9488, 0xF7, 0xF7); // Adjust Control
        
        // Power Control
        addTranslation(DisplayDriver::ILI9486, DisplayDriver::ILI9341, 0xC0, 0xC0);
        addTranslation(DisplayDriver::ILI9486, DisplayDriver::ST7789, 0xC0, 0xBB);
    }

    void initializeHX8357Translations() {
        // HX8357D specific commands
        addTranslation(DisplayDriver::HX8357D, DisplayDriver::ILI9341, 0xE9, 0xCF); // Enable 3G
        addTranslation(DisplayDriver::HX8357D, DisplayDriver::ILI9341, 0xC3, 0xC0); // Power Control 3
        addTranslation(DisplayDriver::HX8357D, DisplayDriver::ILI9341, 0xC4, 0xC1); // Power Control 4
        
        // VCOM Control
        addTranslation(DisplayDriver::HX8357D, DisplayDriver::ILI9341, 0xC5, 0xC5);
        addTranslation(DisplayDriver::HX8357D, DisplayDriver::ST7789, 0xC5, 0xC7);
    }

    void initializeSSD1351Translations() {
        // SSD1351 has quite different command set, many need special handling
        addTranslation(DisplayDriver::SSD1351, DisplayDriver::ILI9341, 0x15, 0x2A); // Set Column Address
        addTranslation(DisplayDriver::SSD1351, DisplayDriver::ILI9341, 0x75, 0x2B); // Set Row Address
        addTranslation(DisplayDriver::SSD1351, DisplayDriver::ILI9341, 0x5C, 0x2C); // Write RAM
        
        // Display commands
        addTranslation(DisplayDriver::SSD1351, DisplayDriver::ILI9341, 0xA0, 0x36); // Remap
        addTranslation(DisplayDriver::SSD1351, DisplayDriver::ILI9341, 0xA4, 0x28); // Display Off
        addTranslation(DisplayDriver::SSD1351, DisplayDriver::ILI9341, 0xA5, 0x29); // Display On
    }

    void initializeGC9A01Translations() {
        // Power Control
        addTranslation(DisplayDriver::GC9A01, DisplayDriver::ILI9341, 0xFE, 0xCF);  // Inter Register Enable 1
        addTranslation(DisplayDriver::GC9A01, DisplayDriver::ILI9341, 0xEF, 0xCB);  // Inter Register Enable 2
        addTranslation(DisplayDriver::GC9A01, DisplayDriver::ST7789, 0xEB, 0xB7);   // VCOM Control
        
        // Display Function Control
        addTranslation(DisplayDriver::GC9A01, DisplayDriver::ILI9341, 0x85, 0xB6);  // Function Control
        addTranslation(DisplayDriver::GC9A01, DisplayDriver::ST7789, 0x85, 0xB6);
        
        // Power Control
        addTranslation(DisplayDriver::GC9A01, DisplayDriver::ILI9341, 0xA6, 0xC0);  // GVDD Control
        addTranslation(DisplayDriver::GC9A01, DisplayDriver::ST7789, 0xA6, 0xBB);
        
        // Gamma Control
        addTranslation(DisplayDriver::GC9A01, DisplayDriver::ILI9341, 0xE0, 0xE0);  // Positive Gamma
        addTranslation(DisplayDriver::GC9A01, DisplayDriver::ILI9341, 0xE1, 0xE1);  // Negative Gamma
    }

    void initializeRM68140Translations() {
        // Power Control
        addTranslation(DisplayDriver::RM68140, DisplayDriver::ILI9341, 0xC0, 0xC0);  // Power Control 1
        addTranslation(DisplayDriver::RM68140, DisplayDriver::ILI9341, 0xC1, 0xC1);  // Power Control 2
        addTranslation(DisplayDriver::RM68140, DisplayDriver::ST7789, 0xC0, 0xBB);   // VREG1 Control
        
        // VCOM Control
        addTranslation(DisplayDriver::RM68140, DisplayDriver::ILI9341, 0xC5, 0xC5);
        addTranslation(DisplayDriver::RM68140, DisplayDriver::ST7789, 0xC5, 0xC7);
        
        // Frame Rate Control
        addTranslation(DisplayDriver::RM68140, DisplayDriver::ILI9341, 0xB1, 0xB1);
        addTranslation(DisplayDriver::RM68140, DisplayDriver::ST7789, 0xB1, 0xB2);
        
        // Display Function Control
        addTranslation(DisplayDriver::RM68140, DisplayDriver::ILI9341, 0xB6, 0xB6);
        addTranslation(DisplayDriver::RM68140, DisplayDriver::ST7789, 0xB6, 0xB7);
    }

    void initializeR61581Translations() {
        // Power Control
        addTranslation(DisplayDriver::R61581, DisplayDriver::ILI9341, 0xB0, 0xC0);  // Power Control 1
        addTranslation(DisplayDriver::R61581, DisplayDriver::ILI9341, 0xB1, 0xC1);  // Power Control 2
        
        // Panel Driving
        addTranslation(DisplayDriver::R61581, DisplayDriver::ILI9341, 0xC0, 0xB6);
        addTranslation(DisplayDriver::R61581, DisplayDriver::ST7789, 0xC0, 0xB7);
        
        // Gamma Setting
        addTranslation(DisplayDriver::R61581, DisplayDriver::ILI9341, 0xC8, 0xE0);
        addTranslation(DisplayDriver::R61581, DisplayDriver::ST7789, 0xC8, 0xE0);
    }

    void initializeSSD1963Translations() {
        // Set PLL
        addTranslation(DisplayDriver::SSD1963, DisplayDriver::ILI9341, 0xE0, 0xC0);  // Start PLL
        addTranslation(DisplayDriver::SSD1963, DisplayDriver::ILI9341, 0xE2, 0xC1);  // Set PLL
        
        // LCD Panel
        addTranslation(DisplayDriver::SSD1963, DisplayDriver::ILI9341, 0xE6, 0xB6);  // Panel Setting
        addTranslation(DisplayDriver::SSD1963, DisplayDriver::ST7789, 0xE6, 0xB7);
        
        // Display Control
        addTranslation(DisplayDriver::SSD1963, DisplayDriver::ILI9341, 0xB0, 0x36);  // LCD Mode
        addTranslation(DisplayDriver::SSD1963, DisplayDriver::ILI9341, 0xB4, 0x3A);  // Color Mode
        
        // Power Control
        addTranslation(DisplayDriver::SSD1963, DisplayDriver::ILI9341, 0xBA, 0xC0);  // Power Control
        addTranslation(DisplayDriver::SSD1963, DisplayDriver::ST7789, 0xBA, 0xBB);
    }

    void initializeST7796Translations() {
        // ST7796 is very similar to ILI9341 in command set
        // Power Control
        addTranslation(DisplayDriver::ST7796, DisplayDriver::ILI9341, 0xC0, 0xC0);  // Power Control 1
        addTranslation(DisplayDriver::ST7796, DisplayDriver::ILI9341, 0xC1, 0xC1);  // Power Control 2
        addTranslation(DisplayDriver::ST7796, DisplayDriver::ST7789, 0xC0, 0xBB);   // VREG1 Control
        
        // VCOM Control
        addTranslation(DisplayDriver::ST7796, DisplayDriver::ILI9341, 0xC5, 0xC5);
        addTranslation(DisplayDriver::ST7796, DisplayDriver::ST7789, 0xC5, 0xC7);
        
        // Frame Rate Control
        addTranslation(DisplayDriver::ST7796, DisplayDriver::ILI9341, 0xB1, 0xB1);
        addTranslation(DisplayDriver::ST7796, DisplayDriver::ST7789, 0xB1, 0xB2);
        
        // Display Function Control
        addTranslation(DisplayDriver::ST7796, DisplayDriver::ILI9341, 0xB6, 0xB6);
        addTranslation(DisplayDriver::ST7796, DisplayDriver::ST7789, 0xB6, 0xB7);

        // ST7796S uses the same commands as ST7796
        addTranslation(DisplayDriver::ST7796S, DisplayDriver::ST7796, 0xC0, 0xC0);
        addTranslation(DisplayDriver::ST7796S, DisplayDriver::ST7796, 0xC1, 0xC1);
        addTranslation(DisplayDriver::ST7796S, DisplayDriver::ST7796, 0xC5, 0xC5);
        addTranslation(DisplayDriver::ST7796S, DisplayDriver::ST7796, 0xB1, 0xB1);
        addTranslation(DisplayDriver::ST7796S, DisplayDriver::ST7796, 0xB6, 0xB6);
    }

    void initializeST7789V2Translations() {
        // ST7789V2 and V3 are newer versions of ST7789 with some additional features
        // but maintain command compatibility
        
        // Frame Rate Control
        addTranslation(DisplayDriver::ST7789V2, DisplayDriver::ST7789, 0xB2, 0xB2);
        addTranslation(DisplayDriver::ST7789V3, DisplayDriver::ST7789, 0xB2, 0xB2);
        
        // Display Function Control
        addTranslation(DisplayDriver::ST7789V2, DisplayDriver::ST7789, 0xB6, 0xB6);
        addTranslation(DisplayDriver::ST7789V3, DisplayDriver::ST7789, 0xB6, 0xB6);
        
        // Power Control
        addTranslation(DisplayDriver::ST7789V2, DisplayDriver::ST7789, 0xBB, 0xBB);
        addTranslation(DisplayDriver::ST7789V3, DisplayDriver::ST7789, 0xBB, 0xBB);
        
        // VCOM Setting
        addTranslation(DisplayDriver::ST7789V2, DisplayDriver::ST7789, 0xC7, 0xC7);
        addTranslation(DisplayDriver::ST7789V3, DisplayDriver::ST7789, 0xC7, 0xC7);
        
        // LCM Control
        addTranslation(DisplayDriver::ST7789V2, DisplayDriver::ST7789, 0xC0, 0xC0);
        addTranslation(DisplayDriver::ST7789V3, DisplayDriver::ST7789, 0xC0, 0xC0);
        
        // VDV and VRH Command Enable
        addTranslation(DisplayDriver::ST7789V2, DisplayDriver::ST7789, 0xC2, 0xC2);
        addTranslation(DisplayDriver::ST7789V3, DisplayDriver::ST7789, 0xC2, 0xC2);
    }

    void initializeST7735RTranslations() {
        // ST7735R is a variant of ST7735 with some different initialization sequences
        // but mostly compatible command set
        
        // Frame Rate Control
        addTranslation(DisplayDriver::ST7735R, DisplayDriver::ST7735, 0xB1, 0xB1);
        addTranslation(DisplayDriver::ST7735R, DisplayDriver::ILI9341, 0xB1, 0xB1);
        
        // Display Function Control
        addTranslation(DisplayDriver::ST7735R, DisplayDriver::ST7735, 0xB6, 0xB6);
        addTranslation(DisplayDriver::ST7735R, DisplayDriver::ILI9341, 0xB6, 0xB6);
        
        // Power Control
        addTranslation(DisplayDriver::ST7735R, DisplayDriver::ST7735, 0xC0, 0xC0);
        addTranslation(DisplayDriver::ST7735R, DisplayDriver::ILI9341, 0xC0, 0xC0);
        addTranslation(DisplayDriver::ST7735R, DisplayDriver::ST7735, 0xC1, 0xC1);
        addTranslation(DisplayDriver::ST7735R, DisplayDriver::ILI9341, 0xC1, 0xC1);
        
        // VCOM Control
        addTranslation(DisplayDriver::ST7735R, DisplayDriver::ST7735, 0xC5, 0xC5);
        addTranslation(DisplayDriver::ST7735R, DisplayDriver::ILI9341, 0xC5, 0xC5);
    }

    // Helper method to add common translations for all supported displays
    void addCommonTranslation(uint8_t cmd, uint8_t translatedCmd) {
        for (auto srcDriver : {DisplayDriver::ILI9341, DisplayDriver::ST7789, DisplayDriver::ST7735, DisplayDriver::ILI9486, DisplayDriver::HX8357D, DisplayDriver::SSD1351, DisplayDriver::GC9A01, DisplayDriver::RM68140, DisplayDriver::R61581, DisplayDriver::SSD1963, DisplayDriver::ST7796, DisplayDriver::ST7789V2, DisplayDriver::ST7789V3, DisplayDriver::ST7735R, DisplayDriver::ST7796S}) {
            for (auto dstDriver : {DisplayDriver::ILI9341, DisplayDriver::ST7789, DisplayDriver::ST7735, DisplayDriver::ILI9486, DisplayDriver::HX8357D, DisplayDriver::SSD1351, DisplayDriver::GC9A01, DisplayDriver::RM68140, DisplayDriver::R61581, DisplayDriver::SSD1963, DisplayDriver::ST7796, DisplayDriver::ST7789V2, DisplayDriver::ST7789V3, DisplayDriver::ST7735R, DisplayDriver::ST7796S}) {
                if (srcDriver != dstDriver) {
                    _commandTranslationMap[cmd][dstDriver] = translatedCmd;
                }
            }
        }
    }

    // Helper method to add translation between specific display drivers
    void addTranslation(DisplayDriver srcDriver, DisplayDriver dstDriver, uint8_t srcCmd, uint8_t dstCmd) {
        _commandTranslationMap[srcCmd][dstDriver] = dstCmd;
        // Add reverse mapping if commands are different
        if (srcCmd != dstCmd) {
            _commandTranslationMap[dstCmd][srcDriver] = srcCmd;
        }
    }

    CommandSequence optimizeSequence(const CommandSequence& original) {
        CommandSequence optimized(original.name() + "_optimized");
        
        // Combine consecutive data writes
        std::vector<uint8_t> dataBuffer;
        
        for (const auto& cmd : original.commands()) {
            switch (cmd.type) {
                case CommandType::DATA:
                    dataBuffer.push_back(cmd.data[0]);
                    break;
                case CommandType::DATA_LIST:
                    dataBuffer.insert(dataBuffer.end(), cmd.data.begin(), cmd.data.end());
                    break;
                default:
                    // Flush data buffer if not empty
                    if (!dataBuffer.empty()) {
                        optimized.addDataList(dataBuffer);
                        dataBuffer.clear();
                    }
                    optimized.addCommand(cmd);
                    break;
            }
        }
        
        // Flush any remaining data
        if (!dataBuffer.empty()) {
            optimized.addDataList(dataBuffer);
        }
        
        return optimized;
    }

    void translateCommand(const Command& cmd, DisplayDriver targetDriver, CommandSequence& output) {
        if (cmd.type == CommandType::COMMAND) {
            auto& translationMap = _commandTranslationMap[cmd.data[0]];
            auto translatedCmd = translationMap.find(targetDriver);
            
            if (translatedCmd != translationMap.end()) {
                output.addCommand(translatedCmd->second);
            } else {
                // If no translation exists, keep original command
                output.addCommand(cmd);
            }
        } else if (cmd.type == CommandType::COMMAND_LIST) {
            std::vector<uint8_t> translatedCmds;
            for (uint8_t c : cmd.data) {
                auto& translationMap = _commandTranslationMap[c];
                auto translatedCmd = translationMap.find(targetDriver);
                
                translatedCmds.push_back(translatedCmd != translationMap.end() ? 
                    translatedCmd->second : c);
            }
            output.addCommandList(translatedCmds);
        }
    }

    void cacheSequenceIfNeeded(const std::string& name, const CommandSequence& sequence) {
        // Check if sequence is used frequently enough to cache
        auto it = _sequenceCache.find(name);
        if (it != _sequenceCache.end() && it->second.use_count >= CACHE_THRESHOLD) {
            it->second.sequence = sequence;
            it->second.last_used = millis();
            return;
        }

        // Add to cache if there's room or replace least recently used
        if (_sequenceCache.size() >= MAX_CACHE_SIZE) {
            auto lru = std::min_element(_sequenceCache.begin(), _sequenceCache.end(),
                [](const auto& a, const auto& b) {
                    return a.second.last_used < b.second.last_used;
                });
            _sequenceCache.erase(lru);
        }

        _sequenceCache[name] = {sequence, millis(), 1};
    }

    // Get current time in milliseconds (to be implemented based on platform)
    uint32_t millis() const {
        // Platform-specific implementation needed
        return 0;
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMAND_TRANSLATOR_H_
