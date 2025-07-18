#include "parser/parser.hpp"

// Basic parsers implementation
parsec::Parser<char> CommandParser::character(char c) {
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

parsec::Parser<std::string> CommandParser::whitespace() {
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

parsec::Parser<std::string> CommandParser::identifier() {
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

parsec::Parser<std::string> CommandParser::quotedString() {
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

parsec::Parser<int> CommandParser::number() {
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

parsec::Parser<std::string> CommandParser::keyword(const std::string& word) {
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

// Command parser implementation
parsec::Parser<Command> CommandParser::commandParser() {
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

// Command-specific parsers implementation
parsec::Parser<Command> CommandParser::createUserParser() {
    return parsec::fmap<Command, std::string>(
        [](const std::string& username) -> Command {
            return CreateUserCommand{username};
        },
        keyword("CREATE") >> whitespace() >> keyword("USER") >> whitespace() >> identifier()
    );
}

parsec::Parser<Command> CommandParser::deleteUserParser() {
    return parsec::fmap<Command, std::string>(
        [](const std::string& username) -> Command {
            return DeleteUserCommand{username};
        },
        keyword("DELETE") >> whitespace() >> keyword("USER") >> whitespace() >> identifier()
    );
}

parsec::Parser<Command> CommandParser::disableUserParser() {
    return parsec::fmap<Command, std::string>(
        [](const std::string& username) -> Command {
            return DisableUserCommand{username};
        },
        keyword("DISABLE") >> whitespace() >> keyword("USER") >> whitespace() >> identifier()
    );
}

parsec::Parser<Command> CommandParser::sendMessageParser() {
    return parsec::fmap<Command, std::tuple<std::string, std::string>>(
        [](const std::tuple<std::string, std::string>& params) -> Command {
            return SendMessageCommand{std::get<0>(params), std::get<1>(params)};
        },
        (keyword("SEND") >> whitespace() >> keyword("MESSAGE") >> whitespace() >> identifier()) & 
        (whitespace() >> quotedString())
    );
}

parsec::Parser<Command> CommandParser::pingParser() {
    return parsec::fmap<Command, std::tuple<std::string, int>>(
        [](const std::tuple<std::string, int>& params) -> Command {
            return PingCommand{std::get<0>(params), std::get<1>(params)};
        },
        (keyword("PING") >> whitespace() >> identifier()) & (whitespace() >> number())
    );
}

parsec::Parser<Command> CommandParser::addUserToGroupParser() {
    return parsec::fmap<Command, std::tuple<std::string, std::string>>(
        [](const std::tuple<std::string, std::string>& params) -> Command {
            return AddUserToGroupCommand{std::get<0>(params), std::get<1>(params)};
        },
        (keyword("ADD") >> whitespace() >> keyword("USER") >> whitespace() >> identifier()) & 
        (whitespace() >> keyword("TO") >> whitespace() >> keyword("GROUP") >> whitespace() >> identifier())
    );
}

parsec::Parser<Command> CommandParser::removeUserFromGroupParser() {
    return parsec::fmap<Command, std::tuple<std::string, std::string>>(
        [](const std::tuple<std::string, std::string>& params) -> Command {
            return RemoveUserFromGroupCommand{std::get<0>(params), std::get<1>(params)};
        },
        (keyword("REMOVE") >> whitespace() >> keyword("USER") >> whitespace() >> identifier()) & 
        (whitespace() >> keyword("FROM") >> whitespace() >> keyword("GROUP") >> whitespace() >> identifier())
    );
}

parsec::Parser<Command> CommandParser::getUsersParser() {
    return parsec::fmap<Command, std::string>(
        [](const std::string&) -> Command {
            return GetUsersCommand{};
        },
        keyword("GET") >> whitespace() >> keyword("USERS")
    );
}

parsec::Parser<Command> CommandParser::getGroupsParser() {
    return parsec::fmap<Command, std::string>(
        [](const std::string&) -> Command {
            return GetGroupsCommand{};
        },
        keyword("GET") >> whitespace() >> keyword("GROUPS")
    );
}

parsec::Parser<Command> CommandParser::getMessageHistoryParser() {
    return parsec::fmap<Command, std::string>(
        [](const std::string& username) -> Command {
            return GetMessageHistoryCommand{username};
        },
        keyword("GET") >> whitespace() >> keyword("MESSAGE") >> whitespace() >> keyword("HISTORY") >> whitespace() >> identifier()
    );
}

parsec::Parser<Command> CommandParser::exitParser() {
    return parsec::fmap<Command, std::string>(
        [](const std::string&) -> Command {
            return ExitCommand{};
        },
        keyword("EXIT")
    );
}