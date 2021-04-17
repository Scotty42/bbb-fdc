CXX		  := arm-linux-gnueabihf-g++
CXX_FLAGS := -Wall -Wextra -std=c++17 -ggdb

BIN		:= test/bin
SRC		:= test
INCLUDE	:= test/include
LIB		:= test/lib

LIBRARIES	:= -liobb
EXECUTABLE	:= bbb-fdc


all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)

clean:
	-rm $(BIN)/*
