
set textIndex=102

taskkill /f /im NO$PSX.EXE

cd "C:\Users\Zaq\source\repository\trunk\Romhacking\Wiz7\Wiz7_Patching_Utilities\Builds\VisualStudio2019\x64\Debug\ConsoleApp"
C:
Wiz7_Patching_Utilities_ExchangeSubIdx_JAP.exe %textIndex%

xcopy /y "MSGJ_rewritten.HDR" "%~dp0Wizardry VII - Guardia no Houju (Japan)_japMod\dump\CDS\D\MSGJ.HDR"

cd %~dp0
I:
cd "..\TOOLS_PSX\mkpsxiso-2.03-win64\mkpsxiso-2.03-win64\bin\"
set directoryName=..\..\..\..\Wizardry VII - Guardia no Houju (Japan)\Wizardry VII - Guardia no Houju (Japan)_japMod\

mkpsxiso.exe -y -o "%directoryName%Wizardry VII - Guardia no Houju (Japan).bin" -c "%directoryName%Wizardry VII - Guardia no Houju (Japan).cue" "%directoryName%XML"

cd "..\..\..\no$psx"
Start NO$PSX.EXE "..\..\Wizardry VII - Guardia no Houju (Japan)\Wizardry VII - Guardia no Houju (Japan)_japMod\Wizardry VII - Guardia no Houju (Japan).cue"
WIZ7_single.au3 %textIndex%

REM taskkill /f /im NO$PSX.EXE