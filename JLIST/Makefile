# John Thoms
CC = gcc
CFLAGS = -g
CPPFLAGS = -std=c99 -Wall -pedantic -Wextra


OBJ_DIR=build/$(PLATFORM)/obj
LIB_DIR=build/$(PLATFORM)/lib
BIN_DIR=build/$(PLATFORM)/bin

BINARIES = JLIST
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


$(OBJ_DIR)/JLIST.o: JLIST.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) JLIST.c -I. -o $(OBJ_DIR)/JLIST.o


$(BIN_DIR)/JLIST: $(OBJ_DIR)/JLIST.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJ_DIR)/JLIST.o -o $(BIN_DIR)/JLIST


JLIST: $(BIN_DIR)/JLIST
	ln -sf $(BIN_DIR)/JLIST JLIST