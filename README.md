# benbot


- [benbot](#benbot)
    - [About](#about)
    - [Build requirements](#build-requirements)
    - [Build instructions](#build-instructions)

## About
Starcraft 2 rule-based bot for Terran race.

benbot about things this is:
* thing1
* thing2
* thing3

## Support
Starcraft II AI Discord server. [Invite Link](https://discordapp.com/invite/Emm5Ztz)

## Build requirements
0. Download Starcraft II (https://starcraft2.com/en-us/)

1. Download (the password is iagreetotheeula) at least one of the following map packs:
  * [Ladder 2017 Season 1](http://blzdistsc2-a.akamaihd.net/MapPacks/Ladder2017Season1.zip)
  * [Ladder 2017 Season 2](http://blzdistsc2-a.akamaihd.net/MapPacks/Ladder2017Season2.zip)
  * [Ladder 2017 Season 3](http://blzdistsc2-a.akamaihd.net/MapPacks/Ladder2017Season3_Updated.zip)
  * [Ladder 2017 Season 4](http://blzdistsc2-a.akamaihd.net/MapPacks/Ladder2017Season4.zip)
  * [Ladder 2018 Season 1](http://blzdistsc2-a.akamaihd.net/MapPacks/Ladder2018Season1.zip)
  * [Ladder 2018 Season 2](http://blzdistsc2-a.akamaihd.net/MapPacks/Ladder2018Season2_Updated.zip)

2. Put the downloaded maps into the Maps folder (create it if the folder doesn't exist):
  * Windows: C:\Program Files\StarCraft II\Maps

3. Download and install [CMake](https://cmake.org/download/).

4. A compiler with C++14 support.

5. Windows: Download and install [Visual Studio 2017](https://www.visualstudio.com/downloads/)

## Build instructions

### Windows (Visual Studio)
```bat
:: Get the project.
$ git clone --recursive https://github.com/bacahillsmu/benbot.git
$ cd benbot

:: Create build directory.
$ mkdir build
$ cd build

:: Generate VS solution.
$ cmake ../ -G "Visual Studio 15 2017 Win64"

:: Build the project using Visual Studio.
$ start ben.sln

:: Launch the bot with the specified path to a SC2 map, e.g.
$ bin\Debug\ben.exe Ladder2017Season3\InterloperLE.SC2Map
```
