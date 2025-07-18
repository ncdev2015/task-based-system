#ifndef COMMANDS_EXECUTOR_HPP
#define COMMANDS_EXECUTOR_HPP

#include "commands/command.hpp"
#include "user/manager.hpp"
#include <fmt/format.h>
#include <string>
#include <memory>
#include <typeindex>

#include "result.hpp"

// Forward declarations
class UserManager;

// Base command executor interface
class CommandExecutor {
public:
    virtual ~CommandExecutor() = default;
    virtual CommandResult execute(const Command& cmd, UserManager& userManager) = 0;
};

// Command executors
class CreateUserExecutor : public CommandExecutor {
public:
    CommandResult execute(const Command& cmd, UserManager& userManager) override {
        const auto& createCmd = std::get<CreateUserCommand>(cmd);
        if (userManager.createUser(createCmd.username)) {
            return {true, fmt::format("✅ CREATE USER {}", createCmd.username)};
        }
        return {false, fmt::format("❌ CREATE USER {} (Failed: User already exists)", createCmd.username)};
    }
};

class DeleteUserExecutor : public CommandExecutor {
public:
    CommandResult execute(const Command& cmd, UserManager& userManager) override {
        const auto& deleteCmd = std::get<DeleteUserCommand>(cmd);
        if (userManager.deleteUser(deleteCmd.username)) {
            return {true, fmt::format("✅ DELETE USER {}", deleteCmd.username)};
        }
        return {false, fmt::format("❌ DELETE USER {} (Failed: User does not exist)", deleteCmd.username)};
    }
};

class DisableUserExecutor : public CommandExecutor {
public:
    CommandResult execute(const Command& cmd, UserManager& userManager) override {
        const auto& disableCmd = std::get<DisableUserCommand>(cmd);
        if (userManager.disableUser(disableCmd.username)) {
            return {true, fmt::format("✅ DISABLE USER {}", disableCmd.username)};
        }
        return {false, fmt::format("❌ DISABLE USER {} (Failed: User does not exist)", disableCmd.username)};
    }
};

class SendMessageExecutor : public CommandExecutor {
public:
    CommandResult execute(const Command& cmd, UserManager& userManager) override {
        const auto& sendCmd = std::get<SendMessageCommand>(cmd);
        if (userManager.sendMessage(sendCmd.username, sendCmd.message)) {
            return {true, fmt::format("✅ SEND MESSAGE {} \"{}\"", sendCmd.username, sendCmd.message)};
        }
        return {false, fmt::format("❌ SEND MESSAGE {} \"{}\" (Failed: User does not exist)", sendCmd.username, sendCmd.message)};
    }
};

class PingExecutor : public CommandExecutor {
public:
    CommandResult execute(const Command& cmd, UserManager& userManager) override {
        const auto& pingCmd = std::get<PingCommand>(cmd);
        std::string result = fmt::format("✅ Send ping to {} ({}):\n", pingCmd.username, pingCmd.times);
        
        for (int i = 0; i < pingCmd.times; ++i) {
            result += fmt::format("Sent ping to {}\n", pingCmd.username);
            if (userManager.userExists(pingCmd.username)) {
                result += fmt::format("{} received a ping\n", pingCmd.username);
            }
        }
        
        return {true, result};
    }
};

class AddUserToGroupExecutor : public CommandExecutor {
public:
    CommandResult execute(const Command& cmd, UserManager& userManager) override {
        const auto& addCmd = std::get<AddUserToGroupCommand>(cmd);
        if (userManager.addUserToGroup(addCmd.username, addCmd.group)) {
            return {true, fmt::format("✅ ADD USER {} TO GROUP {}", addCmd.username, addCmd.group)};
        }
        return {false, fmt::format("❌ ADD USER {} TO GROUP {} (Failed: User does not exist)", addCmd.username, addCmd.group)};
    }
};

class RemoveUserFromGroupExecutor : public CommandExecutor {
public:
    CommandResult execute(const Command& cmd, UserManager& userManager) override {
        const auto& removeCmd = std::get<RemoveUserFromGroupCommand>(cmd);
        if (userManager.removeUserFromGroup(removeCmd.username, removeCmd.group)) {
            return {true, fmt::format("✅ REMOVE USER {} FROM GROUP {}", removeCmd.username, removeCmd.group)};
        }
        return {false, fmt::format("❌ REMOVE USER {} FROM GROUP {} (Failed: User does not exist)", removeCmd.username, removeCmd.group)};
    }
};

class GetUsersExecutor : public CommandExecutor {
public:
    CommandResult execute([[maybe_unused]] const Command& cmd, UserManager& userManager) override {
        auto users = userManager.getUsers();
        std::string result = "✅ GET USERS\nUsers: ";
        if (users.empty()) {
            result += "(none)";
        } else {
            for (size_t i = 0; i < users.size(); ++i) {
                if (i > 0) result += ", ";
                result += users[i];
            }
        }
        return {true, result};
    }
};

class GetGroupsExecutor : public CommandExecutor {
public:
    CommandResult execute([[maybe_unused]] const Command& cmd, UserManager& userManager) override {
        auto groups = userManager.getGroups();
        std::string result = "✅ GET GROUPS\nGroups: ";
        if (groups.empty()) {
            result += "(none)";
        } else {
            for (size_t i = 0; i < groups.size(); ++i) {
                if (i > 0) result += ", ";
                result += groups[i];
            }
        }
        return {true, result};
    }
};

class GetMessageHistoryExecutor : public CommandExecutor {
public:
    CommandResult execute(const Command& cmd, UserManager& userManager) override {
        const auto& historyCmd = std::get<GetMessageHistoryCommand>(cmd);
        if (!userManager.userExists(historyCmd.username)) {
            return {false, fmt::format("❌ GET MESSAGE HISTORY {} (Failed: User does not exist)", historyCmd.username)};
        }
        
        auto messages = userManager.getMessageHistory(historyCmd.username);
        std::string result = fmt::format("✅ GET MESSAGE HISTORY {}\nMessages: ", historyCmd.username);
        if (messages.empty()) {
            result += "(none)";
        } else {
            for (size_t i = 0; i < messages.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + messages[i] + "\"";
            }
        }
        return {true, result};
    }
};

class ExitExecutor : public CommandExecutor {
public:
    CommandResult execute([[maybe_unused]] const Command& cmd, [[maybe_unused]] UserManager& userManager) override {
        return {true, "✅ EXIT", true};
    }
};

#endif // COMMANDS_EXECUTOR_HPP
