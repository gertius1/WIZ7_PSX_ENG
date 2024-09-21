setlocal ENABLEDELAYEDEXPANSION
REM cd %~dp0
REM set no$psx Emulation Speed to Unlimited MHz Disaster 10%
set workingDir=%~dp0
set directoryName=..\..\..\..\Wizardry VII - Guardia no Houju (Japan)\Wizardry VII - Guardia no Houju (Japan)\

REM for /F "tokens=*" %%A in  (JAP_indices test.txt) do  (
FOR /F %%i IN (ENG_indices.txt) DO (ECHO %%i

set /a textIndex=%%i

taskkill /f /im NO$PSX.EXE

cd "C:\Users\Zaq\source\repository\trunk\Romhacking\Wiz7\Wiz7_Patching_Utilities\Builds\VisualStudio2019\x64\Debug\ConsoleApp"
C:
Wiz7_Patching_Utilities_ShowTextByIdx_ENG.exe !textIndex!

xcopy /y "MSG.HDR" "%~dp0Wizardry VII - Guardia no Houju (Japan)\dump\CDS\D\MSGJ.HDR"

I:
cd "I:\Projects\Romhacking\TOOLS_PSX\mkpsxiso-2.03-win64\mkpsxiso-2.03-win64\bin"


mkpsxiso.exe -y -o "%directoryName%Wizardry VII - Guardia no Houju (Japan).bin" -c "%directoryName%Wizardry VII - Guardia no Houju (Japan).cue" "%directoryName%XML"

cd "..\..\..\no$psx"
Start NO$PSX.EXE "..\..\Wizardry VII - Guardia no Houju (Japan)\Wizardry VII - Guardia no Houju (Japan)\Wizardry VII - Guardia no Houju (Japan).cue"

WIZ7.au3 !textIndex!

taskkill /f /im NO$PSX.EXE
)