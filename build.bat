@echo off
setlocal

set PATH=C:\msys64\ucrt64\bin;%PATH%
set PKG_CONFIG_PATH=C:\msys64\ucrt64\lib\pkgconfig

for /f "delims=" %%i in ('pkg-config --cflags gtk4') do set CFLAGS=%%i
for /f "delims=" %%i in ('pkg-config --libs gtk4') do set LIBS=%%i

if not exist bin mkdir bin

echo === Building main application ===
gcc.exe src/launcher/main.c src/number_guessing/main.c src/rock_paper_scissors/main.c src/snake_gun_water/main.c src/tic_tac_toe/main.c src/common/persistence.c src/common/ui_utils.c -o bin/c-games-collection.exe -std=c11 -Wall -Wextra -Wconversion -Wshadow -O2 -g -finput-charset=UTF-8 -fexec-charset=UTF-8 %CFLAGS% %LIBS%
if %errorlevel%==0 (echo BUILD SUCCESS) else (echo BUILD FAILED & exit /b 1)

echo === Building test binary ===
gcc.exe tests/test_persistence.c src/common/persistence.c src/common/ui_utils.c -o bin/test_persistence.exe -std=c11 -Wall -Wextra -Wconversion -Wshadow -O2 -g -finput-charset=UTF-8 -fexec-charset=UTF-8 %CFLAGS% %LIBS%
if %errorlevel%==0 (echo TEST BUILD SUCCESS) else (echo TEST BUILD FAILED & exit /b 1)

echo === Running tests ===
bin\test_persistence.exe
if %errorlevel%==0 (echo ALL TESTS PASSED) else (echo TESTS FAILED)

endlocal
