CC := g++
CFLAGS := -Wall -Wextra -std=c++17

# Directories
ROOT_DIR := .
SCR_DIR := $(ROOT_DIR)/scripts
SGE_DIR := $(ROOT_DIR)/dependencies/SGE
SDL_DIR := $(ROOT_DIR)/dependencies/SDL
OBJ_DIR := $(ROOT_DIR)/bin/obj
OUT_DIR := $(ROOT_DIR)/bin/lib

# Include directories
SGE_INC_DIR := $(SGE_DIR)/include
SDL_INC_DIR := $(SDL_DIR)/include

# External libs link
SGE_LIB_DIR := $(SGE_DIR)/lib
SDL_LIB_DIR := $(SDL_DIR)/lib
LIBS_FLAGS :=  -lSimpleGameEngine $(SDL_LIB_DIR)/libSDL2_image-2.0.0.dylib $(SDL_LIB_DIR)/libSDL2_mixer-2.0.0.dylib $(SDL_LIB_DIR)/libSDL2_ttf-2.0.0.dylib $(SDL_LIB_DIR)/libSDL2-2.0.0.dylib

# Source files
SRC_FILES := $(wildcard $(ROOT_DIR)/*.cpp) $(wildcard $(SCR_DIR)/*/*.cpp)
SRC_FILES := $(filter-out $(ROOT_DIR)/dllexport/dllmain.cpp, $(SRC_FILES)) # Exclude that windows only file
OBJ_FILES := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SRC_FILES)))

BIN_NAME := libGame.dylib # Default bin name, build scripts should pass in a BIN_NAME value
TARGET := $(OUT_DIR)/$(BIN_NAME)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) -shared -I $(SDL_INC_DIR) -I $(SGE_INC_DIR) -L $(SGE_LIB_DIR) -L $(SDL_LIB_DIR) $(LIBS_FLAGS) -o $@ $^ -Wl, -rpath .

define cc-command
	@mkdir -p $(OBJ_DIR)/$(dir $*)
	$(CC) $(CFLAGS) -I$(SDL_INC_DIR) -I$(SGE_INC_DIR) -c $< -o $@
endef

$(OBJ_DIR)/%.o: $(ROOT_DIR)/%.cpp
	$(cc-command)

$(OBJ_DIR)/%.o: $(SCR_DIR)/*/%.cpp
	$(cc-command)

clean: 
	@rm -rf $(OBJ_DIR) $(OUT_DIR)
