@echo off

if "%1"=="" goto usage

mkdir %1-win64
cd %1-win64
cmake -G "Visual Studio 16 2019" ../../src/%1
cd ..
goto end

:usage
echo setup_win64.bat [homework_name]
echo Where homework_name is ga1-core, etc.

:end
