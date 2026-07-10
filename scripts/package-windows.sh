#!/bin/bash
set -e

echo "Starting Portable Windows Packaging Process..."

# Directories
PROJECT_ROOT=$(pwd)
VERSION="v2.0.0"
RELEASE_DIR="release/C-GAMES-COLLECTION-${VERSION}-Windows"
ZIP_FILE="C-GAMES-COLLECTION-${VERSION}-Windows.zip"

echo "[1/7] Building Project..."
MAKE_CMD="make"
if command -v mingw32-make &> /dev/null; then
    MAKE_CMD="mingw32-make"
fi
$MAKE_CMD clean
$MAKE_CMD all

echo "[2/7] Creating Release Directories..."
rm -rf release/
mkdir -p "$RELEASE_DIR"

echo "[3/7] Copying Executable and Project Resources..."
mkdir -p "$RELEASE_DIR/bin"
cp bin/c-games-collection.exe "$RELEASE_DIR/bin/"
cp -r assets "$RELEASE_DIR/"
cp README.md LICENSE CHANGELOG.md "$RELEASE_DIR/"
echo "${VERSION}" > "$RELEASE_DIR/VERSION"

# Ensure data directory exists for persistence
mkdir -p "$RELEASE_DIR/data"

echo "[4/7] Collecting Dynamic Dependencies (DLLs)..."
# Find all dependent DLLs from the UCRT64 environment (ignore Windows system DLLs)
ldd bin/c-games-collection.exe | grep "=> /ucrt64" | awk '{print $3}' | while read -r dll_path; do
    cp "$dll_path" "$RELEASE_DIR/bin/"
done
# Also include gdbus.exe to prevent GIO dbus warnings
cp /ucrt64/bin/gdbus.exe "$RELEASE_DIR/bin/" 2>/dev/null || true

# We also need to copy GTK specific loaded DLLs that aren't statically linked (e.g., gdk-pixbuf loaders, icon themes)
echo "[5/7] Copying GTK Runtime Assets..."

# 1. GSettings Schemas
mkdir -p "$RELEASE_DIR/share/glib-2.0/schemas"
cp /ucrt64/share/glib-2.0/schemas/gschemas.compiled "$RELEASE_DIR/share/glib-2.0/schemas/"
# Copy GTK4 specific schemas if available
cp /ucrt64/share/glib-2.0/schemas/org.gtk.gtk4.Settings.*.xml "$RELEASE_DIR/share/glib-2.0/schemas/" 2>/dev/null || true
glib-compile-schemas "$RELEASE_DIR/share/glib-2.0/schemas"

# 2. Icons (GTK needs default icons for close buttons, window icons, etc.)
mkdir -p "$RELEASE_DIR/share/icons"
cp -r /ucrt64/share/icons/hicolor "$RELEASE_DIR/share/icons/"
cp -r /ucrt64/share/icons/Adwaita "$RELEASE_DIR/share/icons/" 2>/dev/null || true

# 3. GDK-Pixbuf Loaders (for image loading, though GTK4 often relies on its own loaders now, good for compatibility)
mkdir -p "$RELEASE_DIR/lib/gdk-pixbuf-2.0/2.10.0/loaders"
cp /ucrt64/lib/gdk-pixbuf-2.0/2.10.0/loaders/*.dll "$RELEASE_DIR/lib/gdk-pixbuf-2.0/2.10.0/loaders/" 2>/dev/null || true
# Re-query loaders inside the release dir
gdk-pixbuf-query-loaders > "$RELEASE_DIR/lib/gdk-pixbuf-2.0/2.10.0/loaders.cache" 2>/dev/null || true
# Need to copy DLLs that loaders depend on too (like libjpeg, libpng)
ldd "$RELEASE_DIR"/lib/gdk-pixbuf-2.0/2.10.0/loaders/*.dll 2>/dev/null | grep "=> /ucrt64" | awk '{print $3}' | sort -u | while read -r dll_path; do
    cp -n "$dll_path" "$RELEASE_DIR/bin/" 2>/dev/null || true
done

echo "[6/7] Creating Environment Wrapper Batch Script..."
# Create a robust runner script for users
# GTK will automatically resolve ../share and ../lib relative to bin/
cat << 'EOF' > "$RELEASE_DIR/Play.bat"
@echo off
setlocal
set "BASE_DIR=%~dp0"
set "GSETTINGS_SCHEMA_DIR=%BASE_DIR%share\glib-2.0\schemas"
set "GTK_DATA_PREFIX=%BASE_DIR%"
set "XDG_DATA_DIRS=%BASE_DIR%share"
set "GDK_PIXBUF_MODULEDIR=%BASE_DIR%lib\gdk-pixbuf-2.0\2.10.0\loaders"
set "GDK_PIXBUF_MODULE_FILE=%BASE_DIR%lib\gdk-pixbuf-2.0\2.10.0\loaders.cache"
start "" "%BASE_DIR%bin\c-games-collection.exe"
EOF

cd release
if command -v zip &> /dev/null; then
    zip -r -q "$ZIP_FILE" "C-GAMES-COLLECTION-${VERSION}-Windows"
else
    # Use powershell if zip is not installed in MSYS2 (e.g. local environment)
    powershell.exe -Command "Compress-Archive -Path 'C-GAMES-COLLECTION-${VERSION}-Windows' -DestinationPath '${ZIP_FILE}' -Force"
fi
cd ..

echo "Packaging Complete! Output: release/$ZIP_FILE"
