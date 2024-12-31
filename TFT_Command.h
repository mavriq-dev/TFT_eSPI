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

// Command validation error codes
enum class CommandError {
    NONE,                   // No error
    EMPTY_SEQUENCE,         // Command sequence is empty
    NO_END_MARKER,         // Missing end marker
    INVALID_DATA_SIZE,      // Invalid data size for command/data
    EMPTY_DATA_LIST,        // Empty data list
    ZERO_DELAY,            // Zero delay value
    INVALID_COMMAND_TYPE,   // Invalid command type
    SEQUENCE_TOO_LONG,      // Sequence exceeds maximum length
    INVALID_COMMAND_VALUE,  // Command value out of valid range
    INVALID_DATA_VALUE      // Data value out of valid range
};

// Command validation result
struct ValidationResult {
    bool isValid;
    CommandError error;
    size_t errorIndex;      // Index of the command that caused the error
    std::string message;    // Detailed error message

    static ValidationResult success() {
        return {true, CommandError::NONE, 0, ""};
    }

    static ValidationResult failure(CommandError err, size_t index, const std::string& msg) {
        return {false, err, index, msg};
    }
};

// Command structure
struct Command {
    CommandType type;
    std::vector<uint8_t> data;
    uint16_t delay_ms;
    std::string description;  // Optional description for debugging

    // Validation constants
    static constexpr size_t MAX_DATA_SIZE = 256;
    static constexpr uint16_t MAX_DELAY_MS = 1000;

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

    // Validate individual command
    ValidationResult validate() const {
        switch (type) {
            case CommandType::COMMAND:
            case CommandType::DATA:
                if (data.size() != 1) {
                    return ValidationResult::failure(
                        CommandError::INVALID_DATA_SIZE, 
                        0, 
                        "Single command/data must have exactly one byte"
                    );
                }
                break;

            case CommandType::COMMAND_LIST:
            case CommandType::DATA_LIST:
                if (data.empty()) {
                    return ValidationResult::failure(
                        CommandError::EMPTY_DATA_LIST,
                        0,
                        "Command/data list cannot be empty"
                    );
                }
                if (data.size() > MAX_DATA_SIZE) {
                    return ValidationResult::failure(
                        CommandError::SEQUENCE_TOO_LONG,
                        0,
                        "Command/data list exceeds maximum size"
                    );
                }
                break;

            case CommandType::DELAY:
                if (delay_ms == 0) {
                    return ValidationResult::failure(
                        CommandError::ZERO_DELAY,
                        0,
                        "Delay value cannot be zero"
                    );
                }
                if (delay_ms > MAX_DELAY_MS) {
                    return ValidationResult::failure(
                        CommandError::INVALID_DATA_VALUE,
                        0,
                        "Delay value exceeds maximum allowed"
                    );
                }
                break;

            case CommandType::END:
                break;

            default:
                return ValidationResult::failure(
                    CommandError::INVALID_COMMAND_TYPE,
                    0,
                    "Invalid command type"
                );
        }
        return ValidationResult::success();
    }
};

// Command sequence class
class CommandSequence {
public:
    static constexpr size_t MAX_SEQUENCE_LENGTH = 1024;

    CommandSequence(const std::string& name = "") : _name(name) {}

    // Add commands to sequence with validation
    bool addCommand(const Command& cmd) {
        if (_commands.size() >= MAX_SEQUENCE_LENGTH) {
            _lastError = ValidationResult::failure(
                CommandError::SEQUENCE_TOO_LONG,
                _commands.size(),
                "Sequence exceeds maximum length"
            );
            return false;
        }

        auto result = cmd.validate();
        if (!result.isValid) {
            _lastError = result;
            return false;
        }

        _commands.push_back(cmd);
        return true;
    }

    bool addCommand(uint8_t cmd, const std::string& desc = "") {
        return addCommand(Command::makeCommand(cmd, desc));
    }

    bool addData(uint8_t data, const std::string& desc = "") {
        return addCommand(Command::makeData(data, desc));
    }

    bool addCommandList(const std::vector<uint8_t>& cmds, const std::string& desc = "") {
        return addCommand(Command::makeCommandList(cmds, desc));
    }

    bool addDataList(const std::vector<uint8_t>& data, const std::string& desc = "") {
        return addCommand(Command::makeDataList(data, desc));
    }

    bool addDelay(uint16_t ms, const std::string& desc = "") {
        return addCommand(Command::makeDelay(ms, desc));
    }

    bool addEnd() {
        return addCommand(Command::makeEnd());
    }

    // Access commands and status
    const std::vector<Command>& commands() const { return _commands; }
    const std::string& name() const { return _name; }
    const ValidationResult& lastError() const { return _lastError; }

    // Validation
    ValidationResult validate() const {
        if (_commands.empty()) {
            return ValidationResult::failure(
                CommandError::EMPTY_SEQUENCE,
                0,
                "Command sequence is empty"
            );
        }

        if (_commands.back().type != CommandType::END) {
            return ValidationResult::failure(
                CommandError::NO_END_MARKER,
                _commands.size() - 1,
                "Missing end marker"
            );
        }

        for (size_t i = 0; i < _commands.size(); i++) {
            auto result = _commands[i].validate();
            if (!result.isValid) {
                return ValidationResult::failure(
                    result.error,
                    i,
                    "Invalid command at index " + std::to_string(i) + ": " + result.message
                );
            }
        }

        return ValidationResult::success();
    }

private:
    std::string _name;
    std::vector<Command> _commands;
    ValidationResult _lastError{true, CommandError::NONE, 0, ""};
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
    
    virtual bool executeCommand(uint8_t cmd) = 0;
    virtual bool executeData(uint8_t data) = 0;
    virtual bool executeDelay(uint16_t ms) = 0;
    
    // Execute a full command sequence with error handling
    virtual bool executeSequence(const CommandSequence& sequence) {
        auto validation = sequence.validate();
        if (!validation.isValid) {
            _lastError = validation;
            return false;
        }

        for (size_t i = 0; i < sequence.commands().size(); i++) {
            const auto& cmd = sequence.commands()[i];
            bool success = true;

            switch (cmd.type) {
                case CommandType::COMMAND:
                    success = executeCommand(cmd.data[0]);
                    break;
                case CommandType::DATA:
                    success = executeData(cmd.data[0]);
                    break;
                case CommandType::COMMAND_LIST:
                    for (uint8_t c : cmd.data) {
                        if (!executeCommand(c)) {
                            success = false;
                            break;
                        }
                    }
                    break;
                case CommandType::DATA_LIST:
                    for (uint8_t d : cmd.data) {
                        if (!executeData(d)) {
                            success = false;
                            break;
                        }
                    }
                    break;
                case CommandType::DELAY:
                    success = executeDelay(cmd.delay_ms);
                    break;
                case CommandType::END:
                    break;
            }

            if (!success) {
                _lastError = ValidationResult::failure(
                    CommandError::INVALID_COMMAND_VALUE,
                    i,
                    "Failed to execute command at index " + std::to_string(i)
                );
                return false;
            }
        }

        return true;
    }

    const ValidationResult& lastError() const { return _lastError; }

protected:
    ValidationResult _lastError{true, CommandError::NONE, 0, ""};
};

} // namespace TFT_Runtime

#endif // _TFT_COMMAND_H_
