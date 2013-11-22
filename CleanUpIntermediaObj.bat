@echo off 
echo Cleaning intermediate files...... 

echo [ Delete ] all sdf files
del /f /s /q *.sdf 

rem Some debug info is stored in suo files 
rem echo [ Delete ] all suo files
rem del /f /s /q *.suo 
rem del /f /s /q *.v11.suo

rem echo [ Delete ] all user files
rem del /f /s /q *.user

echo [ Delete ] all exe files
del /f /s /q *.exe

echo [ Delete ] all bin files
del /f /s /q *.bin

echo [ Delete ] all pfx files
del /f /s /q *.pfx

echo [ Delete ] all bin folder
@for /r  %%i in (bin) do @if exist "%%i" @rd/s/q "%%i"

echo [ Delete ] all ARM folder
@for /r  %%i in (ARM) do @if exist "%%i" @rd/s/q "%%i"

echo [ Delete ] all Win32 folder
@for /r  %%i in (Win32) do @if exist "%%i" @rd/s/q "%%i"

echo [ Delete ] all Debug folder
@for /r  %%i in (Debug) do @if exist "%%i" @rd/s/q "%%i"

echo [ Delete ] all Release folder
@for /r  %%i in (Release) do @if exist "%%i" @rd/s/q "%%i"

echo [ Delete ] all Generated Files folder
@for /r  %%i in (Generated Files) do @if exist "%%i" @rd/s/q "%%i"

echo Now ,you can use Git Add£¡ 
echo. & pause 