# Makefile for SimpleGameEngine

# Compiler and flags
CC := g++
CFLAGS := -Wall -Wextra -std=c++17

# Directories
DEP_DIR := dependencies/SDL
INC_DIR := include
SRC_DIR := src
OBJ_DIR := obj
LIB_DIR := lib

# Dependencies
SDL_INC_DIR := $(DEP_DIR)/include

# Source files
SRC_FILES := $(wildcard $(SRC_DIR)/*/*.cpp) $(wildcard $(SRC_DIR)/*/*/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Target
TARGET := $(LIB_DIR)/libSimpleGameEngine.a

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	@mkdir -p $(LIB_DIR)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)/$(dir $*)
	$(CC) $(CFLAGS) -I $(INC_DIR) -I $(SDL_INC_DIR) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR) $(LIB_DIR)

