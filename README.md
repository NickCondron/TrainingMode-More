﻿<p align="center"><img src="Logos/Training-Mode-banner.png"  alt=""  width="300"/></p>

# Training Mode - More :: A Melee Modpack for Practicing Tech

Training Mode - More is an expanded and updated version of UnclePunch's training modpack for Super Smash Bros. Melee.

[comment]: <> (To download the ISO, click 'Releases' on the right side, then download "TM-More.zip".
Unzip, then drag your melee iso on the 'Drag Vanilla Melee Here.bat' file.
Feel free to file an issue to request a new feature!)

To download the ISO, click 'Releases' on the right side, then download "TM-More.zip".

To discuss changes and new features or ask for assistance, join [the discord](https://discord.gg/2Khb8CVP7A).

## Changes From the Original
- New Training Lab Features:
    - Updated to UCF 0.84 (Allows practicing with dashback out of crouch).
    - New neutral jump option for CPUs, set as the default.
    - Reworked recording UI. Allows resaveing existing recordings with different percents or positioning.
    - SDI and mashing are set to none by default.
    - Random custom DI option.
    - Hazard toggle in training lab.
    - Much more planned in the near future!
- Bugfixes:
    - Jump actions no longer make the CPU self-destruct.
    - Ledge and Act out of Jump OSDs are now back.
    - Fixed cpu acting too late out of sakurai angle and other non-knockdown hits (such as fox drill).
    - Lightshield now works in recordings.
    - Can now use lightshield L with DPad to adjust percents.
    - CPUs now DI DK cargo throw.
- Work in progress:
    - Fixing Export/Import replay crashes.
    - Fixing OSDs not showing.
- Developer Features:
    - Simple and easily reproducible builds on Windows and Linux.
    - Fast recompilation on Linux using make.
    - Simplified and performant [tool](https://github.com/AlexanderHarrison/gc_fst) to extract and rebuild ISOs.

## Compilation

We have completely overhauled the build process.
Now any developer can easily compile an iso from source and add new features to the modpack.
Have some specific tech you want to train? Find a bug that's been annoying you? Come make a PR!

### Windows
1. [Install DevKitPro](https://github.com/devkitPro/installer/releases/latest). Install the Gamecube (aka PPC or PowerPC) package.
2. Drag your legally obtained SSBM v1.02 ISO on to the 'build_windows.bat' file. If all goes well, 'TM-More.iso' will be created.

### Linux
1. [Install DevKitPro](https://devkitpro.org/wiki/Getting_Started#Unix-like_platforms). Install the Gamecube (gamecube-dev) package.
    - Ensure that `/opt/devkitpro/devkitPPC/bin/` is added to the PATH.
2. [Install Mono](https://www.mono-project.com/download/stable/#download-lin). Prefer installation through your package manager.
    - I would like nothing more than to remove mono, msbuild, .NET, C#, and Microsoft from my life. 
    Alas it is just as deeply ingrained the SSBM ecosystem as I am.
3. Install xdelta3. This should be simple to install through your package manager.
4. Run `make iso=path-to-melee.iso iso`. If all goes well, 'TM-More.iso' will be created.
    - If the provided 'gecko' binary fails (likely due to libc issues), you can compile your own binary from [here](https://github.com/JLaferri/gecko). YOU MUST USE VERSION 3.4.0.
    - If the provided 'gc_fst' binary fails (likely due to libc issues), you can compile your own binary from [here](https://github.com/AlexanderHarrison/gc_fst).

## How to make changes:  
- If you want to alter an event written in C (Easy):
    - The training lab, lcancel, ledgedash, and wavedash events are written in c. This makes them much easier to modify than the other events. Poke around in their source in `src/`.
    - The other events are written in assembly. 
- If you want to alter an event written in asm (Big Knowledge Check):
    - You will need to know a bit of Power PC asm.
    - Read `ASM/Readme.md`
    - Go to `ASM/training-mode/Custom Events/Custom Event Code - Rewrite.asm` and search for the event you want to modify.
    - These will trickier to modify than the other events. Prefer making a new event or modifying the lab.
    - There are a lot of random loads from random offsets there. If it's not listed in the ASM readme then I don't know what it is.
- If you want to make a new event (A little tricky):
    - Add a file and header to the `src/`.
    - Create a dat file in `dats/`.
        - This is a little annoying.
        - If you don't want any fancy features, just a pause menu, copy `lcancel.dat` as your dat file.
            - **You will need to use [HSDRaw](https://github.com/Ploaj/HSDLib) or a hex editor to modify the internal name.**
            - If you use a hex editor, ensure you set the altered length of the renamed dat file as the first 4 bytes in the file.
        - If you want other assets, you can use HSDRaw to add new objects to the dat file, or copy them from the other events.
    - Add the required compilation steps in `Makefile` and `build_windows.bat`. Follow the same structure as the other events. Be sure to use the evFunction mode.
    - Implement the `Event_Init`, `Event_Update`, `Event_Think` methods and `Event_Menu` pointer in your c file. Poke around the other events to figure out how the data flows.
- Other changes may be easy or difficult. Join [the discord](https://discord.gg/2Khb8CVP7A) if you want a nudge in the right direction.

Feel free to DM me on discord (alex_aitch) or twitter (@rwing_aitch) for help!