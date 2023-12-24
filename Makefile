CC := g++
CFLAGS := -Wall -std=c++11 -Iinclude/SDL2
LDFLAGS := -lSDL2 -lSDL2_image

SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
TEST_DIR := test

SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC))
EXECUTABLE := $(BIN_DIR)/game.chess

TEST_SRC := $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJ := $(patsubst $(TEST_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(TEST_SRC))
TEST_EXECUTABLE := $(BIN_DIR)/test.runner

.PHONY: all clean run test

all: clean $(EXECUTABLE)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXECUTABLE): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

test: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_EXECUTABLE): $(TEST_OBJ) $(filter-out $(BUILD_DIR)/main.o, $(OBJ))
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(BUILD_DIR)/*.o $(BIN_DIR)/*

run: $(EXECUTABLE)
	./$(EXECUTABLE)
