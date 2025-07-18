#ifndef COMMANDS_RESULT_HPP
#define COMMANDS_RESULT_HPP

// Command execution result
struct CommandResult {
    bool success;
    std::string message;
    bool shouldExit = false;
};

#endif // COMMANDS_RESULT_HPP
