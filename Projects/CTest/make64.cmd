@echo off

set Config=Debug64
set CFlags=-fno-builtin -g -O0
if "%1"=="Release" (
	set Config=Release64
	set CFlags=-DNDEBUG -O3 -ffunction-sections -fdata-sections -Wl,--gc-sections,-s
)

set Lib=-lmcf -lmcfcrt -lstdc++ -lmingwex -lmingw32 -lgcc -lgcc_eh -lmcfcrt -lgcc -lmingwex -ldlmalloc -lmsvcrt -lkernel32 -luser32 -lshell32 -ladvapi32

call mingw 64

mcfbuild -p../../MCFCRT/MCFBuild.mcfproj -s../../MCFCRT -d../../.Built/%Config%/mcfcrt -o../../.Built/%Config%/libmcfcrt.a %* || exit /b 1
mcfbuild -p../../MCF/MCFBuild.mcfproj -s../../MCF -d../../.Built/%Config%/mcf -o../../.Built/%Config%/libmcf.a %* || exit /b 1
mcfbuild -p../../External/dlmalloc/MCFBuild.mcfproj -s../../External/dlmalloc -d../../.Built/%Config%/dlmalloc -o../../.Built/%Config%/libdlmalloc.a %* || exit /b 1
mcfbuild -p../../External/lzma/MCFBuild.mcfproj -s../../External/lzma -d../../.Built/%Config%/lzma -o../../.Built/%Config%/liblzma.a %* || exit /b 1
mcfbuild -p../../External/zlib/MCFBuild.mcfproj -s../../External/zlib -d../../.Built/%Config%/zlib -o../../.Built/%Config%/libz.a %* || exit /b 1

gcc %CFlags% -std=c11 -Wall -Wextra -Wsign-conversion -Wsuggest-attribute=noreturn -pipe -mfpmath=both -march=core2 -masm=intel main.c -o ".%Config%.exe" -I../.. -L../../.Built/%Config% -static -nostdlib -Wl,-e__MCF_ExeStartup,--disable-runtime-pseudo-reloc,--disable-auto-import,--wrap=atexit,--wrap=malloc,--wrap=realloc,--wrap=calloc,--wrap=free,--wrap=__cxa_throw %Lib% || exit /b 1