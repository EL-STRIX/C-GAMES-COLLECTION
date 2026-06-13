CC = gcc
CFLAGS = -Wall -O2 $(shell pkg-config --cflags gtk4)
LDFLAGS = $(shell pkg-config --libs gtk4)
BIN_DIR = bin

TARGETS = $(BIN_DIR)/launcher.exe \
          $(BIN_DIR)/number-guessing.exe \
          $(BIN_DIR)/rock-paper-scissors.exe \
          $(BIN_DIR)/snake-gun-water.exe \
          $(BIN_DIR)/tic-tac-toe-gui.exe

$(BIN_DIR):
	-mkdir $(BIN_DIR)

all: $(TARGETS)

# Launcher
$(BIN_DIR)/launcher.exe: launcher/main.c | $(BIN_DIR)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

# Number Guessing
$(BIN_DIR)/number-guessing.exe: games/number-guessing/main.c | $(BIN_DIR)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

# Rock Paper Scissors
$(BIN_DIR)/rock-paper-scissors.exe: games/rock-paper-scissors/main.c | $(BIN_DIR)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

# Snake Gun Water
$(BIN_DIR)/snake-gun-water.exe: games/snake-gun-water/main.c | $(BIN_DIR)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)


# Tic Tac Toe GUI
$(BIN_DIR)/tic-tac-toe-gui.exe: games/tic-tac-toe-gui/main.c | $(BIN_DIR)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf $(BIN_DIR)/*.exe
