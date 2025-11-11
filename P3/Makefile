CC=gcc # compiler
CFLAGS= -std=c99 -Wall -fsanitize=address,undefined -I. -Isrc # compiler options

BUILD_FOLDER = builds

MYSH = mysh.c

# # gets all tests (exclude helper.c since it's included by testingInterface.c)
TESTS := $(filter-out tests/helper.c, $(wildcard tests/*.c))

TEST_OUTPUTS := $(patsubst tests/%.c, $(BUILD_FOLDER)/%, $(TESTS))

all: mysh.o ${TEST_OUTPUTS}

mysh.o: mysh.c
	@mkdir -p $(BUILD_FOLDER)
	$(CC) $(CFLAGS) mysh.c -o mysh

# recipe to build each test output
$(BUILD_FOLDER)/%: tests/%.c
	@mkdir -p $(BUILD_FOLDER)
	$(CC) $(CFLAGS) -o $@ $^

# run a single test: make runTest TEST=someTest
runTest: all
	@echo "\nRunning $(TEST) ... \n"
	@./$(BUILD_FOLDER)/$(TEST)

# # run all tests
# runAllTests: all
# 	@./$(BUILD_FOLDER)/runTests

# remove mysh.o and all built test outputs
clean:
	rm -f -rf $(BUILD_FOLDER)/* mysh.o