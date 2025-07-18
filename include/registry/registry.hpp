#ifndef REGISTRY_REGISTRY_HPP  // Recommended to match path structure
#define REGISTRY_REGISTRY_HPP

// Standard Library
#include <memory>              // For std::unique_ptr
#include <typeindex>          // For std::type_index
#include <typeinfo>           // For typeid()
#include <unordered_map>      // For std::unordered_map
#include <utility>            // For std::move

// Project Headers
#include "commands/command.hpp"       // For Command type
#include "commands/executor.hpp"      // For CommandExecutor
#include "user/manager.hpp"           // For UserManager

// Command registry for extensibility
class CommandRegistry {
private:
    std::unordered_map<std::type_index, std::unique_ptr<CommandExecutor>> executors;

public:
    template<typename T>
    void registerExecutor(std::unique_ptr<CommandExecutor> executor) {
        executors[std::type_index(typeid(T))] = std::move(executor);
    }

    CommandResult execute(const Command& cmd, UserManager& userManager) {
        return std::visit([&](const auto& command) -> CommandResult {
            using T = std::decay_t<decltype(command)>;
            auto it = executors.find(std::type_index(typeid(T)));
            if (it != executors.end()) {
                return it->second->execute(command, userManager);
            }
            return {false, "❌ Unknown command"};
        }, cmd);
    }
};

#endif // REGISTRY_REGISTRY_HPP
