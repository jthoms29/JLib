# John Thoms
CC = gcc
CFLAGS = -g
CPPFLAGS = -std=c99 -Wall -pedantic -Wextra


OBJ_DIR=build/obj
LIB_DIR=build/lib
BIN_DIR=build/bin


BINARY = test

SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)

all: $(BINARY)

clean:
	rm -rf build $(BINARIES)


# directories
$(OBJ_DIR) $(LIB_DIR) $(BIN_DIR):
	mkdir -p $@


# build source files
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -I. $< -o $@


$(BIN_DIR)/$(BINARY): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -lm -o $@

$(BINARY): $(BIN_DIR)/$(BINARY)
	ln -sf $< $@