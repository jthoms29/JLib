# John Thoms
CC = gcc
CFLAGS = -g -O2
CPPFLAGS = -std=c99 -Wall -pedantic -Wextra


OBJ_DIR=build/obj
LIB_DIR=build/lib
BIN_DIR=build/bin


BINARIES = test

SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)

all: $(BINARIES)

clean:
	rm -rf build $(BINARIES)


# directories
$(OBJ_DIR) $(LIB_DIR) $(BIN_DIR):
	mkdir -p $@


# build source files
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -I. $< -o $@


$(BIN_DIR)/$(BINARIES): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -lm -o $@

$(BINARIES): $(BIN_DIR)/$(BINARIES)
	ln -sf $< $@