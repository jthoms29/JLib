# John Thoms
CC = gcc
CFLAGS = -g
CPPFLAGS = -std=gnu90 -Wall -pedantic -Wextra


OBJ_DIR=build/$(PLATFORM)/obj
LIB_DIR=build/$(PLATFORM)/lib
BIN_DIR=build/$(PLATFORM)/bin

BINARIES = JARENA
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


$(OBJ_DIR)/JARENA.o: JARENA.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) JARENA.c -I. -o $(OBJ_DIR)/JARENA.o


$(BIN_DIR)/JARENA: $(OBJ_DIR)/JARENA.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJ_DIR)/JARENA.o -o $(BIN_DIR)/JARENA


JARENA: $(BIN_DIR)/JARENA
	ln -sf $(BIN_DIR)/JARENA JARENA