# Cardboard Engine / Carmine Impact

[![](https://img.shields.io/static/v1?label=Steam&message=wishlist&logo=steam&labelColor=000000&color=ff2400)](https://store.steampowered.com/app/1359640/Carmine_Impact/) [![](https://img.shields.io/static/v1?label=itch.io&message=download&logo=itchdotio&labelColor=fa5c5c&color=ff2400&logoColor=FFFFFF)](https://harpnetstudios.itch.io/carmine-impact)

A live copy of our game engine, the Cardboard Engine!
Based on the fantastic Cube 2 game engine.

It currently also houses the source for Carmine Impact,
as it's deeply rooted within the engine itself.

## Supported Platforms
### Desktop
- Windows 10+: ✅
- OSX 10.9+:
	- x86_64: ⚠️
	- ARM64: ❌
- Linux: ✅

## Build instructions
### Windows
- Build with Visual Studio 2019, `msbuild` should also work.

### Mac (WIP)

***This currently only works on Intel Macs, until SDL2_image and SDL2_mixer are updated for Apple Silicon.***
- Ensure you have at least Xcode Command Line Tools installed, if not Xcode itself.
- Build with Xcode, or use `src/macbuild.sh`

### Linux
- Get dependencies (make and git are not included in the list, as they should be installed on your system already.)

**Ubuntu/Debian:**
```
sudo apt-get update
sudo apt-get install -y libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev libsdl2-ttf-dev libcurl4-openssl-dev libenet-dev
```

**Arch/Manjaro:**
```
sudo pacman -S sdl2 sdl2_mixer sdl2_image sdl2_ttf curl enet
```

- Get source code from latest tagged GitHub release
```
git clone --recursive https://github.com/HarpNetStudios/cardboard.git
cd cardboard
git checkout $(git describe --tags)
```

- Start the build process
```
cmake CMakeLists.txt
make
make install
```

- Open `cardboard_unix` (in the root directory) with your favorite text editor, and replace `-cOFFLINE` with `-c{your game token}`
Confused? [Check here!](https://harpnetstudios.com/my/account/gametokens)

- And lastly, run `chmod +x cardboard_unix` and `./cardboard_unix` to start it

## Contributing
Github Issues is the best way to report bugs,
as well as reporting them in the [official Discord server](https://discord.gg/zsUak4D).

## License
The Cardboard engine is licensed under the zlib license.
Please refer to the [`license`](https://github.com/HarpNetStudios/cardboard/tree/master/license) folder for more info.
