#!/bin/bash
set -e  # exit on error

# Run doxygen
doxygen

# Move into latex directory and build pdf
cd dev/doxygen/latex
make pdf
cd ../../..

# Copy to doc/ with new name
cp dev/doxygen/latex/refman.pdf doc/PROGRAMMER-GUIDE.pdf
wslview doc/PROGRAMMER-GUIDE.pdf
