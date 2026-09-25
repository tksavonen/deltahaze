# Makefile for deltahaze simulator

CC 		:= cc
CFLAGS 	:= -std=c11 -Wall -Wextra -Iinclude -MMD -MP
SRC 	:= $(wildcard code/*.c)
OBJ 	:= $(patsubst code/%.c,build/%.o,$(SRC))
LIVE_OBJ := $(filter-out build/main.o,$(OBJ)) build/live.o
TARGET 	:= build/deltahaze

.PHONY: all clean run
.PHONY: live

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@

build/%.o: code/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@ -lm

build:
	mkdir -p build

run: $(TARGET)
	./$(TARGET)

build/live.o: ui/live.c | build
	$(CC) $(CFLAGS) $(shell pkg-config --cflags raylib) -c $< -o $@

build/deltahaze-live: $(LIVE_OBJ)
	$(CC) $(LIVE_OBJ) -o $@ $(shell pkg-config --libs raylib) -lm

live: build/deltahaze-live
	./build/deltahaze-live
	