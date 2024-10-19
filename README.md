# Magnesium
## Author: Myra Roberts

A Skyrim Special Edition mod built with SKSE to enable real-time console 
command injection, to enable friends or stream chat members to spawn in-game 
items, entities, etc.

### How to Use

At the moment, there are no public binary releases of the mod, and so you must
build it yourself. The project is built with CMake, since it's what CommonlibNG
is made to be used with, and has integration with `vcpkg`. The project also has
several Papyrus scripts, and while these can be compiled using whatever compiler
you choose, I recommend [Pyro](https://github.com/fireundubh/pyro). All 
dependencies for building/running the mod are listen in the 
[Dependencies](#Dependencies) section.

### How it Works

Once initialized, the mod starts up a new worker thread running an HTTP server,
connected to the port specified at the top of [plugin.cpp](src/plugin.cpp)
(`8000` by default). This server then listens for incoming `POST` requests,
with a body that follows the following format:

```
MessageType;Data
```

`MessageType` is the type of message to interpret, of which the currently
supported types are:

 1. `ConsoleCommand`: Run the specified in-game console command
 2. `DisplayMessage`: Display the message in the top-left corner in-game

The parameters for these come from the `Data` section.

If the message is well-formatted and interpreted succesfully, a success message
is sent back and the connection is closed.

Once a message is processed, it is added to the message queue, which holds onto
all recieved messages until polled for them by the 
[Papyrus scripts](src/Scripts/Source/MagnesiumInit.psc). This script is 
initialized at the start by a custom quest in the [ESP](Magnesium.esp), and 
every Papyrus Script Tick polls the C++ backend using a linked native function
to get all messages that are currently in the queue and execute them.

All messages are logged using `spdlog`, by default to 
`Documents\My Games\Skyrim.INI\SKSE\Magnesium.log` (again, Windows path, as
this is designed only to run on Windows (for now)).

### Dependencies
#### Runtime

These are necessary for the mod to run properly in-game. These will usually be
added to your mod manager of choice.

 -  SKSE64
 -  Address Library for SKSE Plugins
 -  ConsoleUtilSSE NG

#### Build

These are the dependencies needed to build the mod for yourself. Unfortunately,
I have only figured out how to build on Windows for the moment. SkyrimSE is 
built primarily for Windows, and Magnesium itself relies on the Windows 
`afxwin` library for its multithreading.

 -  Windows :(
 -  [vcpkg](https://github.com/microsoft/vcpkg)
 -  [CMake](https://cmake.org/)
 -  [mongoose](https://github.com/cesanta/mongoose) (included in project)
 -  A Papyrus compiler (I recommend [Pyro](https://github.com/fireundubh/pyro))

CMake is used to build the C++ part of the project, and Pyro to compile the 
Papyrus scripts. I've included a `.vscode` project setup that includes build 
tasks for both, including copying the built mod to a Mod Organizer mod folder
for usage. I recommend reading through these before using them to set 
necessary variables.

### Resources

This was originally built from the fantastic templates by 
[Skyrim Scripting](https://github.com/SkyrimScripting). I also *highly*
recommend watching through their 
[videos on YouTube](https://youtube.com/SkyrimScripting) for info on how SKSE
modding is done.
