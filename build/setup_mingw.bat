@echo off

if "%1"=="" goto usage

mkdir %1-mingw
cd %1-mingw
cmake -G "MinGW Makefiles" -DVIDEO_OPENGLES=0 ../../src/%1
cd ..
goto end

:usage
echo setup_mingw.bat [homework_name]
echo Where homework_name is ga1-core, etc.

:end
