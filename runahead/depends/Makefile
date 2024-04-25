DEFAULT_N := 10
CC = gcc

ifdef N
    REPEAT_N := $(N)
else
    REPEAT_N := $(DEFAULT_N)
endif

SRC_DIR = ./
BIN_DIR = ../bin

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRCS))
TARGET = $(BIN_DIR)/depend

CFLAGS = -DN=$(REPEAT_N) -g -I ../include -fPIC

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
