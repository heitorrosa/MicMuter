# MicMuter

A lightweight Windows system tray application that allows you to quickly mute/unmute your microphone using a customizable hotkey.

## Running the Application

1. Install [Visual C++ Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170)
2. Download and run `MicMuter.exe`

## Building from Source

**Requirements:**
- [MSYS2](https://www.msys2.org/) with MinGW GCC
- CMake 3.10+

**Build Steps:**
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

The executable will be in `build/MicMuter.exe`

## Configuration

Settings are stored in `config.ini`:
```ini
hotkey=RCtrl
beep=1
```

## License

GPL 3.0 License. See LICENSE for details.