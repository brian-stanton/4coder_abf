CALL D:\ProgramFiles\VisualStudio\VC\Auxiliary\Build\vcvarsall.bat x64

set ProjectDir=C:\main\development-tools\4coder\4coder
set CustomDir=%ProjectDir%\custom


pushd %ProjectDir%
echo
echo Building 4coder for Win32 64 bit...
echo
CALL %CustomDir%\bin\buildsuper_x64-win.bat %CustomDir%\4coder_abf.cpp
popd
