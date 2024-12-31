/***************************************************
  TFT Command Set Abstraction
  
  This file provides structures and classes for managing
  display command sequences and initialization.
  
  Created: December 2024
 ****************************************************/

#ifndef _TFT_COMMANDS_H_
#define _TFT_COMMANDS_H_

#include <Arduino.h>
#include <vector>

namespace TFT_Runtime {

// Command types
enum class CommandType : uint8_t {
    COMMAND,        // Display command
    DATA,           // Data byte
    DATA16,         // 16-bit data
    DELAY,          // Delay in milliseconds
    END_SEQUENCE    // End of command sequence
};

// Command structure
struct Command {
    CommandType type;
    union {
        uint16_t value;      // Command value or data
        uint16_t delay_ms;   // Delay in milliseconds
    };
    
    // Constructors for different command types
    static Command cmd(uint8_t command) {
        return Command{CommandType::COMMAND, command};
    }
    
    static Command data(uint8_t data) {
        return Command{CommandType::DATA, data};
    }
    
    static Command data16(uint16_t data) {
        return Command{CommandType::DATA16, data};
    }
    
    static Command delay(uint16_t ms) {
        return Command{CommandType::DELAY, ms};
    }
    
    static Command end() {
        return Command{CommandType::END_SEQUENCE, 0};
    }
};

// Command sequence
class CommandSequence {
public:
    CommandSequence() = default;
    
    // Add commands to sequence
    void addCommand(uint8_t cmd) {
        commands.push_back(Command::cmd(cmd));
    }
    
    void addData(uint8_t data) {
        commands.push_back(Command::data(data));
    }
    
    void addData16(uint16_t data) {
        commands.push_back(Command::data16(data));
    }
    
    void addDelay(uint16_t ms) {
        commands.push_back(Command::delay(ms));
    }
    
    void endSequence() {
        commands.push_back(Command::end());
    }
    
    // Add multiple data bytes
    void addDataBytes(const uint8_t* data, size_t len) {
        commands.reserve(commands.size() + len);
        for (size_t i = 0; i < len; i++) {
            commands.push_back(Command::data(data[i]));
        }
    }
    
    // Add command with data
    void addCommandWithData(uint8_t cmd, const uint8_t* data, size_t len) {
        addCommand(cmd);
        addDataBytes(data, len);
    }
    
    // Access commands
    const std::vector<Command>& getCommands() const { return commands; }
    
    // Clear sequence
    void clear() { commands.clear(); }
    
    // Sequence validation
    bool validate() const {
        if (commands.empty()) return false;
        
        bool hasCommand = false;
        for (const auto& cmd : commands) {
            if (cmd.type == CommandType::COMMAND) {
                hasCommand = true;
            }
        }
        
        return hasCommand;
    }

private:
    std::vector<Command> commands;
};

// Command executor interface
class CommandExecutor {
public:
    virtual ~CommandExecutor() = default;
    
    // Execute a single command
    virtual void executeCommand(const Command& cmd) = 0;
    
    // Execute a sequence of commands
    virtual void executeSequence(const CommandSequence& sequence) {
        for (const auto& cmd : sequence.getCommands()) {
            executeCommand(cmd);
            if (cmd.type == CommandType::END_SEQUENCE) break;
        }
    }
    
protected:
    // Helper for command execution
    virtual void writeCommand(uint8_t cmd) = 0;
    virtual void writeData(uint8_t data) = 0;
    virtual void writeData16(uint16_t data) = 0;
};

} // namespace TFT_Runtime

#endif // _TFT_COMMANDS_H_
