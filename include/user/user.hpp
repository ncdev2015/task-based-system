#ifndef USER_USER_HPP
#define USER_USER_HPP

// Standard Library
#include <string>            // For std::string
#include <unordered_set>     // For std::unordered_set
#include <vector>           // For std::vector

// User data structures
struct User {
    std::string username;
    bool enabled = true;
    std::vector<std::string> messages;
    std::unordered_set<std::string> groups;
};

#endif // USER_USER_HPP
