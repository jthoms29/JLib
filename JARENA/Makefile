CC = gcc
CFLAGS = -g
CPPFLAGS = -std=c99 -Wall -pedantic -Wextra


OBJ_DIR=build/obj
LIB_DIR=build/lib
BIN_DIR=build/bin


BINARY = test

# get name of object files from source files
SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)

all: $(BINARY)

clean:
	rm -rf build $(BINARIES)


# directories
$(OBJ_DIR) $(LIB_DIR) $(BIN_DIR):
	mkdir -p $@


# build obj files
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -I. $< -o $@

# link 
$(BIN_DIR)/$(BINARY): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BINARY): $(BIN_DIR)/$(BINARY)
	ln -sf $< $@