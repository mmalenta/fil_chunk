CC = g++
SRC_DIR = ./src/
INCLUDE_DIR = ./include/
CPP_FILES = filchunk.cpp filterbank.cpp

OBJ_DIR = ./obj/
OBJECTS = $(addprefix $(OBJ_DIR), $(CPP_FILES:.cpp=.o))
BIN_DIR = ./bin/

all: makebindir makeobjdir filchunk

makebindir:
	mkdir -p $(BIN_DIR)

makeobjdir:
	mkdir -p $(OBJ_DIR)

filchunk: $(OBJECTS)
	$(CC) -o $(BIN_DIR)/filchunk $(OBJECTS) -g

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	$(CC) -c -o $@ $< -I$(INCLUDE_DIR) -g

.PHONY: clean
clean:
	rm $(BIN_DIR)/* $(OBJ_DIR)/*