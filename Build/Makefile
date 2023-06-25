CC := g++
CFLAGS := -Wall -Wextra -std=c++17

 # Default Out dir, build scripts should pass a OUT_DIR value
OUT_DIR := GameOut
# Default bin name, build scripts should pass in a BIN_NAME value
BIN_NAME := libGame.dylib
# Default exe name, build scripts should pass in a EXE_NAME value
EXE_NAME := Game

TARGET := $(OUT_DIR)/$(EXE_NAME)

.PHONY: all clean

all: $(TARGET)

$(TARGET):
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $(OUT_DIR)/lib/$(BIN_NAME) main.cpp -o $@ -Wl, -rpath lib/

clean: 
	@rm -rf $(OUT_DIR)
