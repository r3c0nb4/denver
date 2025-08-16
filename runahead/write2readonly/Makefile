CC = gcc 

SRC_DIR = ./
BIN_DIR = ../bin

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRCS))
TARGET = $(BIN_DIR)/spectre

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -g $(OBJS) -o $@ -I ../include

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -g -c $< -o $@ -I ../include

clean:
	rm -f $(OBJS) $(TARGET)
