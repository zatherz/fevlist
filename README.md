fevlist
===

Small tool to list event paths inside an FMOD `.bank` file.

## Usage

```
help: fevlist -h|--help
usage: fevlist [-v|--verbose -d|--fmod-debug] PATH_TO_MASTER_STRINGS_BANK PATH_TO_TARGET_BANK

       the tool requires you to provide the 'Master Bank.strings.bank' file on top
       of the one you're actually interested in, because that's what contains information
       about event names
```

## Building

### Windows

Building on Windows coming soon.

### Linux

Building requires `make` and a C++ compiler like `g++`.

* Go to [the FMOD Download page](https://www.fmod.com/download) and sign in.
* Click on "FMOD Studio Suite", scroll down to "FMOD Engine", click on "Older" and make sure "1.10.20 (Unsupported)" is selected in the list.
* Download the Linux archive.
* Unpack this archive somewhere.
* Go into the `fevlist` root directory with the `Makefile` and run `make CPU=... FMOD_STUDIO_PATH=... `, like so:
  ```
  make CPU=x86_64 FMOD_STUDIO_PATH=/home/zatherz/Misc/fmodstudioapi11020linux/
  ```
  `CPU` should be one of `[x86|x86_64|arm|armhf]`, and `FMOD_STUDIO_PATH` should be the path to the unpacked files you just downloaded.
* Done!

### Mac

idk

## Why?

For grabbing event paths from Noita for mods.
