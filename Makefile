CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -Wconversion -Wshadow \
           -Wstrict-prototypes -Wmissing-prototypes -Wformat=2 \
           -O2 -g -finput-charset=UTF-8 -fexec-charset=UTF-8 \
           $(shell pkg-config --cflags gtk4)
LDFLAGS = $(shell pkg-config --libs gtk4)

TEST_CFLAGS  = -std=c11 -Wall -Wextra -Wconversion -Wshadow \
           -Wstrict-prototypes -Wmissing-prototypes -Wformat=2 \
           -O2 -g -finput-charset=UTF-8 -fexec-charset=UTF-8 \
           $(shell pkg-config --cflags glib-2.0)
TEST_LDFLAGS = $(shell pkg-config --libs glib-2.0)
BIN_DIR = bin

COMMON_SRC = src/common/persistence.c src/common/ui_utils.c

ALL_SRC = src/launcher/main.c \
          src/number_guessing/main.c \
          src/rock_paper_scissors/main.c \
          src/rock_paper_scissors/rps_logic.c \
          src/snake_gun_water/main.c \
          src/tic_tac_toe/main.c \
          src/common/persistence.c \
          src/common/ui_utils.c

TARGETS = $(BIN_DIR)/c-games-collection.exe \
          $(BIN_DIR)/test_persistence.exe \
          $(BIN_DIR)/test_rps.exe

.PHONY: all clean test

all: $(TARGETS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Unified Binary
$(BIN_DIR)/c-games-collection.exe: $(ALL_SRC) | $(BIN_DIR)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

# Tests
$(BIN_DIR)/test_persistence.exe: tests/test_persistence.c src/common/persistence.c | $(BIN_DIR)
	$(CC) $^ -o $@ $(TEST_CFLAGS) $(TEST_LDFLAGS)

$(BIN_DIR)/test_rps.exe: tests/test_rps.c src/rock_paper_scissors/rps_logic.c | $(BIN_DIR)
	$(CC) $^ -o $@ $(TEST_CFLAGS) $(TEST_LDFLAGS)

test: $(BIN_DIR)/test_persistence.exe $(BIN_DIR)/test_rps.exe
	$(BIN_DIR)/test_persistence.exe
	$(BIN_DIR)/test_rps.exe

clean:
	rm -f $(BIN_DIR)/*.exe
	@echo Clean complete.

package: all
	@echo "Invoking packaging script..."
	bash scripts/package-windows.sh
