# Makefile

# Compiler
CC = gcc

# Flags
CFLAGS = -I /usr/include/sai
LDFLAGS = -L /usr/lib
LIBS = -lsai

# Source and output
SRC = test.c
OUT = unit_test

# Build target
all: $(OUT)

$(OUT): $(SRC)
	$(CC) -o $(OUT) $(CFLAGS) $(LDFLAGS) $(SRC) $(LIBS)

# Clean target
clean:
	rm -f $(OUT)

# Phony targets
.PHONY: all clean
