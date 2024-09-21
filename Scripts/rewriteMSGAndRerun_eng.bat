
REM set textIndex=15090

taskkill /f /im NO$PSX.EXE

cd "C:\Users\Zaq\source\repository\trunk\Romhacking\Wiz7\Wiz7_Patching_Utilities\Builds\VisualStudio2019\x64\Release\ConsoleApp"
C:
Wiz7_Patching_Utilities_rewriteMSGfromTXT.exe

xcopy /y "MSG_rewritten.HDR" "%~dp0Wizardry VII - Guardia no Houju (Japan)\dump\CDS\D\MSGJ.HDR"
xcopy /y "MSG_rewritten.DBS" "%~dp0Wizardry VII - Guardia no Houju (Japan)\dump\CDS\D\MSGJ.DBS"

cd %~dp0
I:
cd "..\TOOLS_PSX\mkpsxiso-2.03-win64\mkpsxiso-2.03-win64\bin\"
set directoryName=..\..\..\..\Wizardry VII - Guardia no Houju (Japan)\Wizardry VII - Guardia no Houju (Japan)\

mkpsxiso.exe -y -o "%directoryName%Wizardry VII - Guardia no Houju (Japan).bin" -c "%directoryName%Wizardry VII - Guardia no Houju (Japan).cue" "%directoryName%XML"

cd "..\..\..\no$psx"
Start NO$PSX.EXE "..\..\Wizardry VII - Guardia no Houju (Japan)\Wizardry VII - Guardia no Houju (Japan)\Wizardry VII - Guardia no Houju (Japan).cue"
REM WIZ7.au3 %textIndex%

REM taskkill /f /im NO$PSX.EXE