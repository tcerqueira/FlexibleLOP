#! /bin/sh

# default exporter
EXPORTER="gmake2"
while getopts e: option
do
case "${option}"
in
e) EXPORTER=${OPTARG};;
esac
done

./build/premake5.exe --file=premake5.lua $EXPORTER
