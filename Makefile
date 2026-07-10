CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -Wconversion -Wshadow -Wpedantic \
           -Wstrict-prototypes -Wmissing-prototypes -Wformat=2 \
           -O2 -g -finput-charset=UTF-8 -fexec-charset=UTF-8 \
           $(shell pkg-config --cflags gtk4)
LDFLAGS = $(shell pkg-config --libs gtk4)
BIN_DIR = bin

COMMON_SRC = src/common/persistence.c src/common/ui_utils.c

ALL_SRC = src/launcher/main.c \
          src/number_guessing/main.c \
          src/rock_paper_scissors/main.c \
          src/snake_gun_water/main.c \
          src/tic_tac_toe/main.c \
          src/common/persistence.c \
          src/common/ui_utils.c

TARGETS = $(BIN_DIR)/c-games-collection.exe \
          $(BIN_DIR)/test_persistence.exe

.PHONY: all clean test

all: $(TARGETS)

$(BIN_DIR):
	-mkdir $(BIN_DIR)

# Unified Binary
$(BIN_DIR)/c-games-collection.exe: $(ALL_SRC) | $(BIN_DIR)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

# Tests
$(BIN_DIR)/test_persistence.exe: tests/test_persistence.c $(COMMON_SRC) | $(BIN_DIR)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

test: $(BIN_DIR)/test_persistence.exe
	./$(BIN_DIR)/test_persistence.exe

clean:
	-del /Q $(BIN_DIR)\*.exe 2>NUL
	@echo Clean complete.
