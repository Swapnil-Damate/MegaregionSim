@echo off
echo Generating Visual Studio project files...

set UBT_PATH="C:\Program Files\Epic Games\UE_5.3\Engine\Build\BatchFiles\Build.bat"
set PROJECT_PATH="%~dp0MegaregionSim.uproject"

call %UBT_PATH% -projectfiles -project=%PROJECT_PATH% -game -rocket -progress

echo.
echo Done! You should now see MegaregionSim.sln in this folder.
pause
