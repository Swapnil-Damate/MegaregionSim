@echo off
echo Starting Automated CI/CD Testing Pipeline...

set ENGINE_PATH="C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
set PROJECT_PATH="%~dp0MegaregionSim.uproject"

echo Running tests for MegaregionSim...
%ENGINE_PATH% %PROJECT_PATH% -ExecCmds="Automation RunTests Megaregion" -unattended -nopause -testexit="Automation Test Queue Empty" -log=TestResults.log

echo Testing Complete! Check Saved/Logs/TestResults.log for details.
pause
