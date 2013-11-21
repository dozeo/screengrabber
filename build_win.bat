@PATH=%PATH%;C:\Mingw\msys\1.0\bin

@rem -- git uses HOME to find .ssh/* stuff
@SET HOME=%USERPROFILE%
@SET CURDIR=%~dp0

@rem -- dependencies must be build without visual studio on path to avoid 
@rem -- some ./configure scripts picking up visual studio linker
@rem -- exception: ffmpeg needs it but the script will put it on path when it needs to
@bash %CURDIR%build_dependencies_win.sh

@call "%VS90COMNTOOLS%vsvars32.bat"
@bash %CURDIR%build_win.sh %*

pause