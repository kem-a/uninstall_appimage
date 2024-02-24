<img src="https://github.com/kem-a/uninstall_appimage/blob/main/resources/example.png" alt="Logo" align="right" />

# AppImage Uninstaller

This project provides a C-based command-line tool designed to uninstall AppImage applications on Linux systems. It leverages `.desktop` files to locate and remove both the application's executable and its desktop file, streamlining the uninstallation process by adding 'Uninstall Application' menu to right click.
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
- **Add Uninstall to all**: When run with option `--update` add *Uninstall Application* menu to all AppImage desktop files in user home directory.

## Dependencies

- libnotify: For sending desktop notifications.

## Build & Installation from Source

Clone this repository and compile the project using GCC or any compatible C compiler. Ensure `libnotify` is installed on your system.

1. Clone repository
```bash
git clone https://github.com/kem-a/uninstall_appimage && cd uninstall_appimage
```
2. Build and install
```bash
make && make install
```
3. Uninstall
```bash
make uninstall
```
