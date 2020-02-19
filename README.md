# i8080 & Space Invaders emulator
![alt text](https://github.com/SpamBot/emu/blob/master/spaceInvaders.gif)

# i8080
Mostly done, except DAA and ACops. May have bugs.

# Space Invaders
Known issues:
- LEFT/RIGHT controls are not responding
- moving objects leave weird trails (those can be seen on gif)  

Technically game is "playable"

# Files and dependencies
- **/i8080/comparrison_debugger.c** debuger for running alongside
2 emulators (mainly emu101, look at resources); depends: **debugger.h, 8080emu.c**(not in this repo)
- **/i8080/cpu.h** i8080 emulator; depends: **cpu_clock_cycles.h** (but that can be avoided)
- **/i8080/cpu_clock_cycles.h** contains opcode clock cycles
- **/i8080/debugger.h** depends: **cpu.h, disassembler.h**
- **/i8080/disassembler.h** i8080 disassembler
- **/spaceInvaders/Sdl_ver.c** Space invaders machine simulator,
uses SDL lib for graphics and input handling; depends: **cpu.h, SDL.h**(not in this repo)
- **/spaceInvaders/console_ver.c** Use that one if you don't want to install any
fancy libs, just set console font size to min, suits for testing purposes; depends: **cpu.h**
- **/spaceInvaders/sdl_moving_square.ex** Not a part of the emulator,
just a simple example of SDL lib use for input'n'graphics handling; depends: **SDL.h**

# Resources and credits:
- [emu101](http://emulator101.com): main idea, cpu structure, opcodes;
- [kpmiller](http://github.com/kpmiller/emulator101): graphics and interrupts handling, debugging;
- [JavaScript i8080&SpaceInvaders emulator](http://bluishcoder.co.nz/js8080/): clock cycles, interrupts handling, debugging;
