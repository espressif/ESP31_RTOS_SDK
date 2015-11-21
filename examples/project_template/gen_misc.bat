@echo off

Rem ******NOTICE******
Rem MUST set SDK_PATH & BIN_PATH firstly!!!
Rem example:
Rem set SDK_PATH=/c/ESP32_RTOS_SDK
Rem set BIN_PATH=/c/ESP32_BIN

set SDK_PATH=""
set BIN_PATH=""

echo gen_misc.bat version 20151105
echo .

if not %SDK_PATH% == "" (
    echo SDK_PATH: %SDK_PATH%
) else (
    echo ERROR: Please set SDK_PATH in gen_misc.bat firstly, exit!!!
    goto end
)

if not %BIN_PATH% == "" (
    echo BIN_PATH: %BIN_PATH%
) else (
    echo ERROR: Please set BIN_PATH in gen_misc.bat firstly, exit!!!
    goto end
)

echo.
echo start...
echo.

make clean

make COMPILE=xcc

:end