# ProtoOS with AI Assistant Integration - Build Script for Windows
# This script builds a complete voice-controlled AI operating system

param(
    [switch]$Clean,
    [switch]$Run,
    [switch]$Help
)

# Configuration
$BOOT_DIR = "boot"
$KERNEL_DIR = "kernel"
$BUILD_DIR = "build"
$BOOTLOADER_SOURCE = "$BOOT_DIR\bootloader.asm"
$KERNEL_SOURCES = @(
    "$KERNEL_DIR\kernel.c", 
    "$KERNEL_DIR\screen.c", 
    "$KERNEL_DIR\keyboard.c",
    "$KERNEL_DIR\network.c",
    "$KERNEL_DIR\json.c",
    "$KERNEL_DIR\langchain.c",
    "$KERNEL_DIR\shell.c",
    "$KERNEL_DIR\env.c",
    "$KERNEL_DIR\voice.c",
    "$KERNEL_DIR\assistant.c"
)
$OS_IMAGE = "$BUILD_DIR\protoos-ai-assistant.img"

# Colors for output
$Red = "Red"
$Green = "Green"
$Yellow = "Yellow"
$Cyan = "Cyan"

function Write-ColorOutput {
    param([string]$Message, [string]$Color = "White")
    Write-Host $Message -ForegroundColor $Color
}

function Write-Info {
    param([string]$Message)
    Write-ColorOutput "[INFO] $Message" $Cyan
}

function Write-Success {
    param([string]$Message)
    Write-ColorOutput "[SUCCESS] $Message" $Green
}

function Write-Error {
    param([string]$Message)
    Write-ColorOutput "[ERROR] $Message" $Red
}

function Write-Warning {
    param([string]$Message)
    Write-ColorOutput "[WARNING] $Message" $Yellow
}

function Test-Command {
    param([string]$Command)
    try {
        Get-Command $Command -ErrorAction Stop | Out-Null
        return $true
    } catch {
        return $false
    }
}

function Install-Dependencies {
    Write-Info "Checking dependencies..."
    
    $missing = @()
    
    if (-not (Test-Command "nasm")) {
        $missing += "nasm"
    }
    
    if (-not (Test-Command "gcc")) {
        $missing += "gcc"
    }
    
    if (-not (Test-Command "qemu-system-i386")) {
        $missing += "qemu"
    }
    
    if ($missing.Count -gt 0) {
        Write-Warning "Missing dependencies: $($missing -join ', ')"
        Write-Info "Installing dependencies..."
        
        if (Test-Command "choco") {
            foreach ($dep in $missing) {
                if ($dep -eq "gcc") { $dep = "mingw" }
                Write-Info "Installing $dep via Chocolatey..."
                choco install $dep -y
            }
        } elseif (Test-Command "winget") {
            foreach ($dep in $missing) {
                if ($dep -eq "nasm") { $dep = "nasm.nasm" }
                elseif ($dep -eq "gcc") { $dep = "mingw-w64.x86_64-toolchain" }
                elseif ($dep -eq "qemu") { $dep = "qemu.qemu" }
                Write-Info "Installing $dep via winget..."
                winget install $dep
            }
        } else {
            Write-Error "No package manager found. Please install manually:"
            Write-Info "  - NASM: https://www.nasm.us/"
            Write-Info "  - MinGW-w64: https://www.mingw-w64.org/"
            Write-Info "  - QEMU: https://www.qemu.org/"
            exit 1
        }
    } else {
        Write-Success "All dependencies are available"
    }
}

function New-BuildDirectory {
    if (-not (Test-Path $BUILD_DIR)) {
        Write-Info "Creating build directory..."
        New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null
    }
}

function Build-Bootloader {
    Write-Info "Building bootloader..."
    
    if (-not (Test-Path $BOOTLOADER_SOURCE)) {
        Write-Error "Bootloader source not found: $BOOTLOADER_SOURCE"
        exit 1
    }
    
    $bootloader_bin = "$BUILD_DIR\bootloader.bin"
    $result = nasm -f bin -o $bootloader_bin $BOOTLOADER_SOURCE
    
    if ($LASTEXITCODE -eq 0) {
        Write-Success "Bootloader built successfully"
        return $bootloader_bin
    } else {
        Write-Error "Failed to build bootloader"
        exit 1
    }
}

function Build-Kernel {
    Write-Info "Building kernel with AI Assistant and voice integration..."
    
    $object_files = @()
    
    # Compile each source file
    foreach ($source in $KERNEL_SOURCES) {
        if (-not (Test-Path $source)) {
            Write-Error "Kernel source not found: $source"
            exit 1
        }
        
        $obj_file = "$BUILD_DIR\$([System.IO.Path]::GetFileNameWithoutExtension($source)).o"
        $object_files += $obj_file
        
        Write-Info "Compiling $source..."
        $result = gcc -m32 -fno-pie -fno-stack-protector -nostdlib -nostdinc -fno-builtin -fno-pic -mno-red-zone -c $source -o $obj_file
        
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Failed to compile $source"
            exit 1
        }
    }
    
    # Link kernel
    $kernel_bin = "$BUILD_DIR\kernel.bin"
    Write-Info "Linking kernel..."
    
    $linker_script = "linker.ld"
    if (-not (Test-Path $linker_script)) {
        Write-Error "Linker script not found: $linker_script"
        exit 1
    }
    
    $result = ld -m elf_i386 -T $linker_script -o $kernel_bin $object_files
    
    if ($LASTEXITCODE -eq 0) {
        Write-Success "Kernel with AI Assistant and voice integration built successfully"
        return $kernel_bin
    } else {
        Write-Error "Failed to link kernel"
        exit 1
    }
}

function Create-OSImage {
    param([string]$BootloaderBin, [string]$KernelBin)
    
    Write-Info "Creating voice-controlled AI OS image..."
    
    # Create 1.44MB floppy disk image
    $image_size = 1474560  # 1.44MB in bytes
    
    try {
        $stream = [System.IO.File]::Create($OS_IMAGE)
        $buffer = New-Object byte[] $image_size
        $stream.Write($buffer, 0, $buffer.Length)
        $stream.Close()
        
        # Write bootloader to first sector (512 bytes)
        $bootloader_data = [System.IO.File]::ReadAllBytes($BootloaderBin)
        $stream = [System.IO.File]::OpenWrite($OS_IMAGE)
        $stream.Write($bootloader_data, 0, $bootloader_data.Length)
        $stream.Close()
        
        # Write kernel starting from second sector
        $kernel_data = [System.IO.File]::ReadAllBytes($KernelBin)
        $stream = [System.IO.File]::OpenWrite($OS_IMAGE)
        $stream.Seek(512, [System.IO.SeekOrigin]::Begin)
        $stream.Write($kernel_data, 0, $kernel_data.Length)
        $stream.Close()
        
        Write-Success "Voice-controlled AI OS image created successfully: $OS_IMAGE"
        
    } catch {
        Write-Error "Failed to create OS image: $($_.Exception.Message)"
        exit 1
    }
}

function Run-OS {
    if (-not (Test-Path $OS_IMAGE)) {
        Write-Error "OS image not found. Build the OS first."
        exit 1
    }
    
    Write-Info "Starting ProtoOS with AI Assistant and voice control in QEMU..."
    Write-Info "Press Ctrl+C to exit QEMU"
    Write-Info "Voice Commands: Say 'Hey Proto' to activate voice assistant"
    Write-Info "Text Commands: Use 'ai', 'search', 'weather', 'news' commands"
    Write-Info "Use 'setkey' to configure your Gemini API key"
    
    try {
        qemu-system-i386 -fda $OS_IMAGE -m 16
    } catch {
        Write-Error "Failed to start QEMU: $($_.Exception.Message)"
    }
}

function Clean-Build {
    if (Test-Path $BUILD_DIR) {
        Write-Info "Cleaning build directory..."
        Remove-Item -Recurse -Force $BUILD_DIR
        Write-Success "Build directory cleaned"
    } else {
        Write-Info "Build directory does not exist"
    }
}

function Show-Help {
    Write-ColorOutput "ProtoOS with AI Assistant and Voice Integration - Build Script for Windows" $Cyan
    Write-ColorOutput "=================================================================" $Cyan
    Write-ColorOutput ""
    Write-ColorOutput "This builds a voice-controlled AI operating system with:" $Yellow
    Write-ColorOutput "  - Custom kernel and bootloader" $White
    Write-ColorOutput "  - AI Assistant with Gemini integration" $White
    Write-ColorOutput "  - Voice recognition and speech synthesis" $White
    Write-ColorOutput "  - Web search, weather, and news capabilities" $White
    Write-ColorOutput "  - Always-on voice listening with wake words" $White
    Write-ColorOutput "  - Interactive shell with AI commands" $White
    Write-ColorOutput "  - Environment variable support (.env files)" $White
    Write-ColorOutput ""
    Write-ColorOutput "Usage:" $Yellow
    Write-ColorOutput "  .\build.ps1              - Build complete voice-controlled AI OS"
    Write-ColorOutput "  .\build.ps1 -Clean       - Clean build files"
    Write-ColorOutput "  .\build.ps1 -Run         - Build and run the OS"
    Write-ColorOutput "  .\build.ps1 -Help        - Show this help message"
    Write-ColorOutput ""
    Write-ColorOutput "Examples:" $Yellow
    Write-ColorOutput "  .\build.ps1 -Run         - Build and run immediately"
    Write-ColorOutput "  .\build.ps1 -Clean       - Clean previous build"
    Write-ColorOutput ""
    Write-ColorOutput "Voice Commands in the OS:" $Green
    Write-ColorOutput "  'Hey Proto' - Activate voice assistant" $White
    Write-ColorOutput "  'Search for [topic]' - Web search" $White
    Write-ColorOutput "  'What's the weather' - Weather information" $White
    Write-ColorOutput "  'Tell me the news' - Latest news" $White
    Write-ColorOutput "  'Ask AI [question]' - Chat with AI" $White
    Write-ColorOutput ""
    Write-ColorOutput "Text Commands in the OS:" $Green
    Write-ColorOutput "  ai <message> - Chat with AI" $White
    Write-ColorOutput "  search <query> - Web search" $White
    Write-ColorOutput "  weather <location> - Weather info" $White
    Write-ColorOutput "  news <topic> - Latest news" $White
    Write-ColorOutput "  voice start/stop - Voice control" $White
    Write-ColorOutput "  assistant status - System status" $White
    Write-ColorOutput ""
    Write-ColorOutput "Environment Setup:" $Green
    Write-ColorOutput "  Create a .env file with: GEMINI_API_KEY=your_key_here" $White
    Write-ColorOutput "  Or use 'setkey' command in the OS" $White
    Write-ColorOutput ""
    Write-ColorOutput "Wake Words:" $Green
    Write-ColorOutput "  'Hey Proto', 'Proto Assistant', 'Hey OS'" $White
}

# Main execution
if ($Help) {
    Show-Help
    exit 0
}

if ($Clean) {
    Clean-Build
    exit 0
}

Write-ColorOutput "ProtoOS with AI Assistant and Voice Integration - Build Script" $Cyan
Write-ColorOutput "=============================================================" $Cyan

try {
    Install-Dependencies
    New-BuildDirectory
    
    $bootloader_bin = Build-Bootloader
    $kernel_bin = Build-Kernel
    Create-OSImage -BootloaderBin $bootloader_bin -KernelBin $kernel_bin
    
    Write-Success "ProtoOS with AI Assistant and voice integration built successfully!"
    Write-Info "Features included:"
    Write-Info "  ✓ Custom kernel and bootloader"
    Write-Info "  ✓ AI Assistant with Gemini integration"
    Write-Info "  ✓ Voice recognition and speech synthesis"
    Write-Info "  ✓ Always-on voice listening"
    Write-Info "  ✓ Web search, weather, and news"
    Write-Info "  ✓ Interactive shell with AI commands"
    Write-Info "  ✓ Environment variable support"
    Write-Info "  ✓ Multiple AI model support (OpenAI, Claude, Gemini, Local)"
    
    if ($Run) {
        Run-OS
    } else {
        Write-Info "To run the voice-controlled AI OS, use: .\build.ps1 -Run"
        Write-Info "Once running, say 'Hey Proto' to activate voice commands!"
        Write-Info "Use 'setkey <your_api_key>' to configure your Gemini API key"
        Write-Info "Text commands: ai, search, weather, news, voice, assistant"
    }
    
} catch {
    Write-Error "Build failed: $($_.Exception.Message)"
    exit 1
}
