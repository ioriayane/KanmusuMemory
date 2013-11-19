
if '%1'=='x86' goto X86
if '%1'=='x64' goto X64


echo "deploywin.bat x86|x64"

goto QUIT




REM ------------------------------------------
:X86
echo x86
call "C:\Program Files (x86)\Microsoft Visual Studio 11.0\vc\vcvarsall.bat" x86
perl scripts\deploy.pl win32

goto QUIT



REM ------------------------------------------
:X64
echo x64
call "C:\Program Files (x86)\Microsoft Visual Studio 11.0\vc\vcvarsall.bat" x86_amd64
perl scripts\deploy.pl win64

goto QUIT




REM ------------------------------------------

:QUIT
