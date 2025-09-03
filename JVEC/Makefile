# John Thoms
CC = gcc
CFLAGS = -g
CPPFLAGS = -Wall -pedantic -Wextra


OBJ_DIR=build/$(PLATFORM)/obj
LIB_DIR=build/$(PLATFORM)/lib
BIN_DIR=build/$(PLATFORM)/bin

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


$(OBJ_DIR)/JVEC.o: JVEC.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) JVEC.c -I. -o $(OBJ_DIR)/JVEC.o


$(BIN_DIR)/JVEC: $(OBJ_DIR)/JVEC.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJ_DIR)/JVEC.o -o $(BIN_DIR)/JVEC


$(OBJ_DIR)/test.o: test.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) test.c -I. -o $(OBJ_DIR)/test.o


$(BIN_DIR)/test: $(OBJ_DIR)/JVEC.o  $(OBJ_DIR)/test.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJ_DIR)/JVEC.o $(OBJ_DIR)/test.o -o $(BIN_DIR)/test


test: $(BIN_DIR)/test
	ln -sf $(BIN_DIR)/test test