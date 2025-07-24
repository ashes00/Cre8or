# Cre8or

A Linux desktop entry creator tool written in C with GTK3. This application provides a wizard-based interface for creating `.desktop` files according to the [freedesktop.org Desktop Entry Specification](https://specifications.freedesktop.org/desktop-entry-spec/latest/).

## Features

- **Wizard Interface**: Step-by-step GUI for creating desktop entries
- **Multiple Entry Types**: Support for Application, Link, and Directory types
- **Live Preview**: Edit the generated .desktop content before saving
- **Multiple Save Locations**: Save to desktop, local applications directory, or custom location
- **Automatic Permissions**: Sets executable permissions and marks files as trusted
- **Category Selection**: Choose from standard freedesktop.org categories
- **Icon Support**: Browse and select icon files (PNG, XPM, SVG, ICO)

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

1. Clone or download the source files
2. Run `make` to build the application
3. Run `make install` to install to `/usr/local/bin/` (optional)

```bash
make
make install  # Optional
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

1. **Choose Entry Type**: Select Application, Link, or Directory
2. **Basic Information**: Enter name and description
3. **Executable/Link/Directory**: Specify the target file, URL, or directory
4. **Icon (Optional)**: Browse and select an icon file
5. **Categories (Optional)**: Select one or more categories
6. **Preview and Edit**: Review and modify the generated content
7. **Distribution**: Choose save locations and create the file

### Save Locations

- **Desktop**: Saves to `~/Desktop/`
- **Local Applications**: Saves to `~/.local/share/applications/`
- **Custom Location**: User-specified directory

## File Structure

```
Cre8or/
├── main.c              # Application entry point
├── desktop_entry.h     # Desktop entry data structures
├── desktop_entry.c     # Desktop entry generation and validation
├── file_utils.h        # File operations header
├── file_utils.c        # File saving and permissions
├── wizard.h           # Wizard interface header
├── wizard.c           # Wizard GUI implementation
├── Makefile           # Build configuration
└── README.md          # This file
```

## Desktop Entry Specification Compliance

This tool follows the [freedesktop.org Desktop Entry Specification](https://specifications.freedesktop.org/desktop-entry-spec/latest/) and supports:

- **Application** entries with Exec, Terminal, and Categories fields
- **Link** entries with URL field
- **Directory** entries with Path field
- Standard categories as defined in the specification
- Proper file permissions and trust marking

## License

This project is licensed under the GPL-3.0 License.

## Contributing

Feel free to submit issues, feature requests, or pull requests to improve the application. 