
set textIndex=28130

taskkill /f /im NO$PSX.EXE

cd "C:\Users\Zaq\source\repository\trunk\Romhacking\Wiz7\Wiz7_Patching_Utilities\Builds\VisualStudio2019\x64\Debug\ConsoleApp"
C:
Wiz7_Patching_Utilities.exe %textIndex%

xcopy /y "MSGJ.HDR" "%~dp0Wizardry VII - Guardia no Houju (Japan)_japMod\dump\CDS\D"

cd %~dp0
I:
cd "..\TOOLS_PSX\mkpsxiso-2.03-win64\mkpsxiso-2.03-win64\bin\"
set directoryName=..\..\..\..\Wizardry VII - Guardia no Houju (Japan)\Wizardry VII - Guardia no Houju (Japan)_japMod\

mkpsxiso.exe -y -o "%directoryName%Wizardry VII - Guardia no Houju (Japan).bin" -c "%directoryName%Wizardry VII - Guardia no Houju (Japan).cue" "%directoryName%XML"

cd "..\..\..\no$psx"
Start NO$PSX.EXE "..\..\Wizardry VII - Guardia no Houju (Japan)\Wizardry VII - Guardia no Houju (Japan)_japMod\Wizardry VII - Guardia no Houju (Japan).cue"
WIZ7.au3 %textIndex%

taskkill /f /im NO$PSX.EXE