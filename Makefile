
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGET = source2html
SOURCES = main.c html.c lexer.c
HEADERS = html.h lexer.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET) output.html

.PHONY: all clean
