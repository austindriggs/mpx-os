# Macaroni Penguins

CS450: Operating Systems Structure


## GETTING STARTED (R0)

### SUMMARY

Goals:
- Prepare your system for MPX development.
- Become familiar with building, running, and modifying MPX (and sharing changes through GitHub).
- Initialize low-level functionality of MPX.

### NOTES

Clone this repo into a linux environment (WSL, Ubuntu, etc):
```bash
git clone https://github.com/WVU-CS450/MacaroniPenguins.git
```

Prep your linux environment by running the following commands or `./setup.sh`:
```bash
sudo apt update
sudo apt install -y clang make nasm git binutils-i686-linux-gnu qemu-system-x86 gdb
```

Then to test your environment, run `make` and then `./mpx.sh`.


## BASIC INTERFACE DEVELOPMENT (R0)

### SUMMARY

Interface setup and implementation: this module will focus on developing a simple command line interface, developing a series of commands that the user can execute, and underlying C “string” functions needed by your interface to facilitate communications with your user.

Goals:
- Design a user interface.
- Become familiar with MPX-specific functions.
- Directly interface with hardware components.

### NOTES



