CXX       := clang++
CXX_FLAGS := -std=c++17 #-Wall -Wextra 

BIN     := bin
SRC     := src
INCLUDE := include
LIBRARIES   := -stdlib=libc++
PEER  := peer
SERVER := server
EXECUTABLE := peer server

all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	@echo "🚀 Executing..."
	./$(BIN)/$(EXECUTABLE)

peer: $(SRC)/peer/*.cpp
	@echo "🚧 Building peer..."
	$(CXX) $(CXX_FLAGS) -I$(SRC)/peer $(LIBRARIES) -g $^ -o bin/$@

server: $(SRC)/server/*.cpp
	@echo "🚧 Building server..."
	$(CXX) $(CXX_FLAGS) -I$(SRC)/source $(LIBRARIES) -g $^ -o bin/$@

clean:
	@echo "🧹 Clearing..."
	-rm $(BIN)/*