# John Thoms
CC = gcc
CFLAGS = -g
CPPFLAGS = -Wall -pedantic -Wextra -std=c99


OBJ_DIR=build/obj
LIB_DIR=build/lib
BIN_DIR=build/bin

BINARIES = test
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


$(OBJ_DIR)/JHASHSET.o: JHASHSET.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) JHASHSET.c -I. -o $(OBJ_DIR)/JHASHSET.o


$(OBJ_DIR)/test.o: test.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) test.c -I. -o $(OBJ_DIR)/test.o



$(BIN_DIR)/test: $(OBJ_DIR)/JHASHSET.o $(OBJ_DIR)/test.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJ_DIR)/JHASHSET.o $(OBJ_DIR)/test.o -lm -o $(BIN_DIR)/test


test: $(BIN_DIR)/test
	ln -sf $(BIN_DIR)/test test