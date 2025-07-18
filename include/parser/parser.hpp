#ifndef PARSER_PARSER_HPP
#define PARSER_PARSER_HPP

// Standard Library
#include <algorithm>
#include <cctype>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

// Third-party
#include <fmt/format.h>
#include <parsec/parsec.hpp>

// Project headers
#include "commands/command.hpp"

class CommandParser {
private:
    // Basic parsers
    static parsec::Parser<char> character(char c);
    static parsec::Parser<std::string> whitespace();
    static parsec::Parser<std::string> identifier();
    static parsec::Parser<std::string> quotedString();
    static parsec::Parser<int> number();
    static parsec::Parser<std::string> keyword(const std::string& word);

public:
    static parsec::Parser<Command> commandParser();

private:
    // Command-specific parsers
    static parsec::Parser<Command> createUserParser();
    static parsec::Parser<Command> deleteUserParser();
    static parsec::Parser<Command> disableUserParser();
    static parsec::Parser<Command> sendMessageParser();
    static parsec::Parser<Command> pingParser();
    static parsec::Parser<Command> addUserToGroupParser();
    static parsec::Parser<Command> removeUserFromGroupParser();
    static parsec::Parser<Command> getUsersParser();
    static parsec::Parser<Command> getGroupsParser();
    static parsec::Parser<Command> getMessageHistoryParser();
    static parsec::Parser<Command> exitParser();
};

#endif // PARSER_PARSER_HPP