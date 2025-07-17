#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <cctype>
#include <variant>
#include <string_view>
#include <filesystem>
#include <algorithm>
#include <typeindex>

#include <fmt/format.h>
#include <parsec/parsec.hpp>

// Forward declarations
class UserManager;
class TaskProcessor;

// Command structures
struct CreateUserCommand {
    std::string username;
};

struct DeleteUserCommand {
    std::string username;
};

struct DisableUserCommand {
    std::string username;
};

struct SendMessageCommand {
    std::string username;
    std::string message;
};

struct PingCommand {
    std::string username;
    int times;
};

struct AddUserToGroupCommand {
    std::string username;
    std::string group;
};

struct RemoveUserFromGroupCommand {
    std::string username;
    std::string group;
};

struct GetUsersCommand {};
struct GetGroupsCommand {};

struct GetMessageHistoryCommand {
    std::string username;
};

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

// User data structures
struct User {
    std::string username;
    bool enabled = true;
    std::vector<std::string> messages;
    std::unordered_set<std::string> groups;
};

// Command execution result
struct CommandResult {
    bool success;
    std::string message;
    bool shouldExit = false;
};

// Base command executor interface
class CommandExecutor {
public:
    virtual ~CommandExecutor() = default;
    virtual CommandResult execute(const Command& cmd, UserManager& userManager) = 0;
};

// User Manager class
class UserManager {
private:
    std::unordered_map<std::string, std::unique_ptr<User>> users;
    std::unordered_set<std::string> groups;
public:
    void reset() {
        users.clear();
        groups.clear();
    }
    
    bool createUser(const std::string& username) {
        if (users.find(username) != users.end()) {
            return false; // User already exists
        }
        users[username] = std::make_unique<User>();
        users[username]->username = username;
        return true;
    }
    
    bool deleteUser(const std::string& username) {
        auto it = users.find(username);
        if (it == users.end()) {
            return false; // User doesn't exist
        }
        // Remove user from all groups
        for (const auto& group : it->second->groups) {
            groups.erase(group);
        }
        users.erase(it);
        return true;
    }
    
    bool disableUser(const std::string& username) {
        auto it = users.find(username);
        if (it == users.end()) {
            return false; // User doesn't exist
        }
        it->second->enabled = false;
        return true;
    }
    
    bool userExists(const std::string& username) const {
        return users.find(username) != users.end();
    }
    
    bool isUserEnabled(const std::string& username) const {
        auto it = users.find(username);
        return it != users.end() && it->second->enabled;
    }
    
    bool sendMessage(const std::string& username, const std::string& message) {
        auto it = users.find(username);
        if (it == users.end() || !it->second->enabled) {
            return false;
        }
        it->second->messages.push_back(message);
        return true;
    }
    
    bool addUserToGroup(const std::string& username, const std::string& group) {
        auto it = users.find(username);
        if (it == users.end()) {
            return false;
        }
        it->second->groups.insert(group);
        groups.insert(group);
        return true;
    }
    
    bool removeUserFromGroup(const std::string& username, const std::string& group) {
        auto it = users.find(username);
        if (it == users.end()) {
            return false;
        }
        it->second->groups.erase(group);
        
        // Check if group is empty
        bool groupEmpty = true;
        for (const auto& [_, user] : users) {
            if (user->groups.find(group) != user->groups.end()) {
                groupEmpty = false;
                break;
            }
        }
        if (groupEmpty) {
            groups.erase(group);
        }
        return true;
    }
    
    std::vector<std::string> getUsers() const {
        std::vector<std::string> userList;
        for (const auto& [username, user] : users) {
            userList.push_back(username);
        }
        std::sort(userList.begin(), userList.end());
        return userList;
    }
    
    std::vector<std::string> getGroups() const {
        std::vector<std::string> groupList(groups.begin(), groups.end());
        std::sort(groupList.begin(), groupList.end());
        return groupList;
    }
    
    std::vector<std::string> getMessageHistory(const std::string& username) const {
        auto it = users.find(username);
        if (it == users.end()) {
            return {};
        }
        return it->second->messages;
    }
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
    CommandResult execute(const Command& cmd, UserManager& userManager) override {
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
    CommandResult execute(const Command& cmd, UserManager& userManager) override {
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
    CommandResult execute(const Command& cmd, UserManager& userManager) override {
        return {true, "✅ EXIT", true};
    }
};

// Parser implementation
class CommandParser {
private:
    // Basic parsers
    static parsec::Parser<char> character(char c) {
        return [c](std::string_view s, size_t i) {
            if (i >= s.size()) {
                return parsec::makeError<char>(fmt::format("Expected '{}' but reached end of input", c), i);
            }
            if (s[i] == c) {
                return parsec::makeSuccess<char>(char(s[i]), i + 1);
            }
            return parsec::makeError<char>(fmt::format("Expected '{}' but found '{}'", c, s[i]), i);
        };
    }

    static parsec::Parser<std::string> whitespace() {
        return [](std::string_view s, size_t i) {
            size_t start = i;
            while (i < s.size() && std::isspace(s[i])) {
                ++i;
            }
            if (i == start) {
                return parsec::makeError<std::string>("Expected whitespace", i);
            }
            return parsec::makeSuccess<std::string>(std::string(s.substr(start, i - start)), i);
        };
    }

    static parsec::Parser<std::string> identifier() {
        return [](std::string_view s, size_t i) {
            size_t start = i;
            if (i >= s.size() || !std::isalpha(s[i])) {
                return parsec::makeError<std::string>("Expected identifier", i);
            }
            while (i < s.size() && (std::isalnum(s[i]) || s[i] == '_')) {
                ++i;
            }
            return parsec::makeSuccess<std::string>(std::string(s.substr(start, i - start)), i);
        };
    }

    static parsec::Parser<std::string> quotedString() {
        return [](std::string_view s, size_t i) {
            if (i >= s.size() || s[i] != '"') {
                return parsec::makeError<std::string>("Expected quoted string", i);
            }
            ++i; // Skip opening quote
            size_t start = i;
            while (i < s.size() && s[i] != '"') {
                ++i;
            }
            if (i >= s.size()) {
                return parsec::makeError<std::string>("Unterminated quoted string", i);
            }
            std::string result(s.substr(start, i - start));
            ++i; // Skip closing quote
            return parsec::makeSuccess<std::string>(std::move(result), i);
        };
    }

    static parsec::Parser<int> number() {
        return [](std::string_view s, size_t i) {
            size_t start = i;
            if (i >= s.size() || !std::isdigit(s[i])) {
                return parsec::makeError<int>("Expected number", i);
            }
            while (i < s.size() && std::isdigit(s[i])) {
                ++i;
            }
            std::string numStr(s.substr(start, i - start));
            return parsec::makeSuccess<int>(std::stoi(numStr), i);
        };
    }

    static parsec::Parser<std::string> keyword(const std::string& word) {
        return [word](std::string_view s, size_t i) {
            if (i + word.length() > s.size()) {
                return parsec::makeError<std::string>(fmt::format("Expected '{}'", word), i);
            }
            if (s.substr(i, word.length()) == word) {
                return parsec::makeSuccess<std::string>(std::string(word), i + word.length());
            }
            return parsec::makeError<std::string>(fmt::format("Expected '{}'", word), i);
        };
    }

public:
    static parsec::Parser<Command> commandParser() {
        return createUserParser() | 
               deleteUserParser() | 
               disableUserParser() | 
               sendMessageParser() | 
               pingParser() | 
               addUserToGroupParser() | 
               removeUserFromGroupParser() | 
               getUsersParser() | 
               getGroupsParser() | 
               getMessageHistoryParser() | 
               exitParser();
    }

private:
    static parsec::Parser<Command> createUserParser() {
        return parsec::fmap<Command, std::string>(
            [](const std::string& username) -> Command {
                return CreateUserCommand{username};
            },
            keyword("CREATE") >> whitespace() >> keyword("USER") >> whitespace() >> identifier()
        );
    }

    static parsec::Parser<Command> deleteUserParser() {
        return parsec::fmap<Command, std::string>(
            [](const std::string& username) -> Command {
                return DeleteUserCommand{username};
            },
            keyword("DELETE") >> whitespace() >> keyword("USER") >> whitespace() >> identifier()
        );
    }

    static parsec::Parser<Command> disableUserParser() {
        return parsec::fmap<Command, std::string>(
            [](const std::string& username) -> Command {
                return DisableUserCommand{username};
            },
            keyword("DISABLE") >> whitespace() >> keyword("USER") >> whitespace() >> identifier()
        );
    }

    static parsec::Parser<Command> sendMessageParser() {
        return parsec::fmap<Command, std::tuple<std::string, std::string>>(
            [](const std::tuple<std::string, std::string>& params) -> Command {
                return SendMessageCommand{std::get<0>(params), std::get<1>(params)};
            },
            (keyword("SEND") >> whitespace() >> keyword("MESSAGE") >> whitespace() >> identifier()) & 
            (whitespace() >> quotedString())
        );
    }

    static parsec::Parser<Command> pingParser() {
        return parsec::fmap<Command, std::tuple<std::string, int>>(
            [](const std::tuple<std::string, int>& params) -> Command {
                return PingCommand{std::get<0>(params), std::get<1>(params)};
            },
            (keyword("PING") >> whitespace() >> identifier()) & (whitespace() >> number())
        );
    }

    static parsec::Parser<Command> addUserToGroupParser() {
        return parsec::fmap<Command, std::tuple<std::string, std::string>>(
            [](const std::tuple<std::string, std::string>& params) -> Command {
                return AddUserToGroupCommand{std::get<0>(params), std::get<1>(params)};
            },
            (keyword("ADD") >> whitespace() >> keyword("USER") >> whitespace() >> identifier()) & 
            (whitespace() >> keyword("TO") >> whitespace() >> keyword("GROUP") >> whitespace() >> identifier())
        );
    }

    static parsec::Parser<Command> removeUserFromGroupParser() {
        return parsec::fmap<Command, std::tuple<std::string, std::string>>(
            [](const std::tuple<std::string, std::string>& params) -> Command {
                return RemoveUserFromGroupCommand{std::get<0>(params), std::get<1>(params)};
            },
            (keyword("REMOVE") >> whitespace() >> keyword("USER") >> whitespace() >> identifier()) & 
            (whitespace() >> keyword("FROM") >> whitespace() >> keyword("GROUP") >> whitespace() >> identifier())
        );
    }

    static parsec::Parser<Command> getUsersParser() {
        return parsec::fmap<Command, std::string>(
            [](const std::string&) -> Command {
                return GetUsersCommand{};
            },
            keyword("GET") >> whitespace() >> keyword("USERS")
        );
    }

    static parsec::Parser<Command> getGroupsParser() {
        return parsec::fmap<Command, std::string>(
            [](const std::string&) -> Command {
                return GetGroupsCommand{};
            },
            keyword("GET") >> whitespace() >> keyword("GROUPS")
        );
    }

    static parsec::Parser<Command> getMessageHistoryParser() {
        return parsec::fmap<Command, std::string>(
            [](const std::string& username) -> Command {
                return GetMessageHistoryCommand{username};
            },
            keyword("GET") >> whitespace() >> keyword("MESSAGE") >> whitespace() >> keyword("HISTORY") >> whitespace() >> identifier()
        );
    }

    static parsec::Parser<Command> exitParser() {
        return parsec::fmap<Command, std::string>(
            [](const std::string&) -> Command {
                return ExitCommand{};
            },
            keyword("EXIT")
        );
    }
};

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

int main() {
    TaskProcessor processor;
    
    // Process all task files
    std::vector<std::string> taskFiles = {
        "tasks/task1.txt", 
        "tasks/task2.txt", 
        "tasks/task3.txt", 
        "tasks/task4.txt", 
        "tasks/task5.txt"
    };
    
    processor.processTasks(taskFiles);
    
    return 0;
}
