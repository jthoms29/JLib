# John Thoms
CC = gcc
CFLAGS = -g
CPPFLAGS = -Wall -pedantic -Wextra -std=c99


OBJ_DIR=build/obj
LIB_DIR=build/lib
BIN_DIR=build/bin

BINARIES = main
all: $(BINARIES)

clean:
	rm -rf build $(BINARIES)


# directories
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)


$(OBJ_DIR)/JHASHMAP.o: JHASHMAP.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) JHASHMAP.c -I. -o $(OBJ_DIR)/JHASHMAP.o


$(OBJ_DIR)/main.o: main.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) main.c -I. -o $(OBJ_DIR)/main.o



$(BIN_DIR)/main: $(OBJ_DIR)/JHASHMAP.o $(OBJ_DIR)/main.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJ_DIR)/JHASHMAP.o $(OBJ_DIR)/main.o -lm -o $(BIN_DIR)/main


main: $(BIN_DIR)/main
	ln -sf $(BIN_DIR)/main main