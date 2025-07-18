#ifndef USER_MANAGER_HPP
#define USER_MANAGER_HPP

// Standard Library
#include <algorithm>          // For std::sort
#include <memory>             // For std::unique_ptr
#include <string>
#include <unordered_map>      // For std::unordered_map
#include <unordered_set>      // For std::unordered_set
#include <utility>            // For std::pair
#include <vector>

// Project Headers
#include "user/user.hpp"      // For User class definition

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

#endif // USER_MANAGER_HPP
