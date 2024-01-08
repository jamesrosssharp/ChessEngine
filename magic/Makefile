SRC_DIR=./

BUILD_DIR=../build

TARGET=$(BUILD_DIR)/chess-engine-magic

all: $(TARGET)

CPP_SRC	= $(wildcard $(SRC_DIR)/*.cpp)
HEADERS	= $(wildcard $(SRC_DIR)/*.h)

CPP_OBJ = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(notdir $(CPP_SRC)))
CPP_OBJ_NOMAIN = $(filter-out $(BUILD_DIR)/main.o, $(CPP_OBJ))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(BUILD_DIR)
	@echo "CXX $<"
	@g++ -std=c++20 -O3 -c -ggdb -o $@ -I$(SRC_DIR) $<

$(TARGET): $(CPP_OBJ) 
	@g++ -o $@ $(CPP_OBJ)  -O3 -lGL -lSDL2
	@echo "LNK"

clean:
	rm -r $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)


