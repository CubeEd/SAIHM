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
all: sai $(OUT)

sai: 
	( cd ../stub && ./autogen.sh && ./configure --prefix=/usr && $(MAKE) && $(MAKE) install )

$(OUT): $(SRC)
	$(CC) -o $(OUT) $(CFLAGS) $(LDFLAGS) $(SRC) $(LIBS)

# Clean target
clean:
	rm -f $(OUT)
	$(MAKE) -C ../stub clean

# Phony targets
.PHONY: all clean
