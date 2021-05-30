# MES
MES project files
- Tiago Ramos
- Tiago Cerqueira

# Building MES
This was meant to run on Linux. The libpthread and libdl are required. It is not tested in a Cygwin-like environment.
Follow steps below on a shell.

## Steps:
- ./generateMake.sh
- make config=release -j

## Run:
The executable can be found in "build/bin/Release-${system}/MES" where system is for e.g. windows or linux.