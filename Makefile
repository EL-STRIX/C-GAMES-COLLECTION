CC = gcc
CFLAGS = -Wall -O2 $(shell pkg-config --cflags gtk4)
LDFLAGS = $(shell pkg-config --libs gtk4)
BIN_DIR = bin

COMMON_SRC = src/common/persistence.c

TARGETS = $(BIN_DIR)/launcher.exe \
          $(BIN_DIR)/number-guessing.exe \
          $(BIN_DIR)/rock-paper-scissors.exe \
          $(BIN_DIR)/snake-gun-water.exe \
          $(BIN_DIR)/tic-tac-toe-gui.exe \
          $(BIN_DIR)/test_persistence.exe

all: $(TARGETS)

$(BIN_DIR):
	-mkdir $(BIN_DIR)

# Launcher
$(BIN_DIR)/launcher.exe: src/launcher/main.c $(COMMON_SRC) | $(BIN_DIR)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

# Number Guessing
$(BIN_DIR)/number-guessing.exe: src/number_guessing/main.c $(COMMON_SRC) | $(BIN_DIR)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

# Rock Paper Scissors
$(BIN_DIR)/rock-paper-scissors.exe: src/rock_paper_scissors/main.c $(COMMON_SRC) | $(BIN_DIR)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

# Snake Gun Water
$(BIN_DIR)/snake-gun-water.exe: src/snake_gun_water/main.c $(COMMON_SRC) | $(BIN_DIR)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

# Tic Tac Toe GUI
$(BIN_DIR)/tic-tac-toe-gui.exe: src/tic_tac_toe/main.c $(COMMON_SRC) | $(BIN_DIR)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

# Tests
$(BIN_DIR)/test_persistence.exe: tests/test_persistence.c $(COMMON_SRC) | $(BIN_DIR)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

test: $(BIN_DIR)/test_persistence.exe
	./$(BIN_DIR)/test_persistence.exe

clean:
	-del /Q /S bin\*.exe
