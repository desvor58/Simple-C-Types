CC := gcc
CFLAGS := -Wall -Wextra --std=c99 -Iinclude
MODE ?= release

SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c, obj/%.o, $(SRCS))

ifeq ($(OS),Windows_NT)
	CLEAN_CMD := del /q
	MKDIR_CMD = if not exist "$(1)" mkdir "$(1)"
	SCT_LIB_FILE := sct-win
else
	CLEAN_CMD := rm -rf
	MKDIR_CMD = mkdir -p "$(1)"
	SCT_LIB_FILE := sct-elf
endif

TARGET_LIB := lib/lib$(SCT_LIB_FILE).a

ifeq ($(MODE), debug)
	CFLAGS += -O0 -g
else
	CFLAGS += -O3
endif

all: lib

lib: $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	$(AR) rcs $@ $^

obj/%.o: src/%.c init
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	if exist obj\* $(CLEAN_CMD) obj
	if exist lib\* $(CLEAN_CMD) lib
	if exist tests\test_all.exe del tests\test_all.exe

init:
	$(call MKDIR_CMD,obj)
	$(call MKDIR_CMD,lib)

TEST_SRCS := tests/test_all.c
TEST_TARGET := tests/test_all.exe

test: $(TARGET_LIB)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_SRCS) -Llib -l$(SCT_LIB_FILE)
	$(TEST_TARGET)

.PHONY: all lib clean init test
