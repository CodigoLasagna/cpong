CXX := gcc
CFLAGS := -g -std=gnu89 -pedantic-errors
LDFLAGS := -lSDL2 -lSDL2main -lGLEW -lGL
SRCE := ./source/engine/
SRCGS := ./source/gameSnake/
TARGET := main

SRCS := $(wildcard  *.c $(SRCE)*.c $(SRCGS)*.c)
OBJS := $(patsubst %.c,%.o,$(SRCS))

all: $(TARGET)
%.o: %.c
	$(CXX) $(CFLAGS) -c $< -o $@
$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)
clean:
	rm -rf $(TARGET) *.o $(SRCE)*.o $(SRCGS)
	
.PHONY: all clean
