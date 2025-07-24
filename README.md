# Cre8or V2.0

A Linux desktop entry creator tool written in **C with GTK3**. This application provides a wizard-based interface for creating `.desktop` files according to the [freedesktop.org Desktop Entry Specification](https://specifications.freedesktop.org/desktop-entry-spec/latest/).

## 🚀 What's New in V2.0

**Cre8or V2.0** is a complete rewrite from Python to C, bringing significant performance improvements, enhanced security, and a more native Linux experience.

### ✨ Key Improvements
- **🚀 Performance**: Native C compilation eliminates Python overhead
- **🛡️ Security**: Comprehensive input validation and path traversal protection
- **🎯 Smart Detection**: Automatic file type detection (ELF, Python, Shell scripts)
- **🎨 UI Enhancements**: Compact design with professional logo
- **🔧 Better Integration**: Improved terminal support for scripts

## Features

- **Wizard Interface**: Step-by-step GUI for creating desktop entries
- **Smart File Type Detection**: Automatically detects ELF, Python, and Shell scripts
- **Intelligent Exec Line Generation**: Creates proper Exec lines based on file type
- **Terminal Support**: Optional terminal execution for scripts with `gnome-terminal`
- **Multiple Entry Types**: Support for Application, Link, and Directory types
- **Live Preview**: Edit the generated .desktop content before saving
- **Overwrite Confirmation**: User-friendly dialogs for existing files
- **Multiple Save Locations**: Save to desktop, local applications directory, or custom location
- **Automatic Permissions**: Sets executable permissions and marks files as trusted
- **Category Selection**: Choose from standard freedesktop.org categories
- **Icon Support**: Browse and select icon files (PNG, XPM, SVG, ICO)
- **Input Validation**: Robust validation for all user inputs

## System Requirements

- **OS**: Linux (tested on Ubuntu, Debian, Arch)
- **Dependencies**: GTK3, GLib, GIO
- **Architecture**: x86_64, ARM64
- **Memory**: Minimal RAM usage

## Dependencies

- GTK+ 3.0 development libraries
- GLib 2.0 development libraries
- GCC compiler

### Ubuntu/Debian
```bash
sudo apt-get install libgtk-3-dev libglib2.0-dev build-essential
```

### Fedora/RHEL/CentOS
```bash
sudo dnf install gtk3-devel glib2-devel gcc make
```

### Arch Linux
```bash
sudo pacman -S gtk3 glib2 gcc make
```

## Building

### Quick Install
```bash
git clone https://github.com/ashes00/cre8or-c
cd cre8or-c
make
sudo make install
```

### Manual Build
```bash
# Install dependencies
sudo apt install build-essential libgtk-3-dev libglib2.0-dev

# Build and install
make
sudo make install  # Optional
```

## Usage

Run the application:
```bash
./cre8or
```

Or if installed:
```bash
cre8or
```

### Wizard Steps

1. **Basic Information**: Enter application name and description
2. **Select Executable**: Choose executable file (auto-detects file type)
3. **Icon (Optional)**: Browse and select an icon file
4. **Categories (Optional)**: Select one or more categories
5. **Preview**: Review the generated desktop entry content
6. **Distribution**: Choose save locations and create the file

### File Type Support

- **ELF Binaries**: Direct execution with proper path quoting
- **Python Scripts**: Automatic `python3` interpreter detection
- **Shell Scripts**: Smart terminal handling with `gnome-terminal`
- **Other Scripts**: Fallback support for various executable types

### Save Locations

- **Desktop**: Saves to `~/Desktop/`
- **Local Applications**: Saves to `~/.local/share/applications/`
- **Custom Location**: User-specified directory

## File Structure

```
Cre8or/
├── main.c              # Application entry point and main window
├── desktop_entry.h     # Desktop entry data structures
├── desktop_entry.c     # Desktop entry generation and validation
├── file_utils.h        # File operations header
├── file_utils.c        # File saving, permissions, and type detection
├── wizard.h           # Wizard interface header
├── wizard.c           # Wizard GUI implementation
├── Makefile           # Build configuration
├── images/            # Application icons and assets
│   └── robot-icon2.png
└── README.md          # This file
```

## Security Features

- **Path Traversal Protection**: Prevents malicious path inputs
- **Input Validation**: Robust validation for all user inputs
- **Safe File Operations**: Uses GLib's secure file handling
- **Command Injection Prevention**: Secure process spawning
- **Executable Validation**: Verifies selected files are actually executable

## Desktop Entry Specification Compliance

This tool follows the [freedesktop.org Desktop Entry Specification](https://specifications.freedesktop.org/desktop-entry-spec/latest/) and supports:

- **Application** entries with Exec, Terminal, and Categories fields
- **Link** entries with URL field
- **Directory** entries with Path field
- Standard categories as defined in the specification
- Proper file permissions and trust marking

## Version History

### V2.0 (Current)
- Complete rewrite from Python to C
- Enhanced security and input validation
- Smart file type detection
- Improved UI with compact design
- Better terminal integration

### V1.x (Python)
- Original Python implementation
- Basic desktop entry creation
- GTK interface

## Contributing

Feel free to submit issues, feature requests, or pull requests to improve the application.

## Support

- **Issues**: [GitHub Issues](https://github.com/ashes00/cre8or-c/issues)
- **Discussions**: [GitHub Discussions](https://github.com/ashes00/cre8or-c/discussions)
- **Website**: [https://github.com/ashes00/](https://github.com/ashes00/)

---

**Cre8or V2.0 - Making Linux desktop entry creation simple, secure, and fast! 🐧** 
