# John Thoms
CC = gcc
CFLAGS = -g
CPPFLAGS = -Wall -pedantic -Wextra

included=../JHASHMAP ../JLIST

OBJ_DIR=build/$(PLATFORM)/obj
LIB_DIR=build/$(PLATFORM)/lib
BIN_DIR=build/$(PLATFORM)/bin

BINARIES = JTHREAD_IPC
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

$(OBJ_DIR)/JHASHMAP.o: ../JHASHMAP/JHASHMAP.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) ../JHASHMAP/JHASHMAP.c -I. -I../JHASHMAP -o $(OBJ_DIR)/JHASHMAP.o

$(OBJ_DIR)/JLIST.o: ../JLIST/JLIST.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) ../JLIST/JLIST.c -I. -I../JLIST -o $(OBJ_DIR)/JLIST.o

$(OBJ_DIR)/JTHREAD_IPC.o: JTHREAD_IPC.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) JTHREAD_IPC.c -I. -o $(OBJ_DIR)/JTHREAD_IPC.o


$(BIN_DIR)/JTHREAD_IPC: $(OBJ_DIR)/JTHREAD_IPC.o $(OBJ_DIR)/JHASHMAP.o $(OBJ_DIR)/JLIST.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJ_DIR)/JHASHMAP.o $(OBJ_DIR)/JLIST.o $(OBJ_DIR)/JTHREAD_IPC.o -lpthread -lm -o $(BIN_DIR)/JTHREAD_IPC


JTHREAD_IPC: $(BIN_DIR)/JTHREAD_IPC
	ln -sf $(BIN_DIR)/JTHREAD_IPC JTHREAD_IPC