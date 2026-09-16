CC = gcc
CFLAGS = -Wall -Wextra -O1
LDFLAGS = -lm
TARGET = benchmark_arquitectura
SRC = benchmark_arquitectura.c

.PHONY: all run hardware clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

hardware:
	sh ver_hardware.sh

clean:
	rm -f $(TARGET)
