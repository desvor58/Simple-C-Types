CC ?= gcc
CFLAGS := -Wall -Wextra --std=c99 -Iinclude
MODE ?= release

SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c, obj/%.o, $(SRCS))

ifeq ($(OS),Windows_NT)
    RM_DIR = if exist "$(subst /,\,$(1))" rmdir /s /q "$(subst /,\,$(1))"
    RM_FILE = if exist "$(subst /,\,$(1))" del /q /f "$(subst /,\,$(1))"
    MKDIR = if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
	CLEAN_CMD := del /q
	MKDIR_CMD = if not exist "$(1)" mkdir "$(1)"
	SCT_LIB_FILE := sct-win
else
    RM_DIR = rm -rf "$(1)"
    RM_FILE = rm -f "$(1)"
    MKDIR = mkdir -p "$(1)"
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

.PHONY: all lib clean init test

all: lib

lib: $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	$(AR) rcs $@ $^

obj/%.o: src/%.c init
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@$(call RM_DIR,obj)
	@$(call RM_DIR,lib)

init:
	@$(call MKDIR,bin)
	@$(call MKDIR,lib)
