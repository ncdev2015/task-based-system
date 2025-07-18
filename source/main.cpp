#include "task/processor.hpp"  // For TaskProcessor
#include <vector>              // For std::vector
#include <string>              // For std::string

int main() {
    TaskProcessor processor;
    
    // Process all task files
    const std::vector<std::string> taskFiles = {
        "tasks/task1.txt", 
        "tasks/task2.txt", 
        "tasks/task3.txt", 
        "tasks/task4.txt", 
        "tasks/task5.txt"
    };
    
    processor.processTasks(taskFiles);
    
    return 0;
}