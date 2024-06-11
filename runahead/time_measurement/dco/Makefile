CC = gcc

SRC_DIR = ./
BIN_DIR = ../../bin

N ?= 100

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRCS))
TARGET = $(BIN_DIR)/dco

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -g $(OBJS) -o $@ -I ../../include -DN=$(N)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -g -c $< -o $@ -I ../../include -DN=$(N)

clean:
	rm -f $(OBJS) $(TARGET)
