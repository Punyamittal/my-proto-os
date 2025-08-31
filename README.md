# ProtoOS with AI Assistant Integration - Voice-Controlled AI Operating System

ProtoOS is a complete, bootable operating system written from scratch in assembly and C, now enhanced with **AI Assistant integration** and **voice control capabilities**. It demonstrates the fundamental concepts of operating system development including bootloading, kernel initialization, memory management, basic I/O, **artificial intelligence integration**, and **voice recognition**.

## 🚀 **Voice-Controlled AI Assistant Features**

- **🎤 Always-On Voice Assistant**: Continuous voice listening with wake word detection
- **🔍 Web Search Integration**: Search the internet for information
- **🌤️ Weather Service**: Get current weather and forecasts
- **📰 News Updates**: Latest news on any topic
- **🤖 AI Chat**: Intelligent conversations with Gemini AI
- **🎵 Speech Synthesis**: AI responses spoken back to you
- **🔑 Environment Support**: Automatic API key loading from .env files

## 🏗️ **Core Features**

- **Bootloader**: 512-byte boot sector that loads the kernel from disk
- **Kernel**: 32-bit protected mode kernel with AI assistant system
- **Screen Management**: VGA text mode support with color and cursor control
- **Keyboard Input**: Basic keyboard input handling with command processing
- **Memory Management**: Simple memory layout and stack setup
- **Network Support**: HTTP client for AI API communication
- **JSON Parser**: Handle AI API responses and data
- **Voice Recognition**: Speech-to-text and wake word detection
- **Speech Synthesis**: Text-to-speech for AI responses
- **Build System**: Automated build process for Windows and Unix-like systems

## 🧠 **AI Assistant Capabilities**

### **Voice Commands**
- **Wake Words**: "Hey Proto", "Proto Assistant", "Hey OS"
- **Natural Language**: "Search for weather information", "What's the time?"
- **Hands-Free Operation**: Complete voice control of the system

### **Information Services**
- **Web Search**: "Search for [topic]" - Find information online
- **Weather**: "What's the weather" - Get current conditions
- **News**: "Tell me the news" - Latest updates on any topic
- **Time & Date**: "What time is it" - Current system time

### **AI Integration**
- **Gemini AI**: Google's most capable AI model (default)
- **Multi-Model Support**: OpenAI GPT, Anthropic Claude, Local LLMs
- **Conversation Memory**: AI remembers context and history
- **Smart Responses**: Intelligent answers to any question

## 🏛️ **Architecture**

```
┌─────────────────────────────────────────────────────────────────┐
│                ProtoOS with AI Assistant                       │
├─────────────────────────────────────────────────────────────────┤
│ Bootloader (512 bytes) - Loads kernel from disk               │
├─────────────────────────────────────────────────────────────────┤
│ Kernel (12KB+) - Main operating system with AI & Voice        │
│ ├─ Screen Management (VGA text mode)                           │
│ ├─ Keyboard Input Handling                                     │
│ ├─ Network Stack (HTTP client)                                 │
│ ├─ JSON Parser (API responses)                                 │
│ ├─ LangChain Integration (AI framework)                        │
│ ├─ Voice Recognition (Speech-to-Text)                          │
│ ├─ Speech Synthesis (Text-to-Speech)                           │
│ ├─ AI Assistant System (Web, Weather, News)                   │
│ ├─ Interactive Shell (Voice & Text commands)                   │
│ └─ System Initialization                                       │
└─────────────────────────────────────────────────────────────────┘
```

## 📁 **File Structure**

```
.
├── boot/
│   └── bootloader.asm      # 16-bit bootloader with disk loading
├── kernel/
│   ├── kernel.c            # Main kernel entry point with AI integration
│   ├── kernel.h            # Kernel function declarations
│   ├── screen.c            # VGA screen management implementation
│   ├── screen.h            # Screen function declarations
│   ├── keyboard.c          # Keyboard input handling
│   ├── keyboard.h          # Keyboard function declarations
│   ├── network.c           # HTTP client for AI APIs
│   ├── network.h           # Network function declarations
│   ├── json.c              # JSON parser for AI responses
│   ├── json.h              # JSON function declarations
│   ├── langchain.c         # LangChain AI framework
│   ├── langchain.h         # LangChain declarations
│   ├── voice.c             # Voice recognition and synthesis
│   ├── voice.h             # Voice system declarations
│   ├── assistant.c         # AI assistant system
│   ├── assistant.h         # Assistant system declarations
│   ├── shell.c             # Interactive shell with voice commands
│   ├── shell.h             # Shell function declarations
│   ├── env.c               # Environment variable handling
│   └── env.h               # Environment declarations
├── include/
│   └── a.h                 # Main header file including all components
├── linker.ld               # GNU LD linker script for the kernel
├── Makefile                # Unix/Linux build system
├── Makefile.win            # Windows build system
├── build.ps1               # PowerShell build script for Windows
├── build.bat               # Batch build script for Windows
├── env.template            # Environment variables template
└── README.md               # This file
```

## 🛠️ **Building ProtoOS with AI Assistant**

### **Prerequisites**

You need the following tools installed:
- **NASM** - for assembling the bootloader
- **GCC (MinGW on Windows)** - for compiling the kernel
- **GNU LD** - for linking the kernel
- **QEMU** - for running the OS in a virtual machine

### **Windows (Recommended)**

1. **Install Dependencies**:
   - Install [Chocolatey](https://chocolatey.org/install) or [winget](https://learn.microsoft.com/en-us/windows/package-manager/winget/)
   - Open PowerShell as Administrator and run:
     ```powershell
     # Using Chocolatey
     choco install nasm mingw qemu

     # Or using winget
     winget install -e --id Gnu.Nasm
     winget install -e --id TDM-GCC.TDM-GCC
     winget install -e --id QEMU.QEMU
     ```

2. **Build and Run**:
   ```powershell
   cd my-proto-os

   # Build the voice-controlled AI OS
   .\build.ps1

   # To build and run directly
   .\build.ps1 -Run
   ```

### **Unix-like Systems (Linux/macOS)**

1. **Install Dependencies**:
   ```bash
   # On Debian/Ubuntu
   sudo apt update
   sudo apt install nasm gcc-i686-elf binutils-i686-elf qemu-system-x86

   # On Fedora
   sudo dnf install nasm gcc-i686-elf binutils-i686-elf qemu-system-x86

   # On macOS (using Homebrew)
   brew install nasm x86_64-elf-gcc qemu
   ```

2. **Build and Run**:
   ```bash
   cd my-proto-os

   # Build the voice-controlled AI OS
   make all

   # Run in QEMU
   make run
   ```

## 🎤 **Using the Voice Assistant**

### **Voice Commands**
Once ProtoOS boots, the voice assistant is always listening. Use these voice commands:

```
"Hey Proto"                    # Activate voice assistant
"Search for [topic]"           # Web search
"What's the weather"           # Weather information
"What time is it"              # Current time
"Tell me the news"             # Latest news
"Ask AI [question]"            # Chat with AI
```

### **Text Commands**
You can also use text commands in the shell:

```bash
ProtoOS> ai Hello, what can you help me with?
ProtoOS> search artificial intelligence
ProtoOS> weather London
ProtoOS> news technology
ProtoOS> voice start          # Start voice listening
ProtoOS> assistant status     # Show system status
```

### **Wake Word Detection**
The system continuously listens for wake words:
- **"Hey Proto"** - Primary wake word
- **"Proto Assistant"** - Alternative wake word
- **"Hey OS"** - Another option

## 🔧 **How It Works**

### **1. Boot Process**
1. **BIOS loads bootloader** from the first sector (512 bytes) of the disk
2. **Bootloader initializes** segments and prints loading message
3. **Disk I/O** loads the kernel from sector 2 onwards
4. **Protected mode transition** sets up GDT and switches to 32-bit mode
5. **Kernel execution** begins with AI assistant system initialization

### **2. AI Assistant System Initialization**
1. **Voice system setup** - Initialize speech recognition and synthesis
2. **AI framework setup** - Initialize LangChain and Gemini integration
3. **Information services** - Set up web search, weather, and news
4. **Always-on listening** - Start continuous voice monitoring

### **3. Voice Interaction Flow**
1. **Wake word detection** - System listens for "Hey Proto"
2. **Voice command capture** - Record and process voice input
3. **Speech recognition** - Convert speech to text
4. **Command processing** - Route to appropriate service (search, weather, AI)
5. **Response generation** - Get information or AI response
6. **Speech synthesis** - Speak response back to user

### **4. Information Services**
- **Web Search**: Query search engines for information
- **Weather**: Fetch current conditions and forecasts
- **News**: Get latest articles on any topic
- **AI Chat**: Intelligent conversations with Gemini

## 🎯 **Customization**

### **Voice Settings**
- **Wake Words**: Customize activation phrases
- **Voice Speed**: Adjust speech synthesis speed
- **Voice Pitch**: Modify speech synthesis pitch
- **Audio Volume**: Control system audio levels

### **AI Models**
- **Gemini**: Google's AI (default)
- **OpenAI**: GPT models
- **Claude**: Anthropic's AI
- **Local**: Run AI locally

### **Assistant Capabilities**
- **Enable/Disable Services**: Turn features on/off
- **Custom Commands**: Add new voice commands
- **API Integration**: Connect to real services
- **Language Support**: Multiple languages

## 🚨 **Troubleshooting**

### **Common Build Issues**
1. **"nasm not found"**: Install NASM assembler
2. **"gcc not found"**: Install GCC/MinGW compiler
3. **"qemu not found"**: Install QEMU emulator
4. **Linker errors**: Check that all source files exist and compile correctly

### **Voice Assistant Issues**
1. **No voice response**: Check audio settings and permissions
2. **Wake word not detected**: Try different wake words or check microphone
3. **Command not recognized**: Use clear, simple voice commands
4. **API errors**: Verify your Gemini API key in .env file

### **Performance Issues**
1. **Slow voice recognition**: Voice processing is simulated in current version
2. **High CPU usage**: The system uses busy waiting; proper interrupts would be more efficient
3. **Memory usage**: Voice buffers and AI models require significant memory

## 📚 **Learning Resources**

- **OSDev Wiki**: https://wiki.osdev.org/
- **LangChain Documentation**: https://python.langchain.com/
- **Google Gemini API**: https://ai.google.dev/
- **Speech Recognition**: https://en.wikipedia.org/wiki/Speech_recognition
- **Intel x86 Manuals**: https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html
- **NASM Documentation**: https://www.nasm.us/doc/
- **QEMU Documentation**: https://qemu.readthedocs.io/

## 🌟 **Future Enhancements**

- **Real Voice Recognition**: Integrate actual speech-to-text engines
- **Advanced Speech Synthesis**: Natural-sounding AI voices
- **Multimodal AI**: Support for images, audio, and video
- **Real API Integration**: Connect to actual search, weather, and news services
- **Machine Learning**: On-device AI model training
- **Multi-language Support**: Internationalization and localization
- **Cloud Integration**: Sync with cloud services
- **Mobile App**: Companion app for mobile devices

## 🤝 **Contributing**

This is an educational project showcasing OS development and AI integration. Feel free to:
- Fork and modify for your own learning
- Submit improvements and bug fixes
- Extend with new AI capabilities
- Add more voice commands and features
- Use as a starting point for your own AI-powered OS

## 📄 **License**

This project is open source and available under the MIT License.

## 🙏 **Acknowledgments**

- Inspired by various OS development tutorials and examples
- Built with educational purposes in mind
- Demonstrates fundamental computer architecture concepts
- Integrates modern AI and voice technologies with low-level system programming
- Special thanks to Google Gemini, OpenAI, and Anthropic for AI capabilities

---

**Happy Voice-Controlled AI OS Development! 🚀🎤🤖**
