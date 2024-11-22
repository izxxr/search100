# search100
A simple yet fast search engine for text files.

## Installation
There are two modes of installing and setting up of Search100:

1. Downloading precompiled files from releases
2. Manual installation and compilation of dependencies

> ℹ️ The installation steps below are only applicable for Windows.

### Installing GCC Compiler
For either mode, the prerequisite is to install the GCC compiler and have it included
in the `PATH` variable.

For proper functioning of SFML library, it is recommended to use the version of GCC linked below:

> **64-bit:** [**⬇️ Download MSVCRT 13.1.0 x64**](https://github.com/brechtsanders/winlibs_mingw/releases/download/13.1.0-16.0.5-11.0.0-msvcrt-r5/winlibs-x86_64-posix-seh-gcc-13.1.0-mingw-w64msvcrt-11.0.0-r5.7z)

> **32-bit:** [**⬇️ Download MSVCRT 13.1.0 x86**](https://github.com/brechtsanders/winlibs_mingw/releases/download/13.1.0-16.0.5-11.0.0-msvcrt-r5/winlibs-i686-posix-dwarf-gcc-13.1.0-mingw-w64msvcrt-11.0.0-r5.7z)

Extract the downloaded ZIP file and add the path of extracted `mingw64/bin/` directory to `PATH`
environment variable.

<!-- TODO: Add GIF/video for adding to path --->

### Using Precompiled Files

Use the following link to download the files for your respective operating system:

> **64-bit:** [**⬇️ Download search100_w64.zip**](https://github.com/izxxr/search100/releases/download/1.0.0/search100_w64.zip)

> **32-bit:** [**⬇️ Download search100_w32.zip**](https://github.com/izxxr/search100/releases/download/1.0.0/search100_w32.zip)

Extract the downloaded ZIP file and run `search100.exe` file to start using Search100.

### Manual Installation
In most cases, the first method should be enough to get Search100 running however it is
recommended to use manual installation mode for advanced use cases (e.g. development) or
if precompiled files do not work for any reason.

Download the SFML library for your respective operating system from the following link:

> **64-bit:** [**⬇️ Download SFML 2.6.2 - GCC MinGW (SEH) 13.1.0**](https://www.sfml-dev.org/files/SFML-2.6.2-windows-gcc-13.1.0-mingw-64-bit.zip)

> **32-bit:** [**⬇️ Download SFML 2.6.2 - GCC MinGW (DW2) 13.1.0**](https://www.sfml-dev.org/files/SFML-2.6.2-windows-gcc-13.1.0-mingw-32-bit.zip)

Clone this repository either by downloading the files directly from GitHub or using Git:

```bash
$ git clone https://github.com/izxxr/search100
```

Extract the downloaded `SFML-2.6.2-windows-gcc-13.1.0-mingw-xx-bit.zip` file and from the
extracted directory:

1. Copy `SFML-2.6.2/lib` directory to cloned `search100` directory.
2. Copy content of `SFML-2.6.2/include/SFML` to `search100/include` directory.
3. Copy DLL files from `SFML-2.6.2/bin` to `search100` directory.

With this done, open terminal in cloned `search100` directory and compile Search100:

```bash
$ mingw32-make -f Makefile
```

Run the produced `search100.exe` file to use Search100.
