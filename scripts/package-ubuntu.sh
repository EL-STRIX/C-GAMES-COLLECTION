#!/bin/bash
set -e

echo "Starting Ubuntu Packaging Process..."

# Directories
PROJECT_ROOT=$(pwd)
VERSION="v4.0.0" # This should ideally be passed in or read from a config
RELEASE_DIR="release/C-GAMES-COLLECTION-${VERSION}-Ubuntu"
TAR_FILE="C-GAMES-COLLECTION-${VERSION}-Ubuntu.tar.gz"

echo "[1/4] Building Project (if not built)..."
make all

echo "[2/4] Creating Release Directories..."
rm -rf release/
mkdir -p "$RELEASE_DIR/bin"
mkdir -p "$RELEASE_DIR/data"

echo "[3/4] Copying Executable and Project Resources..."
cp bin/c-games-collection.exe "$RELEASE_DIR/bin/"
cp -r assets "$RELEASE_DIR/"
cp README.md LICENSE CHANGELOG.md "$RELEASE_DIR/"
echo "${VERSION}" > "$RELEASE_DIR/VERSION"

# Create a launcher script
cat << 'EOF' > "$RELEASE_DIR/Play.sh"
#!/bin/bash
cd "$(dirname "$0")"

# Check if GTK4 is installed
if ! dpkg -l | grep -q libgtk-4; then
    echo "GTK4 is required to run this game but it is not installed."
    echo "Please install it by running:"
    echo "  sudo apt update && sudo apt install libgtk-4-1"
    read -p "Press Enter to exit..."
    exit 1
fi

./bin/c-games-collection.exe
EOF
chmod +x "$RELEASE_DIR/Play.sh"

echo "[4/4] Creating Archive..."
cd release
tar -czf "$TAR_FILE" "C-GAMES-COLLECTION-${VERSION}-Ubuntu"
cd ..

echo "Packaging Complete! Output: release/$TAR_FILE"
