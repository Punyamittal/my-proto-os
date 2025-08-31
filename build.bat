@echo off
REM ProtoOS Build Script for Windows (Batch version)
REM This script builds a complete operating system

echo ProtoOS Build Script
echo ====================

REM Check if build directory exists, create if not
if not exist build mkdir build

REM Build bootloader
echo Building bootloader...
nasm -f bin -o build\bootloader.bin boot\bootloader.asm
if errorlevel 1 (
    echo Error: Failed to build bootloader
    echo Make sure NASM is installed and in your PATH
    pause
    exit /b 1
)

REM Compile kernel sources
echo Building kernel...
gcc -m32 -fno-pie -fno-stack-protector -nostdlib -nostdinc -fno-builtin -fno-pic -mno-red-zone -c kernel\kernel.c -o build\kernel.o
if errorlevel 1 (
    echo Error: Failed to compile kernel.c
    echo Make sure GCC/MinGW is installed and in your PATH
    pause
    exit /b 1
)

gcc -m32 -fno-pie -fno-stack-protector -nostdlib -nostdinc -fno-builtin -fno-pic -mno-red-zone -c kernel\screen.c -o build\screen.o
if errorlevel 1 (
    echo Error: Failed to compile screen.c
    pause
    exit /b 1
)

gcc -m32 -fno-pie -fno-stack-protector -nostdlib -nostdinc -fno-builtin -fno-pic -mno-red-zone -c kernel\keyboard.c -o build\keyboard.o
if errorlevel 1 (
    echo Error: Failed to compile keyboard.c
    pause
    exit /b 1
)

REM Link kernel
echo Linking kernel...
ld -m elf_i386 -T linker.ld -o build\kernel.bin build\kernel.o build\screen.o build\keyboard.o
if errorlevel 1 (
    echo Error: Failed to link kernel
    echo Make sure LD (GNU Linker) is installed and in your PATH
    pause
    exit /b 1
)

REM Create OS image
echo Creating OS image...
REM Create empty image file (1.44MB)
fsutil file createnew build\protoos.img 1474560 >nul 2>&1

REM Copy bootloader to first sector
copy /b build\bootloader.bin + build\protoos.img build\temp.img >nul 2>&1
copy /b build\temp.img build\protoos.img >nul 2>&1

REM Copy kernel starting from second sector
copy /b build\protoos.img + build\kernel.bin build\temp2.img >nul 2>&1
copy /b build\temp2.img build\protoos.img >nul 2>&1

REM Clean up temp files
del build\temp.img >nul 2>&1
del build\temp2.img >nul 2>&1

echo.
echo ProtoOS built successfully!
echo.
echo To run the OS:
echo   qemu-system-i386 -fda build\protoos.img -m 16
echo.
echo Or use the PowerShell script: .\build.ps1 -Run
echo.

REM Ask if user wants to run immediately
set /p choice="Do you want to run ProtoOS now? (y/n): "
if /i "%choice%"=="y" (
    echo Starting ProtoOS in QEMU...
    echo Press Ctrl+C to exit QEMU
    qemu-system-i386 -fda build\protoos.img -m 16
)

pause
