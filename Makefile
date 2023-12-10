CXX := gcc
CFLAGS := 
LDFLAGS := -lSDL2 -lSDL2main -lGLEW -lGL -lSDL2_mixer -lSDL2_image -lSDL2_ttf
SRCE := ./
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
