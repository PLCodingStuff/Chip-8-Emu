# Chip-8 Emu
Chip-8 language interpreter written in C, based on [Cowgod's technical Reference v1.0](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM). 

### Current Status
The current version runs in command line and is able to render images. The movement operation codes are not yet functional. Specifically, the opcodes `SKP`, `SKNP`, `LDVK` and `LDFV`. Sound is also not supported. The project is developed in `C23` with `GCC 13.4.0`.

# Installation & Setup
1. Clone the project from GitHub:
   ```bash
   git clone https://github.com/PLCodingStuff/Chip-8-Emu
   cd Chip-8-Emu
   ```
2. Build the project:
   ```bash
   make
   ```
3. Run the project:
   ```bash
   ./chip8 <filename.rom>
   ```

# Example Usage
This is a demo of the IBM's Logo rendered in chip8, in command line.

```bash
./chip8 "IBM Logo.ch8"
```
![IBM Logo Chip8](https://github.com/PLCodingStuff/Chip-8-Emu/blob/main/imgs/IBM%20Logo%20Demo.png)
