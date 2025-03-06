#!/bin/sh
#

mkdir bin 2>/dev/null

gcc -std=c++11 -w -m32 -march=i686 -mtune=generic -msse -msse2 -O3 -s -shared -static-libgcc -static-libstdc++\
	-fno-stack-protector -fno-strict-aliasing -fno-strict-overflow -fvisibility=hidden -fPIC \
	-Isrc \
	-Idep/rehlsdk/common -Idep/rehlsdk/dlls -Idep/rehlsdk/engine -Idep/rehlsdk/public \
	-Idep/metamod \
    -Wl,--no-export-dynamic \
	src/*.cpp \
	-o bin/rehlmaster.so

# /opt/intel/bin/icc -mia32 -O3 -s -static-intel -shared -static-libgcc -no-intel-extensions \
# 	-Isrc \
# 	-Idep/rehlsdk/common -Idep/rehlsdk/dlls -Idep/rehlsdk/engine -Idep/rehlsdk/public \
# 	-Idep/metamod \
# 	src/*.cpp \
# 	-o bin/rehlmaster.so

