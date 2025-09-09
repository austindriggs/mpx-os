#!/bin/bash

# prep
sudo apt update
sudo apt install -y clang make nasm git binutils-i686-linux-gnu qemu-system-x86 gdb doxygen

# next steps
echo
echo "Run 'make' and then run './mpx.sh'."

