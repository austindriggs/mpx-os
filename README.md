# Macaroni Penguins

A freestanding C environment, dubbed "Macaroni OS", developed in a group project for Operating System Structures (CS450) at WVU in Fall 2025.

See the repo at https://github.com/WVU-CS450/MacaroniPenguins.


## GETTING STARTED

Install WSL if you need to (some features may only work inside of an Ubuntu WSL distro):
```bash
wsl --install -d ubuntu
```

Clone this repo into a linux environment (WSL, Ubuntu, etc):
```bash
git clone https://github.com/WVU-CS450/MacaroniPenguins.git
```

Prep your linux environment by running the following commands:
```bash
sudo apt update
sudo apt install -y clang make nasm git binutils-i686-linux-gnu qemu-system-x86 gdb
```

Then run `make` and `./mpx.sh`.

For more information, either run the `help` command (or `help verbose`) inside of MacaroniOS, or consult the [doc/USER-GUIDE.pdf](doc/USER-GUIDE.pdf).


## CONTRIBUTING

After making changes to the code, running `version` will show that your working directory is 'dirty'. This simply means that you have uncommitted changes. Ensure you have checked out the correct branch and pulled its latest changes. Stage/add the relevant files before committing them.

Now you can run `make clean` and `make` again, run `./mpx.sh`, and finally run `version` to see your latest commit hash and showing that your working directory is 'clean'.

When you're done, add your contributions to [dev/CONTRIBUTIONS.docx](dev/CONTRIBUTIONS.docx) and save it as [doc/CONTRIBUTIONS.pdf](/doc/CONTRIBUTIONS.pdf).


## DOXYGEN

Install doxygen and dependancies:
```bash
sudo apt update
sudo apt install -y doxygen texlive-full texlive-latex-base texlive-latex-extra wslu
```

> If you get some errors, maybe try `sudo apt upgrade -y`.
> If you are getting an "WSL Interoperability is disabled" error, see [WSL issue 13449](https://github.com/microsoft/WSL/issues/13449) and run the permenant fix codeblock.

Create the configuration file (convention is a Doxyfile) with `doxygen -g Doxyfile`. Edit the file to your liking, reference the [doxygen manual](https://www.doxygen.nl/manual/index.html) if needed, then run this script:
```bash
./doxyfile.sh
```

> When releasing a new version of MacaroniOS, remember to change the `PROJECT_NUMER` (to R1, R2, etc) (this should also get changed in `user/version.c`).

This script runs doxygen to generates a bunch of latex files in the dev folder, makes those files into a pdf, and then renames and moves it to become the [doc/PROGRAMMERS-GUIDE.pdf](doc/PROGRAMMERS-GUIDE.pdf).


## RELEASES

View all of the [Releases](https://github.com/WVU-CS450/MacaroniPenguins/releases/) or view previous documentation in [doc/.legacy/](doc/.legacy/).
