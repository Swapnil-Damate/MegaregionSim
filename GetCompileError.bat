@echo off
echo Running Compiler Diagnostics...
set UBT_PATH="C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat"
set PROJECT_PATH="%~dp0MegaregionSim.uproject"

call %UBT_PATH% MegaregionSimEditor Win64 Development -Project=%PROJECT_PATH% -WaitMutex -FromMsBuild -architecture=x64 > CompileErrorLog.txt 2>&1

echo Diagnostics complete. Please open CompileErrorLog.txt and paste the errors you see!
pause
