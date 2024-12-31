#ifndef _TFT_COMMAND_H_
#define _TFT_COMMAND_H_

#include <vector>
#include <cstdint>
#include <string>
#include <memory>

namespace TFT_Runtime {

// Command type definitions
enum class CommandType {
    COMMAND,            // Single command byte
    DATA,              // Single data byte
    COMMAND_LIST,      // Multiple command bytes
    DATA_LIST,         // Multiple data bytes
    DELAY,             // Delay in milliseconds
    END               // End of command sequence
};

// Command structure
struct Command {
    CommandType type;
    std::vector<uint8_t> data;
    uint16_t delay_ms;
    std::string description;  // Optional description for debugging

    // Constructors for different command types
    static Command makeCommand(uint8_t cmd, const std::string& desc = "") {
        return Command{CommandType::COMMAND, {cmd}, 0, desc};
    }

    static Command makeData(uint8_t data, const std::string& desc = "") {
        return Command{CommandType::DATA, {data}, 0, desc};
    }

    static Command makeCommandList(const std::vector<uint8_t>& cmds, const std::string& desc = "") {
        return Command{CommandType::COMMAND_LIST, cmds, 0, desc};
    }

    static Command makeDataList(const std::vector<uint8_t>& data, const std::string& desc = "") {
        return Command{CommandType::DATA_LIST, data, 0, desc};
    }

    static Command makeDelay(uint16_t ms, const std::string& desc = "") {
        return Command{CommandType::DELAY, {}, ms, desc};
    }

    static Command makeEnd() {
        return Command{CommandType::END, {}, 0, "End of sequence"};
    }
};

// Command sequence class
class CommandSequence {
public:
    CommandSequence(const std::string& name = "") : _name(name) {}

    // Add commands to sequence
    void addCommand(const Command& cmd) {
        _commands.push_back(cmd);
    }

    void addCommand(uint8_t cmd, const std::string& desc = "") {
        _commands.push_back(Command::makeCommand(cmd, desc));
    }

    void addData(uint8_t data, const std::string& desc = "") {
        _commands.push_back(Command::makeData(data, desc));
    }

    void addCommandList(const std::vector<uint8_t>& cmds, const std::string& desc = "") {
        _commands.push_back(Command::makeCommandList(cmds, desc));
    }

    void addDataList(const std::vector<uint8_t>& data, const std::string& desc = "") {
        _commands.push_back(Command::makeDataList(data, desc));
    }

    void addDelay(uint16_t ms, const std::string& desc = "") {
        _commands.push_back(Command::makeDelay(ms, desc));
    }

    void addEnd() {
        _commands.push_back(Command::makeEnd());
    }

    // Access commands
    const std::vector<Command>& commands() const { return _commands; }
    const std::string& name() const { return _name; }

    // Validation
    bool validate() const {
        if (_commands.empty()) return false;
        if (_commands.back().type != CommandType::END) return false;
        
        for (const auto& cmd : _commands) {
            switch (cmd.type) {
                case CommandType::COMMAND:
                case CommandType::DATA:
                    if (cmd.data.size() != 1) return false;
                    break;
                case CommandType::COMMAND_LIST:
                case CommandType::DATA_LIST:
                    if (cmd.data.empty()) return false;
                    break;
                case CommandType::DELAY:
                    if (cmd.delay_ms == 0) return false;
                    break;
                case CommandType::END:
                    break;
                default:
                    return false;
            }
        }
        return true;
    }

private:
    std::string _name;
    std::vector<Command> _commands;
};

// Command sequence factory
class CommandSequenceFactory {
public:
    virtual ~CommandSequenceFactory() = default;
    
    // Create standard command sequences
    virtual CommandSequence createInitSequence() = 0;
    virtual CommandSequence createSleepSequence() = 0;
    virtual CommandSequence createWakeSequence() = 0;
    virtual CommandSequence createDisplayOnSequence() = 0;
    virtual CommandSequence createDisplayOffSequence() = 0;
    virtual CommandSequence createInvertOnSequence() = 0;
    virtual CommandSequence createInvertOffSequence() = 0;
    
    // Optional sequences that may not be supported by all displays
    virtual CommandSequence createPartialModeSequence() { return CommandSequence(); }
    virtual CommandSequence createNormalModeSequence() { return CommandSequence(); }
    virtual CommandSequence createIdleModeSequence() { return CommandSequence(); }
    virtual CommandSequence createResetGammaSequence() { return CommandSequence(); }
};

// Command executor interface
class CommandExecutor {
public:
    virtual ~CommandExecutor() = default;
    
    virtual void executeCommand(uint8_t cmd) = 0;
    virtual void executeData(uint8_t data) = 0;
    virtual void executeDelay(uint16_t ms) = 0;
    
    // Execute a full command sequence
    virtual void executeSequence(const CommandSequence& sequence) {
        for (const auto& cmd : sequence.commands()) {
            switch (cmd.type) {
                case CommandType::COMMAND:
                    executeCommand(cmd.data[0]);
                    break;
                case CommandType::DATA:
                    executeData(cmd.data[0]);
                    break;
                case CommandType::COMMAND_LIST:
                    for (uint8_t c : cmd.data) executeCommand(c);
                    break;
                case CommandType::DATA_LIST:
                    for (uint8_t d : cmd.data) executeData(d);
                    break;
                case CommandType::DELAY:
                    executeDelay(cmd.delay_ms);
                    break;
                case CommandType::END:
                    break;
            }
        }
    }
};

} // namespace TFT_Runtime

#endif // _TFT_COMMAND_H_
