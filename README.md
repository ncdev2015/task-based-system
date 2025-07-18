# Task-Based User Management System

A C++17 task processor that reads predefined files containing user management commands and executes them sequentially. This system is designed for extensibility and robust error handling.

## Overview

The system processes multiple task files, executing user management commands one by one. Each file represents an independent task, and execution stops on the first failure within a task while continuing with the next task.

## Features

- **Sequential Task Processing**: Executes multiple task files in order
- **Command Validation**: Validates command syntax and arguments before execution
- **Error Handling**: Stops task execution on first failure, continues with next task
- **Extensible Architecture**: Easy to add new operations without modifying existing logic
- **Structured Output**: Clear success/failure indicators with detailed error messages

## Supported Commands

### User Management
- `CREATE USER <username>` - Create a new user
- `DELETE USER <username>` - Remove a user
- `DISABLE USER <username>` - Disable a user account

### Messaging
- `SEND MESSAGE <username> "<message>"` - Send a message to a user
- `GET MESSAGE HISTORY <username>` - Retrieve all messages sent to a user

### Network Operations
- `PING <username> <times>` - Send ping to user specified number of times

### Group Management
- `ADD USER <username> TO GROUP <group>` - Add user to a group
- `REMOVE USER <username> FROM GROUP <group>` - Remove user from a group

### Information Retrieval
- `GET USERS` - Retrieve list of all users
- `GET GROUPS` - Retrieve list of all user groups

### Control Flow
- `EXIT` - Stop executing commands in current task

## Requirements

- **C++17** or higher
- **CMake 3.24** or higher
- **Git** (for dependency management)

## Dependencies

- **fmt** (10.1.1) - Modern C++ formatting library
- **parsec** - Parser combinator library (included in third_party)
- **Catch2** (3.4.0) - Testing framework (for development)

## Building

### Clone and Build
```bash
git clone <repository-url>
cd task-based-system
mkdir build && cd build
cmake ..
make
```

### Run
```bash
./wzh-assesment
```

## Project Structure

```
task-based-system/
├── CMakeLists.txt
├── include/
│   ├── commands/          # Command interfaces
│   ├── parser/            # Parser combinator headers
│   ├── task/              # Task processing headers
│   ├── registry/          # User/Group registry headers
│   └── user/              # User management headers
├── source/
│   ├── main.cpp
│   ├── commands/          # Command implementations
│   ├── parser/            # Parser implementations
│   ├── task/              # Task processing implementations
│   ├── registry/          # Registry implementations
│   └── user/              # User management implementations
├── third_party/
│   └── parsec/            # Parser combinator library
├── tests/
│   ├── test_task_processor.cpp
│   ├── test_user_manager.cpp
│   ├── test_command_registry.cpp
│   └── test_integration.cpp
└── tasks/
    ├── task1.txt
    ├── task2.txt
    └── ...
```

## Task File Format

Task files contain commands separated by newlines. Comments start with `#`.

### Example Task File
```
# Creating users
CREATE USER alice
CREATE USER bob

# User operations
SEND MESSAGE alice "Welcome to the system!"
PING alice 3

# Group operations
ADD USER alice TO GROUP admins
GET USERS
GET GROUPS

# End task
EXIT
```

## Output Format

The system provides structured output showing command execution results:

```
[Processing task: task1.txt]
✅ CREATE USER alice
✅ CREATE USER bob
✅ DELETE USER bob
❌ SEND MESSAGE bob "Hello, how are you?" (Failed: User does not exist)
[Task task1.txt stopped due to failure]
```

### Output Symbols
- ✅ Command succeeded
- ❌ Command failed (with reason)

## Error Handling

### Task Validation
- **Incorrect Command Names**: Task creation fails on typos (e.g., `CREAT` instead of `CREATE`)
- **Missing Arguments**: Task creation fails on insufficient arguments
- **Extra Arguments**: Task creation fails on too many arguments

### Runtime Errors
- **User Not Found**: Operations on non-existent users fail gracefully
- **Group Operations**: Validates group membership before operations
- **Message Operations**: Validates user existence before sending messages

## Extensibility

The system is designed for easy extension:

1. **Add New Commands**: Implement command interface and register in command registry
2. **Dynamic Registration**: Commands are registered at runtime
3. **Modular Design**: New operations don't require modifying existing execution flow

## Testing

The project includes comprehensive tests using **Catch2 v3.4.0** framework.

### Test Structure

Tests are organized by component:

- **Unit Tests**: Test individual classes in isolation
  - `test_user_manager.cpp` - UserManager functionality
  - `test_task_processor.cpp` - TaskProcessor operations
  - `test_command_registry.cpp` - Command registration and execution
  
- **Integration Tests**: Test component interactions
  - `test_integration.cpp` - End-to-end workflow testing

### Running Tests

```bash
# Build with tests enabled
mkdir build && cd build
cmake -DBUILD_TESTING=ON ..
make

# Run all tests
ctest

# Run specific test with verbose output
ctest -V -R "TaskProcessor"

# Run tests directly
./wzh-assesment-tests
```

### Test Coverage

The test suite covers:

- **Happy Path**: Normal operation scenarios
- **Error Handling**: Invalid inputs, missing files, user not found
- **Edge Cases**: Empty files, malformed commands, boundary conditions
- **Integration**: Complete task processing workflows

### Example Test Output

```
===============================================================================
All tests passed (41 assertions in 6 test cases)

Test Summary:
- TaskProcessor: File handling, command processing, error scenarios
- UserManager: User creation, authentication, state management
- CommandRegistry: Command registration, execution, validation
- Integration: Complete workflow testing
```

### Writing New Tests

When adding new features:

1. **Create unit tests** for new classes/methods
2. **Test error conditions** and edge cases
3. **Add integration tests** for new workflows
4. **Follow existing patterns** in test structure

Example test structure:
```cpp
TEST_CASE("ClassName - Feature Description") {
    // Setup
    ClassName instance;
    
    SECTION("Success case") {
        // Test normal operation
        REQUIRE(instance.method() == expected_result);
    }
    
    SECTION("Error case") {
        // Test error handling
        REQUIRE_THROWS_AS(instance.method("invalid"), std::runtime_error);
    }
}
```

## Development

### Adding New Commands

1. Create command class implementing the command interface
2. Add command parser in parser module
3. Register command in command registry
4. **Add corresponding tests** for the new command
5. Update documentation

### Debug Build
```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON ..
make
```

### Memory Debugging
```bash
# Run tests with valgrind
valgrind --tool=memcheck --leak-check=full ./wzh-assesment-tests

# Debug specific test failures
gdb ./wzh-assesment-tests
```

## Example Usage

Given task files in the `tasks/` directory:

```bash
# Run all tasks
./wzh-assesment

# Expected output:
[Processing task: task1.txt]
✅ CREATE USER alice
✅ CREATE USER bob
...
[Task task1.txt completed successfully]

[Processing task: task2.txt]
✅ CREATE USER alice
...
```

## Architecture

The system follows modern C++ practices:

- **RAII**: Resource management through smart pointers
- **Exception Safety**: Proper error handling and resource cleanup
- **Modularity**: Clear separation of concerns
- **Extensibility**: Plugin-like architecture for commands
- **Type Safety**: Strong typing throughout the system
- **Test-Driven**: Comprehensive test coverage for reliability

## License

This project is part of a technical assessment and is provided as-is for evaluation purposes.