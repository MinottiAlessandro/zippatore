# Compiler and flags
CC ?= gcc                             # C compiler
CXX ?= g++                            # C++ compiler
CFLAGS ?= -Wall -Wextra -g            # C flags
CXXFLAGS ?= -Wall -Wextra -g          # C++ flags
LDFLAGS ?= -lm                        # Linking flags

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin
LIB_DIR = lib
TEST_DIR = tests

# Files
SRCS := $(wildcard $(SRC_DIR)/*.c)                    # Get all .c files
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)        # Replace .c with .o
DEPS := $(OBJS:.o=.d)                                 # Dependencies files
TARGET = $(BIN_DIR)/zippatore                           # Final executable

# Libraries (example with math and pthread)
LIBS = -lm -lpthread

# Include paths
INC = -I$(INC_DIR)

# Colors for pretty printing
GREEN = \033[0;32m
NC = \033[0m  # No Color

# Default target
.PHONY: all
all: dirs $(TARGET)

# Create necessary directories
.PHONY: dirs
dirs:
	@mkdir -p $(BIN_DIR) $(OBJ_DIR)
	@echo "$(GREEN)Created directories$(NC)"

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "$(GREEN)Compiling $<$(NC)"
	@$(CC) $(CFLAGS) $(INC) -c $< -o $@
	@$(CC) $(CFLAGS) $(INC) -MM -MT $@ $< > $(OBJ_DIR)/$*.d

# Link object files to create executable
$(TARGET): $(OBJS)
	@echo "$(GREEN)Linking $@$(NC)"
	@$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $@
	@echo "$(GREEN)Build complete!$(NC)"

# Clean build files
.PHONY: clean
clean:
	@rm -rf $(BIN_DIR) $(OBJ_DIR)
	@echo "$(GREEN)Cleaned build files$(NC)"

# Install target (example)
.PHONY: install
install: all
	@cp $(TARGET) /usr/local/bin/
	@echo "$(GREEN)Installed to /usr/local/bin/$(NC)"

# Debug build
.PHONY: debug
debug: CFLAGS += -DDEBUG -g
debug: clean all

# Release build
.PHONY: release
release: CFLAGS += -O3 -DNDEBUG
release: clean all

# Run tests
.PHONY: test
test: all
	@echo "$(GREEN)Running tests...$(NC)"
	@./$(TEST_DIR)/run_tests.sh

# Generate documentation (example with Doxygen)
.PHONY: docs
docs:
	@doxygen Doxyfile

# Include dependency files
-include $(DEPS)

# Help target
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  all     : Build everything (default)"
	@echo "  clean   : Remove built files"
	@echo "  install : Install the program"
	@echo "  debug   : Build with debug flags"
	@echo "  release : Build with optimization"
	@echo "  test    : Run tests"
	@echo "  docs    : Generate documentation"
	@echo "  help    : Show this help message"