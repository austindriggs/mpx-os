# Macaroni Penguins

CS450: Operating Systems Structure


## GETTING STARTED

Install WSL if you need to:
```bash
wsl --install -d ubuntu
```

Clone this repo into a linux environment (WSL, Ubuntu, etc):
```bash
git clone https://github.com/WVU-CS450/MacaroniPenguins.git
```

Prep your linux environment by running the following commands or `./setup.sh`:
```bash
sudo apt update
sudo apt install -y clang make nasm git binutils-i686-linux-gnu qemu-system-x86 gdb doxygen
```

Then to test your environment, run `make` and then `./mpx.sh`.


## DOCUMENTATION

See the [doc/](doc/) folder for:
- [User's Manual]()
- [Programmer's Manual]()
- [Contributions]()


## DOXYGEN

Install doxygen in your linux environment:
```bash
sudo apt install -y doxygen
```

Create the configuration file (convention is a Doxyfile):
```bash
doxygen -g <config-file>
```

Edit the file to your liking, reference the [doxygen manual](https://www.doxygen.nl/manual/index.html) if needed, then run doxygen:
```bash
doxygen <config-file>
```
