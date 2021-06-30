@echo off

set application_name=app.exe
set build_options= -DBUILD_DEVELOPER=1 -DBUILD_DEBUG=1 -DBUILD_WIN32=1
set compile_flags= -nologo -FC /W0 /Zi /EHsc  -I../src
set link_flags= -incremental:no -opt:ref gdi32.lib opengl32.lib user32.lib dsound.lib dxguid.lib winmm.lib

REM The fully code is compatible with C++, to compile just remove /Tc from the cl.exe command

if not exist build mkdir build
pushd build
"../tcc/tcc" -Wall -Wl,-subsystem=windows -m64  ../src/win32_main.c -o app.exe
popd