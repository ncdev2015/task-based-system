#ifndef TASK_PROCESSOR_HPP
#define TASK_PROCESSOR_HPP

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "commands/command.hpp"
#include "user/manager.hpp"
#include "registry/registry.hpp"

class TaskProcessor {
private:
    UserManager userManager;
    CommandRegistry registry;

    void registerCommands();
    std::vector<std::string> readTaskFile(const std::string& filename);
    std::optional<Command> parseCommand(const std::string& line);

public:
    TaskProcessor();
    void processTask(const std::string& filename);
    void processTasks(const std::vector<std::string>& filenames);
};

#endif // TASK_PROCESSOR_HPP