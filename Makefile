CXX       := g++
CXX_FLAGS := -std=c++17 #-Wall -Wextra 

BIN     := bin
SRC     := src
INCLUDE := include
LIBRARIES   := -stdlib=libc++
PEER  := peer
SERVER := server
EXECUTABLE := peer server

all: $(BIN) $(EXECUTABLE)

peer: $(SRC)/peer/*.cpp
	@echo "🚧 Building peer..."
	$(CXX) $(CXX_FLAGS) -I$(SRC)/peer -g $^ -o bin/$@ -lpthread -fcoroutines-ts

server: $(SRC)/server/*.cpp
	@echo "🚧 Building server..."
	$(CXX) $(CXX_FLAGS) -I$(SRC)/server -g $^ -o bin/$@ -lpthread

bin:
	mkdir bin

clean:
	@echo "🧹 Clearing..."
	-rm $(BIN)/*