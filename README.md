# Cardboard-rewrite
A live copy of our game engine, the Cardboard Engine!

It's pretty neat.

## Supported Systems
Windows: ✓ | Mac: ❌ | Linux: ✓ (build required)

## How to compile

Ensure you have the development versions of `SDL2`, `SDL2_image`, and `SDL2_mixer`

```
cd src
make
make install (no sudo, it doesn't install to /usr!)
```
That's it! All you have to do now is tell the game your game token.

Edit `cardboard_unix` script to change `GAME_TOKEN` to your very own HNID Project Crimson game token (should be around line 7), and run it.
