# AniTrack

Personal program for tracking watched anime on Linux and syncing with [AniList](https://anilist.co/).

Greatly inspired by [Taiga](https://github.com/erengy/taiga).

## Dependencies
- Qt6
- anitomy
- anime-relations
- utf8proc

## Building (Arch Linux)
- Install dependencies:
```bash
sudo pacman -Syu git cmake gcc qt6-base
```
- Clone and init submodules:
```bash
git clone --recurse-submodules https://github.com/nisemono0/AniTrack.git
```
- Configure and compile:
```bash
./compile.sh --release --clean
```
You can also run `./compile.sh --help` for more options.

The binary is in `build/bin/AniTrack`.

You can also run `AniTrack --help` to see some available options.

## Related projects
- [taiga](https://github.com/erengy/taiga): greatly inspired this program
- [anitomy](https://github.com/erengy/anitomy): anime filename parser
- [anime-relations](https://github.com/erengy/anime-relations): episode redirections
- [utf8proc](https://github.com/JuliaStrings/utf8proc): string normalization
- [tabler-icons](https://github.com/tabler/tabler-icons): application icons

