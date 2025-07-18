#ifndef TASK_PROCESSOR_HPP
#define TASK_PROCESSOR_HPP

// Standard Library
#include <algorithm>           // For std::find_if, string trimming
#include <cctype>              // For std::isspace
#include <exception>           // For std::exception
#include <filesystem>          // For path handling (optional)
#include <fstream>             // For file I/O
#include <iostream>            // For std::cout
#include <memory>              // For std::make_unique
#include <optional>            // For std::optional
#include <string>             
#include <string_view>         // For efficient string handling
#include <vector>
#include <stdexcept>

// Third-party
#include <fmt/format.h>        // For string formatting

// Project Headers
#include "commands/command.hpp"         // For Command type
#include "commands/executor.hpp"        // For CommandExecutor implementations
#include "parser/parser.hpp"            // For CommandParser
#include "registry/registry.hpp"        // For CommandRegistry
#include "user/manager.hpp"             // For UserManager

// Task processor
class TaskProcessor {
private:
    UserManager userManager;
    CommandRegistry registry;

    void registerCommands() {
        registry.registerExecutor<CreateUserCommand>(std::make_unique<CreateUserExecutor>());
        registry.registerExecutor<DeleteUserCommand>(std::make_unique<DeleteUserExecutor>());
        registry.registerExecutor<DisableUserCommand>(std::make_unique<DisableUserExecutor>());
        registry.registerExecutor<SendMessageCommand>(std::make_unique<SendMessageExecutor>());
        registry.registerExecutor<PingCommand>(std::make_unique<PingExecutor>());
        registry.registerExecutor<AddUserToGroupCommand>(std::make_unique<AddUserToGroupExecutor>());
        registry.registerExecutor<RemoveUserFromGroupCommand>(std::make_unique<RemoveUserFromGroupExecutor>());
        registry.registerExecutor<GetUsersCommand>(std::make_unique<GetUsersExecutor>());
        registry.registerExecutor<GetGroupsCommand>(std::make_unique<GetGroupsExecutor>());
        registry.registerExecutor<GetMessageHistoryCommand>(std::make_unique<GetMessageHistoryExecutor>());
        registry.registerExecutor<ExitCommand>(std::make_unique<ExitExecutor>());
    }

    std::vector<std::string> readTaskFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error(fmt::format("Cannot open file: {}", filename));
        }
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(file, line)) {
            // Remove comments
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }

            // Trim whitespace (including \r for cross-platform compatibility)
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);

            if (!line.empty()) {
                lines.push_back(line);
            }
        }
        return lines;
    }

    std::optional<Command> parseCommand(const std::string& line) {
        auto parser = CommandParser::commandParser();
        auto result = parser(line, 0);
        
        if (result.success() && result.index() == line.length()) {
            return result.value();
        }
        return std::nullopt;
    }

public:
    TaskProcessor() {
        registerCommands();
    }

    void processTask(const std::string& filename) {
        userManager.reset();
        
        std::cout << fmt::format("[Processing task: {}]\n", filename);
        
        try {
            auto lines = readTaskFile(filename);
            
            for (const auto& line : lines) {
                auto cmdOpt = parseCommand(line);
                if (!cmdOpt) {
                    std::cout << fmt::format("❌ Invalid command: {}\n", line);
                    std::cout << fmt::format("[Task {} stopped due to failure]\n\n", filename);
                    return;
                }
                
                auto result = registry.execute(*cmdOpt, userManager);
                std::cout << result.message << "\n";
                
                if (result.shouldExit) {
                    break;
                }
                
                if (!result.success) {
                    std::cout << fmt::format("[Task {} stopped due to failure]\n\n", filename);
                    return;
                }
            }
            
            std::cout << fmt::format("[Task {} completed successfully]\n\n", filename);
            
        } catch (const std::exception& e) {
            std::cout << fmt::format("❌ Error processing task {}: {}\n", filename, e.what());
            std::cout << fmt::format("[Task {} stopped due to failure]\n\n", filename);
        }
    }

    void processTasks(const std::vector<std::string>& filenames) {
        for (const auto& filename : filenames) {
            processTask(filename);
        }
    }
};

#endif // TASK_PROCESSOR_HPP
