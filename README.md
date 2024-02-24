# AppImage Uninstaller

This project provides a C-based command-line tool designed to uninstall AppImage applications on Linux systems. It leverages `.desktop` files to locate and remove both the application's executable and its desktop file, streamlining the uninstallation process.
This helper file works very well together with automated AppImage install scripts, that can automatically generate desktop file uninstall entry.

## Usage

1. Copy `uninstall_appimage` to `$PATH` location, for example to `~/.local/bin`.
2. Add entry to application `.desktop` file:

```bash
Actions=Uninstall;

[Desktop Action Uninstall]
Name=Uninstall Application
Exec=uninstall_appimage <desktop_file_name_without_extension>
   ```

## Features

- **Simple Usage**: Just provide the name of the application as it appears in its `.desktop` filename (without the `.desktop` extension).
- **Desktop Notification**: Offers feedback through desktop notifications, indicating success or failure of the uninstallation.
- **Error Handling**: Includes basic error handling, such as failure to read the `.desktop` file or delete the AppImage and desktop file.

## Dependencies

- libnotify: For sending desktop notifications.

## Installation

Clone this repository and compile the project using GCC or any compatible C compiler. Ensure `libnotify` is installed on your system.

```bash
git clone https://github.com/kem-a/uninstall_appimage
cd uninstall_appimage
gcc -o uninstall_appimage main.c 'pkg-config --cflags --libs libnotify'
```
