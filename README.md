# Cardboard Engine
A live copy of our game engine, the Cardboard Engine! Based on the fantastic Cube 2 game engine.

It's pretty neat.

## Supported Systems
Windows: ✓ | Mac: ❌ | Linux: ✓ (build required)

## Build instructions

### Windows
*Coming soon*

### Mac
*Coming soon*

### Linux
- Get dependencies (make and git are not included in the list, as they should be installed on your system already.)

**Ubuntu/Debian:**
```
sudo apt-get update
sudo apt-get install -y libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev libsdl2-ttf-dev libcurl4-openssl-dev
```

**Arch/Manjaro:**
```
sudo pacman -S sdl2 sdl2_mixer sdl2_image sdl2_ttf curl
```

- Get source code from latest tagged GitHub release
```
git clone https://github.com/HarpNetStudios/cardboard.git
cd cardboard
git checkout $(git describe --tags)
```

- Start the build process
```
cd src/enet
chmod +x check_cflags.sh
cd ..
make
make install
```

- Open `cardboard_unix` (in the root directory) with your favorite text editor, and replace `-cOFFLINE` with `-c{your game token}` Confused? [Check here!](https://harpnetstudios.com/my/account/gametokens)

- And lastly, run `chmod +x cardboard_unix` and `./cardboard_unix` to start it


