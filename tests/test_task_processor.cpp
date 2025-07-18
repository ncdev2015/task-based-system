#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include "task/processor.hpp"

namespace fs = std::filesystem;

class TaskProcessorTestFixture {
public:
    TaskProcessor processor;
    std::string testDir = "test_tasks";
    
    void SetUp() {
        // Create test directory
        fs::create_directories(testDir);
        
        // Redirect cout to capture output
        originalCout = std::cout.rdbuf();
        std::cout.rdbuf(outputBuffer.rdbuf());
    }
    
    void TearDown() {
        // Restore cout
        std::cout.rdbuf(originalCout);
        
        // Clean up test files
        if (fs::exists(testDir)) {
            fs::remove_all(testDir);
        }
    }
    
    void createTestFile(const std::string& filename, const std::string& content) {
        std::ofstream file(testDir + "/" + filename);
        file << content;
        file.close();
    }
    
    std::string getOutput() {
        return outputBuffer.str();
    }
    
    void clearOutput() {
        outputBuffer.str("");
        outputBuffer.clear();
    }

private:
    std::stringstream outputBuffer;
    std::streambuf* originalCout;
};

TEST_CASE("TaskProcessor - Basic Functionality", "[TaskProcessor]") {
    TaskProcessorTestFixture fixture;
    fixture.SetUp();
    
    SECTION("Constructor creates instance without throwing") {
        REQUIRE_NOTHROW(TaskProcessor{});
    }
    
    SECTION("Process single valid task") {
        fixture.createTestFile("valid_task.txt", R"(
            CREATE USER alice
            CREATE USER bob
            GET USERS
            EXIT
        )");
        
        REQUIRE_NOTHROW(fixture.processor.processTask(fixture.testDir + "/valid_task.txt"));
        
        std::string output = fixture.getOutput();
        REQUIRE(output.find("[Processing task:") != std::string::npos);
        REQUIRE(output.find("✅ CREATE USER alice") != std::string::npos);
        REQUIRE(output.find("✅ CREATE USER bob") != std::string::npos);
        REQUIRE(output.find("completed successfully") != std::string::npos);
    }
    
    fixture.TearDown();
}

TEST_CASE("TaskProcessor - Command Parsing", "[TaskProcessor][Parsing]") {
    TaskProcessorTestFixture fixture;
    fixture.SetUp();
    
    SECTION("Valid commands are parsed correctly") {
        fixture.createTestFile("valid_commands.txt", R"(
            CREATE USER alice
            DELETE USER bob
            DISABLE USER charlie
            SEND MESSAGE alice "Hello World"
            PING dave 3
            ADD USER alice TO GROUP admins
            REMOVE USER alice FROM GROUP users
            GET USERS
            GET GROUPS
            GET MESSAGE HISTORY alice
            EXIT
        )");
        
        REQUIRE_NOTHROW(fixture.processor.processTask(fixture.testDir + "/valid_commands.txt"));
        
        std::string output = fixture.getOutput();
        REQUIRE(output.find("❌ Invalid command:") == std::string::npos);
    }
    
    SECTION("Invalid commands are rejected") {
        fixture.createTestFile("invalid_commands.txt", R"(
            CREATE USER alice
            INVALID_COMMAND bob
            DELETE USER charlie
        )");
        
        fixture.processor.processTask(fixture.testDir + "/invalid_commands.txt");
        
        std::string output = fixture.getOutput();
        REQUIRE(output.find("❌ Invalid command: INVALID_COMMAND bob") != std::string::npos);
        REQUIRE(output.find("stopped due to failure") != std::string::npos);
    }
    
    SECTION("Commands with missing arguments are rejected") {
        fixture.createTestFile("missing_args.txt", R"(
            CREATE USER alice
            SEND MESSAGE alice
            DELETE USER bob
        )");
        
        fixture.processor.processTask(fixture.testDir + "/missing_args.txt");
        
        std::string output = fixture.getOutput();
        REQUIRE(output.find("❌") != std::string::npos);
        REQUIRE(output.find("stopped due to failure") != std::string::npos);
    }
    
    fixture.TearDown();
}

TEST_CASE("TaskProcessor - File Handling", "[TaskProcessor][Files]") {
    TaskProcessorTestFixture fixture;
    fixture.SetUp();
    
    SECTION("Non-existent file throws exception") {
        REQUIRE_THROWS_AS(
            fixture.processor.processTask("non_existent_file.txt"),
            std::runtime_error
        );
    }
    
    SECTION("Empty file processes successfully") {
        fixture.createTestFile("empty.txt", "");
        
        REQUIRE_NOTHROW(fixture.processor.processTask(fixture.testDir + "/empty.txt"));
        
        std::string output = fixture.getOutput();
        REQUIRE(output.find("completed successfully") != std::string::npos);
    }
    
    SECTION("File with only comments processes successfully") {
        fixture.createTestFile("comments_only.txt", R"(
            # This is a comment
            # Another comment
            # Yet another comment
        )");
        
        REQUIRE_NOTHROW(fixture.processor.processTask(fixture.testDir + "/comments_only.txt"));
        
        std::string output = fixture.getOutput();
        REQUIRE(output.find("completed successfully") != std::string::npos);
    }
    
    SECTION("Comments are properly filtered") {
        fixture.createTestFile("with_comments.txt", R"(
            # Create some users
            CREATE USER alice  # This is Alice
            CREATE USER bob    # This is Bob
            # List all users
            GET USERS
            EXIT
        )");
        
        REQUIRE_NOTHROW(fixture.processor.processTask(fixture.testDir + "/with_comments.txt"));
        
        std::string output = fixture.getOutput();
        REQUIRE(output.find("✅ CREATE USER alice") != std::string::npos);
        REQUIRE(output.find("✅ CREATE USER bob") != std::string::npos);
        REQUIRE(output.find("completed successfully") != std::string::npos);
    }
    
    fixture.TearDown();
}

TEST_CASE("TaskProcessor - Task Execution Flow", "[TaskProcessor][Execution]") {
    TaskProcessorTestFixture fixture;
    fixture.SetUp();
    
    SECTION("Task stops on first failure") {
        fixture.createTestFile("failing_task.txt", R"(
            CREATE USER alice
            CREATE USER bob
            DELETE USER bob
            SEND MESSAGE bob "This should fail"
            CREATE USER charlie
        )");
        
        fixture.processor.processTask(fixture.testDir + "/failing_task.txt");
        
        std::string output = fixture.getOutput();
        REQUIRE(output.find("✅ CREATE USER alice") != std::string::npos);
        REQUIRE(output.find("✅ CREATE USER bob") != std::string::npos);
        REQUIRE(output.find("✅ DELETE USER bob") != std::string::npos);
        REQUIRE(output.find("❌ SEND MESSAGE bob") != std::string::npos);
        REQUIRE(output.find("CREATE USER charlie") == std::string::npos); // Should not execute
        REQUIRE(output.find("stopped due to failure") != std::string::npos);
    }
    
    SECTION("EXIT command terminates task gracefully") {
        fixture.createTestFile("exit_task.txt", R"(
            CREATE USER alice
            CREATE USER bob
            EXIT
            DELETE USER alice
        )");
        
        fixture.processor.processTask(fixture.testDir + "/exit_task.txt");
        
        std::string output = fixture.getOutput();
        REQUIRE(output.find("✅ CREATE USER alice") != std::string::npos);
        REQUIRE(output.find("✅ CREATE USER bob") != std::string::npos);
        REQUIRE(output.find("DELETE USER alice") == std::string::npos); // Should not execute
        REQUIRE(output.find("completed successfully") != std::string::npos);
    }
    
    fixture.TearDown();
}

TEST_CASE("TaskProcessor - Multiple Tasks", "[TaskProcessor][Multiple]") {
    TaskProcessorTestFixture fixture;
    fixture.SetUp();
    
    SECTION("Multiple tasks are processed independently") {
        fixture.createTestFile("task1.txt", R"(
            CREATE USER alice
            CREATE USER bob
            EXIT
        )");
        
        fixture.createTestFile("task2.txt", R"(
            CREATE USER charlie
            CREATE USER dave
            EXIT
        )");
        
        std::vector<std::string> tasks = {
            fixture.testDir + "/task1.txt",
            fixture.testDir + "/task2.txt"
        };
        
        REQUIRE_NOTHROW(fixture.processor.processTasks(tasks));
        
        std::string output = fixture.getOutput();
        REQUIRE(output.find("[Processing task: " + fixture.testDir + "/task1.txt]") != std::string::npos);
        REQUIRE(output.find("[Processing task: " + fixture.testDir + "/task2.txt]") != std::string::npos);
        REQUIRE(output.find("✅ CREATE USER alice") != std::string::npos);
        REQUIRE(output.find("✅ CREATE USER charlie") != std::string::npos);
    }
    
    SECTION("Failed task doesn't affect subsequent tasks") {
        fixture.createTestFile("failing_task.txt", R"(
            CREATE USER alice
            INVALID_COMMAND
            CREATE USER bob
        )");
        
        fixture.createTestFile("success_task.txt", R"(
            CREATE USER charlie
            GET USERS
            EXIT
        )");
        
        std::vector<std::string> tasks = {
            fixture.testDir + "/failing_task.txt",
            fixture.testDir + "/success_task.txt"
        };
        
        fixture.processor.processTasks(tasks);
        
        std::string output = fixture.getOutput();
        REQUIRE(output.find("stopped due to failure") != std::string::npos);
        REQUIRE(output.find("✅ CREATE USER charlie") != std::string::npos);
        REQUIRE(output.find("completed successfully") != std::string::npos);
    }
    
    fixture.TearDown();
}

TEST_CASE("TaskProcessor - User State Management", "[TaskProcessor][State]") {
    TaskProcessorTestFixture fixture;
    fixture.SetUp();
    
    SECTION("User state is reset between tasks") {
        fixture.createTestFile("task1.txt", R"(
            CREATE USER alice
            CREATE USER bob
            EXIT
        )");
        
        fixture.createTestFile("task2.txt", R"(
            CREATE USER alice
            GET USERS
            EXIT
        )");
        
        std::vector<std::string> tasks = {
            fixture.testDir + "/task1.txt",
            fixture.testDir + "/task2.txt"
        };
        
        fixture.processor.processTasks(tasks);
        
        std::string output = fixture.getOutput();
        
        // Alice should be created successfully in both tasks
        size_t first_alice = output.find("✅ CREATE USER alice");
        size_t second_alice = output.find("✅ CREATE USER alice", first_alice + 1);
        
        REQUIRE(first_alice != std::string::npos);
        REQUIRE(second_alice != std::string::npos);
    }
    
    fixture.TearDown();
}

// Custom main to handle setup/teardown if needed
int main(int argc, char* argv[]) {
    return Catch::Session().run(argc, argv);
}