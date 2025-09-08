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

Then run `make` and `./mpx.sh`.


## CONTRIBUTING

After making changes, running `version` will show that your working directory is 'dirty'. This simply means that you have uncommitted changes.

Ensure you have checked out the correct branch and pulled its latest changes. Stage/add the relevant files before committing them.

Now you can run `make clean` and `make` again, run `./mpx.sh`, and finally run `version` to see your latest commit hash and showing that your working directory is 'clean'.

When you're done, add your contributions to [CONTRIBUTIONS.docx](doc/CONTRIBUTIONS.docx) and save it as a pdf.


## DOCUMENTATION

See the [doc/](doc/) folder for:
- [User's Manual]()
- [Programmer's Manual]()
- [CONTRIBUTIONS](doc/CONTRIBUTIONS.pdf)


## DOXYGEN

Create the configuration file (convention is a Doxyfile):
```bash
doxygen -g <config-file>
```

Edit the file to your liking, reference the [doxygen manual](https://www.doxygen.nl/manual/index.html) if needed, then run doxygen:
```bash
doxygen <config-file>
```

When releasing a new version of MacaroniOS, remember to change the `PROJECT_NUMER` (to R1, R2, etc) and `OUTPUT_DIRECTORY` (from `dev/doxygen` to `doc`). Also remember to change `user/version.c`.
