# Simple Game Engine

## Presentation

*For educational purposes*

A project I made a while ago. It's a 2D game engine that uses [SDL](https://www.libsdl.orghttps:/) as a base.
As it's name implies, it's a very simple game engine designed for quick prototype building.

The engine's design is heavily inspired from [Unity](https://unity.com/).
Everything is an `Object` that is updated at every frame. The game creator can add `Components` to enrich objects and create his own `ScriptComponents` where their write the logical behaviour of each object.

[SMB1 1st level recreated in the Simple Game Engine](assets/20230616_120623_image.png)

### Some of the engine's features include:

- 📦 **Dynamic object-component system**: Safely instanciate or destroy objects at any time while playing. Attach or detach components and scripts freely from any object.
- 📝 **Scripts**: Write scripts that can be attached to objects with easy-access to various sub-systems like `Input`, `Camera`, `Object`, `Audio`, etc.
- 🎧 **Audio**: Play music and sounds easily from anywhere. You can choose to use `playSound()` or `playSoundFast()` to load the file in memory or not.
- 👾 **Texture**: Draw your object on screen with `TextureComponent`. The engine handles texture caching so you can have multiple objects with the same texture.
- 🚶‍♂️ **Animated Texture**: Animate objects with the `AnimatedTextureComponent`. It requires an `*.sganim` file that describes where animations start and end on a spritesheet. Once initialized, animating an object is just a matter of calling `playAnimation()`.
- 💿 **Object deserialization**: Construct objects from a `*.sgo` file. Within an SGO file you can specify default data for an object as well as which components it holds when instanciated and their default data. You can also specify children for the object.
- 🌍 **World deserialization**: Similarly to object serialization, you can create `*.sgworld` files which contain data to instanciate and destroy multiple objects quickly.
- 🗺 **Generate level from a [Tiled](https://www.mapeditor.org) JSON file**: Generate a level from JSON data and a tileset contained in an image file.
  You can also create an optional `*.sgmap` file which associates SGO file to a tile index in the JSON. This allows you to place game objects like player or enemy spawns, checkpoints, pick-ups, etc. directly in *Tiled*

## Dependencies

This engine uses [SDL2](https://www.libsdl.org), [SDL_image](https://github.com/libsdl-org/SDL_image), [SDL_ttf](https://github.com/libsdl-org/SDL_ttf) and [SDL_mixer](https://github.com/libsdl-org/SDL_mixer)

## How to build

First, build the SimpleGameEngine as a static library.

Your game code will be in the `Game` project which should be built as a dynamic library. Build `Run` as an executable and make sure it links to your game's code, the engine and SDL.

You should write inside `game.cpp`'s `run()` the code to instanciate your `Window` and an loop running indefinitely that calls `Window::processEvents()` and `Window::draw()`

`Window::processEvents()` updates the `Input` sub-system so you know which key is pressed or not in your scripts by just calling `Input::getKeyUp()` or any other function

`Window::draw()` automatically gathers all objects that should be drawn and draw them based on the camera's position.

Your game assets have to be placed in a folder called `resources` and any script you create and want to use in serialized objects must be added to `ScriptParsing.cpp -> associationMap` as a new pair following this syntax:

`{"scriptName", SCRIPT_LAMBDA_SIGNATURE{ target->addScript<ScriptName>(); }}`

## Missing documentation

This project was made long ago and lacks documentation.

## Conclusion

There is a lot to improve and fix on this engine but it fulfills it's main purpose. You can create simple 2D prototypes without having to worry about anything else but your game.

I would not recommend this engine to anyone who wants to make a polished, content-rich and optimized game. There are lots of very powerful free engines with an active community and a documentation, namely [Unity](https://unity.com), [Unreal Engine](https://www.unrealengine.com/en-US/unreal-engine-5), [Godot](https://godotengine.org)
