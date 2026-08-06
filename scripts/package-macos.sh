#!/bin/bash
set -e

echo "Starting macOS Packaging Process..."

# Directories
PROJECT_ROOT=$(pwd)
VERSION="v4.0.0"
APP_NAME="C-Games-Collection"
APP_BUNDLE="release/${APP_NAME}.app"
ZIP_FILE="C-GAMES-COLLECTION-${VERSION}-macOS.zip"

echo "[1/5] Building Project (if not built)..."
make all

echo "[2/5] Creating App Bundle Structure..."
rm -rf release/
mkdir -p "$APP_BUNDLE/Contents/MacOS"
mkdir -p "$APP_BUNDLE/Contents/Resources"
mkdir -p "$APP_BUNDLE/Contents/Resources/data"

echo "[3/5] Copying Executable and Project Resources..."
# Strip the .exe extension for macOS standard
cp bin/c-games-collection.exe "$APP_BUNDLE/Contents/MacOS/c-games-collection"
cp -r assets "$APP_BUNDLE/Contents/Resources/"
cp README.md LICENSE CHANGELOG.md "$APP_BUNDLE/Contents/Resources/"

echo "[4/5] Generating Info.plist and Launcher..."
cat << EOF > "$APP_BUNDLE/Contents/Info.plist"
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>Launcher.sh</string>
    <key>CFBundleIdentifier</key>
    <string>com.elstrix.cgamescollection</string>
    <key>CFBundleName</key>
    <string>${APP_NAME}</string>
    <key>CFBundleVersion</key>
    <string>${VERSION}</string>
    <key>CFBundleShortVersionString</key>
    <string>${VERSION}</string>
</dict>
</plist>
EOF

# Launcher script to check dependencies and run the game with proper working directory
cat << 'EOF' > "$APP_BUNDLE/Contents/MacOS/Launcher.sh"
#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR/../Resources"

if ! command -v brew &> /dev/null || ! brew list gtk4 &> /dev/null; then
    osascript -e 'display alert "Missing Dependencies" message "GTK4 is required to run this game. Please install it using Homebrew: brew install gtk4"'
    exit 1
fi

exec "$DIR/c-games-collection"
EOF
chmod +x "$APP_BUNDLE/Contents/MacOS/Launcher.sh"

echo "[5/5] Creating Archive..."
cd release
zip -r -q "$ZIP_FILE" "${APP_NAME}.app"
cd ..

echo "Packaging Complete! Output: release/$ZIP_FILE"
