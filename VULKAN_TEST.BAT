@echo off
title Program Build! |color 06
echo """"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
echo "                                                                                     "
echo "               ______                                                                "
echo "             _/      /                                                               "
echo "     _______|        /________                                                       "
echo "   _/       |        /__     |                                                       "
echo "   \        |          /     |                                                       "
echo "    \       |___     /       |                                                       "
echo "     \         /    /        |                                                       "
echo "      \       /    /_        |                                                       "
echo "       \     |__/   _        |                                                       "
echo "        \______/  //_________|                                                       "
echo "              / /                                                                    "
echo "             //                                                                      "
echo "                                                                                     "
echo "                                                                                     "
echo """""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
echo.
echo.

REM Config

REM setlocal enabledelayedexpansion  REM delayed variable expansion / Problem Disable ! char

REM SET DIR_LEFA=lefa
SET DIR_GAME=Vulkan_test/
SET DIR_OUTPUT=VK_TEST
REM program object , and build files.
SET EXIT=../%DIR_OUTPUT%/obj
SET buildout=../
SET exename=VK

for /r "%DIR_OUTPUT%/obj" %%f in (*.obj) do del "%%f"




REM Create DIR
REM _________________________________________
if not exist "%DIR_OUTPUT%" (
    mkdir "%DIR_OUTPUT%"
    echo Direcorty '%DIR_OUTPUT%'created with sucess.
) else (
    echo.
)
REM _________________________________________
REM _________________________________________
if not exist "%DIR_OUTPUT%/obj" (
    mkdir "%DIR_OUTPUT%/obj"
    echo Direcorty '%DIR_OUTPUT%/obj'created with sucess.
) else (
    echo.
)
REM _________________________________________


REM Time /t > build_out.txt
REM Date /t >> build_out.txt
REM echo last_compilation >> build_out.txt


REM Define Compiler. 
REM WATCOM 1.9
REM ______________________________
REM I386+
set cc=wcc386
set cpp=wpp386

REM SILD WATCOM 0.0 test
REM AMD64 / EM64t
REM set cc=wcc64  REM tests
REM set cpp=wpp64 REM tests

REM ______________________________




REM IGNORE THIS !    2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See" | it is to hide watcom commercial messages


REM -6r             Pentium Pro register calling conventions
REM.
REM /6r (32-bit only) generate 386 instructions based on Intel Pentium Pro instruction timings and
REM   use register-based argument passing conventions 


REM -6s             Pentium Pro stack calling conventions
REM.
REM /6s (32-bit only) generate 386 instructions based on Intel Pentium Pro instruction timings and
REM   use stack-based argument passing conventions


REM -fp6 Optimized Option For Binary Precision / generate in-line 80x87 instructions optimized for Pentium Pro processor /optimize floating-point

REM -ol  -o and optimization / l -> enable loop optimizations


cd SRC

echo """""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
echo " Game Files                                                                                           "
echo """"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""/
echo.
echo.
echo  ___________MAIN.C_________________________________________
echo /____________________________________________________________\
      @%cc% /6r -fp6 -ol -oo -ecd "vulkan_test/main.c" -i="vulkan_test/include" -Fo="%EXIT%/main"  ^
      2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See" 
echo ______________________________________________________________
echo \____________________________________________________________/ 
echo.

echo.
echo.
echo  ___________GAME.C_________________________________________
echo /____________________________________________________________\
      @%cc% /6r -fp6 -ol -oo "vulkan_test/game.c" -Fo="%EXIT%/game"  ^
      2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See" 
echo ______________________________________________________________
echo \____________________________________________________________/ 
echo.
echo.
echo.
echo.
echo.
echo """""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
echo " Engine Files                                                                                                        "
echo """"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""/
echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo ####################################################################################################################
echo # Final Program                                                                                                    #
echo ####################################################################################################################
echo.
echo.
echo  __________(RESOURCE)________________________________________
echo /____________________________________________________________\
     @wrc vulkan_test/resource/pres.rc -r -fo="%EXIT%/pres.res"   ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo ______________________________________________________________
echo \____________________________________________________________/
echo.
echo.

cd ../
cd %DIR_OUTPUT%/obj
echo.
echo.
echo  __________(Linker)________________________________________
echo /__________________________________________________________\
     @wlink option resource 'pres.res' name %exename%.exe file *.obj Library vulkan-1.lib, glfw3dll.lib ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo ____________________________________________________________
echo \__________________________________________________________/

REM move the bin file system
REM _________________________________________
if exist *.exe (
    move *.exe "%buildout%" | echo DONE !
) else (
    echo BUILD FAIL !
)
REM _________________________________________


echo.
echo _



REM del *.obj 
REM del *.res 

pause