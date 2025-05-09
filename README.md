# Magnesium

## Author: Myra Roberts

A Skyrim Special Edition mod built with SKSE to enable real-time console
command injection, to enable friends or stream chat members to spawn in-game
items, entities, etc.

### How to Use

At the moment, there are no public binary releases of the mod, and so you must
build it yourself. The project is built with CMake, since it's what CommonLib
is made to be used with, and has integration with `vcpkg`. The project also has
several Papyrus scripts, and while these can be compiled using whatever compiler
you choose, I recommend [Pyro](https://github.com/fireundubh/pyro). All
dependencies for building/running the mod are listen in the
[Dependencies](#dependencies) section.

The goal is to eventually ship this as a proper FOMOD package, currently WIP.

### How it Works

Once initialized, the mod starts up a new worker thread running an HTTP server
from [cpp-httplib](https://github.com/yhirose/cpp-httplib/),
connected to the port specified in the [config file](/src/Config/magnesium.yaml)
(`8080` by default). This server then listens for incoming `POST` requests to
either the `/command` or `/display` endpoints, to specify sending one of the two
message types:

 1. `ConsoleCommand`: Run the specified in-game console command
 2. `DisplayMessage`: Display the message in the top-left corner in-game

The parameters for these come from the body of the `POST` request.

Once a message is processed, it is added to the message queue, which holds onto
all recieved messages until polled for them by the
[Papyrus scripts](src/Scripts/Source/MagnesiumInit.psc). This script is
initialized at the start by a custom quest in the [ESP](Magnesium.esp), and
every set time interval it polls the C++ backend using a linked native function
to get all messages that are currently in the queue and execute them. This
interval can also be controlled in the config file.

All messages are logged using `spdlog`, by default to
`Documents\My Games\Skyrim.INI\SKSE\Magnesium.log`, and the log level can be
changed in the config file.

### Dependencies

#### Runtime

These are necessary for the mod to run properly in-game. These will usually be
added to your mod manager of choice.

- SKSE64
- Address Library for SKSE Plugins
- ConsoleUtilSSE NG

#### Build

These are the dependencies needed to build the mod for yourself. Unfortunately,
I have only figured out how to build on Windows for the moment. SkyrimSE is
built primarily for Windows, and at the moment SKSE can only be linked with
MSVC.

- Windows 10 (Windows 11 might work but is currently untested)
- [vcpkg](https://github.com/microsoft/vcpkg)
- [CMake](https://cmake.org/)
- A Papyrus compiler (I recommend [Pyro](https://github.com/fireundubh/pyro))

The
[CommonLibSSE Sample Plugin](https://gitlab.com/colorglass/commonlibsse-sample-plugin)
has a good step-by-step guide for getting set up for SKSE development, I
highly recommend it if you are new to this.

CMake is used to build the C++ part of the project, and Pyro to compile the
Papyrus scripts. I've included a `.vscode` project setup that includes build
tasks for both, including copying the built mod to a Mod Organizer mod folder
for usage. I recommend reading through these before using them to set
necessary variables.

### Resources

This was originally built from the CommonLibSSE-NG Plugin template
[here](https://gitlab.com/colorglass/commonlibsse-sample-plugin), and with lots
of inspiration from [Skyrim Scripting](https://github.com/SkyrimScripting).
I also *highly* recommend watching through their
[videos on YouTube](https://youtube.com/SkyrimScripting) for info on how SKSE
modding is done.
