#ifndef COMMANDS_COMMAND_HPP
#define COMMANDS_COMMAND_HPP

#include <string>
#include <variant>
#include <cstdint> // For fixed size integers as int32_t

// Command structures

/// @brief Command to create a new user
struct CreateUserCommand {
    std::string username;
};

/// @brief Command to delete an existing user
struct DeleteUserCommand {
    std::string username;
};

/// @brief Command to disable a user
struct DisableUserCommand {
    std::string username;
};

/// @brief Command to send a message to a user
struct SendMessageCommand {
    std::string username;
    std::string message;
};

/// @brief Command to ping a user multiple times
struct PingCommand {
    std::string username;
    int32_t times;  // Tipo más explícito para el número de veces
};

/// @brief Command to add user to a group
struct AddUserToGroupCommand {
    std::string username;
    std::string group;
};

/// @brief Command to remove user from a group
struct RemoveUserFromGroupCommand {
    std::string username;
    std::string group;
};

/// @brief Command to list all users
struct GetUsersCommand {};

/// @brief Command to list all groups
struct GetGroupsCommand {};

/// @brief Command to get message history for a user
struct GetMessageHistoryCommand {
    std::string username;
};

/// @brief Command to exit the application
struct ExitCommand {};

// Command variant
using Command = std::variant<
    CreateUserCommand,
    DeleteUserCommand,
    DisableUserCommand,
    SendMessageCommand,
    PingCommand,
    AddUserToGroupCommand,
    RemoveUserFromGroupCommand,
    GetUsersCommand,
    GetGroupsCommand,
    GetMessageHistoryCommand,
    ExitCommand
>;

#endif // COMMANDS_COMMAND_HPP
