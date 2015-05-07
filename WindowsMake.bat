rem ######################## EditHere #########################
    SET FreeGlutDir=.\freeglut-win32
    SET PthreadDir=.\pthreads-win32
    SET Platform=x86
    SET Output=.\main_release.exe
rem ###########################################################
    SET FreeGlutInclude=%FreeGlutDir%\include
    SET FreeGlutLib=%FreeGlutDir%\lib
    SET FreeGlutBin=%FreeGlutDir%\bin
    SET PthreadInclude=%PthreadDir%\include
    SET PthreadLib=%PthreadDir%\lib\%Platform%
    SET PthreadBin=%PthreadDir%\dll\%Platform%
rem ###################### G++ Command ########################
    g++ *.cpp -lopengl32 -lfreeglut -lglu32 -lwsock32 -lpthreadGC2 -I %FreeGlutInclude% -I %PthreadInclude% -L %FreeGlutLib% -L %PthreadLib% -o %Output% -std=c++11
rem ######################## CopyDll ##########################
    COPY %FreeGlutBin%\freeglut.dll freeglut.dll
    COPY %PthreadBin%\pthreadGC2.dll pthreadGC2.dll
rem ###########################################################
