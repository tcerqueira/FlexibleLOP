#! /bin/sh
# default exporter
EXPORTER="gmake2" 

./build/premake5.exe --file=premake5.lua $EXPORTER
