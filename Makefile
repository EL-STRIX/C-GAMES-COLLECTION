CC = gcc
CFLAGS = -Wall -O2 `pkg-config --cflags gtk4`
LDFLAGS = `pkg-config --libs gtk4`
BIN_DIR = bin

TARGETS = $(BIN_DIR)/launcher.exe \
          $(BIN_DIR)/number-guessing.exe \
          $(BIN_DIR)/rock-paper-scissors.exe \
          $(BIN_DIR)/snake-gun-water.exe \
          $(BIN_DIR)/tic-tac-toe-cli.exe \
          $(BIN_DIR)/tic-tac-toe-gui.exe

# Ensure bin directory exists
$(shell mkdir -p $(BIN_DIR))

all: $(TARGETS)

# Launcher
$(BIN_DIR)/launcher.exe: launcher/main.c
	$(CC) $< -o $@ $(CFLAGS) $(LDFLAGS)

# Number Guessing
$(BIN_DIR)/number-guessing.exe: games/number-guessing/main.c common/persistence.c
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

# Rock Paper Scissors
$(BIN_DIR)/rock-paper-scissors.exe: games/rock-paper-scissors/main.c common/persistence.c
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

# Snake Gun Water
$(BIN_DIR)/snake-gun-water.exe: games/snake-gun-water/main.c common/persistence.c
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

# Tic Tac Toe CLI
$(BIN_DIR)/tic-tac-toe-cli.exe: games/tic-tac-toe-cli/main.c common/persistence.c
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

# Tic Tac Toe GUI
$(BIN_DIR)/tic-tac-toe-gui.exe: games/tic-tac-toe-gui/main.c common/persistence.c
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf $(BIN_DIR)/*.exe
