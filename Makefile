CXX		  := arm-linux-gnueabihf-g++
CXX_FLAGS := -Wall -Wextra -std=c++17 -ggdb

BIN		:= test
SRC		:= bbbfdc
INCLUDE	:= bbbfdc
LIB		:= bbbfdc

LIBRARIES	:= -liobb
EXECUTABLE	:= bbb-fdc


all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): main.cpp $(SRC)/wiringPi.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)

clean:
	-rm $(BIN)/*
