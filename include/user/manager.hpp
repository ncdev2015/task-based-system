#ifndef USER_MANAGER_HPP
#define USER_MANAGER_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "user/user.hpp"

class UserManager {
private:
    std::unordered_map<std::string, std::unique_ptr<User>> users;
    std::unordered_set<std::string> groups;

public:
    void reset();
    
    bool createUser(const std::string& username);
    bool deleteUser(const std::string& username);
    bool disableUser(const std::string& username);
    bool userExists(const std::string& username) const;
    bool isUserEnabled(const std::string& username) const;
    bool sendMessage(const std::string& username, const std::string& message);
    bool addUserToGroup(const std::string& username, const std::string& group);
    bool removeUserFromGroup(const std::string& username, const std::string& group);
    
    std::vector<std::string> getUsers() const;
    std::vector<std::string> getGroups() const;
    std::vector<std::string> getMessageHistory(const std::string& username) const;
};

#endif // USER_MANAGER_HPP