CXX = g++
SRC = main.cpp
TARGET = polynomial
CXXFLAGS = -fpermissive -g
BIN_DIR = bin

all: $(BIN_DIR) $(BIN_DIR)/$(TARGET)

$(BIN_DIR):
	"mkdir" -p $(BIN_DIR)

$(BIN_DIR)/$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN_DIR)/$(TARGET)

run: $(BIN_DIR)/$(TARGET)
	./$(BIN_DIR)/$(TARGET)

clean: $(BIN_DIR)
	"rm" -rf $(BIN_DIR)/$(TARGET)
