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
    CommandResult execute(const Command& cmd, UserManager& userManager) override;
};

class DeleteUserExecutor : public CommandExecutor {
public:
    CommandResult execute(const Command& cmd, UserManager& userManager) override;
};

class DisableUserExecutor : public CommandExecutor {
public:
    CommandResult execute(const Command& cmd, UserManager& userManager) override;
};

class SendMessageExecutor : public CommandExecutor {
public:
    CommandResult execute(const Command& cmd, UserManager& userManager) override;
};

class PingExecutor : public CommandExecutor {
public:
    CommandResult execute(const Command& cmd, UserManager& userManager) override;
};

class AddUserToGroupExecutor : public CommandExecutor {
public:
    CommandResult execute(const Command& cmd, UserManager& userManager) override;
};

class RemoveUserFromGroupExecutor : public CommandExecutor {
public:
    CommandResult execute(const Command& cmd, UserManager& userManager) override;
};

class GetUsersExecutor : public CommandExecutor {
public:
    CommandResult execute(const Command& cmd, UserManager& userManager) override;
};

class GetGroupsExecutor : public CommandExecutor {
public:
    CommandResult execute(const Command& cmd, UserManager& userManager) override;
};

class GetMessageHistoryExecutor : public CommandExecutor {
public:
    CommandResult execute(const Command& cmd, UserManager& userManager) override;
};

class ExitExecutor : public CommandExecutor {
public:
    CommandResult execute(const Command& cmd, UserManager& userManager) override;
};

#endif // COMMANDS_EXECUTOR_HPP