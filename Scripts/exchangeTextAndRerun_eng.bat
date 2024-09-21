
set textIndex=15090

taskkill /f /im NO$PSX.EXE

xcopy /y "%~dp0Wizardry VII - Crusaders of the Dark Savant(1992)\wizar7 - mod\MSG.HDR" "C:\Users\Zaq\source\repository\trunk\Romhacking\Wiz7\Wiz7_Patching_Utilities\Builds\VisualStudio2019\x64\Debug\ConsoleApp\MSG.HDR"
xcopy /y "%~dp0Wizardry VII - Crusaders of the Dark Savant(1992)\wizar7 - mod\MSG.DBS" "C:\Users\Zaq\source\repository\trunk\Romhacking\Wiz7\Wiz7_Patching_Utilities\Builds\VisualStudio2019\x64\Debug\ConsoleApp\MSG.DBS"

cd "C:\Users\Zaq\source\repository\trunk\Romhacking\Wiz7\Wiz7_Patching_Utilities\Builds\VisualStudio2019\x64\Debug\ConsoleApp"
C:
Wiz7_Patching_Utilities_ShowTextByIdx_ENG.exe %textIndex%

xcopy /y "MSG.HDR" "%~dp0Wizardry VII - Guardia no Houju (Japan)\dump\CDS\D\MSGJ.HDR"
xcopy /y "MSG.DBS" "%~dp0Wizardry VII - Guardia no Houju (Japan)\dump\CDS\D\MSGJ.DBS"

cd %~dp0
I:
cd "..\TOOLS_PSX\mkpsxiso-2.03-win64\mkpsxiso-2.03-win64\bin\"
set directoryName=..\..\..\..\Wizardry VII - Guardia no Houju (Japan)\Wizardry VII - Guardia no Houju (Japan)\

mkpsxiso.exe -y -o "%directoryName%Wizardry VII - Guardia no Houju (Japan).bin" -c "%directoryName%Wizardry VII - Guardia no Houju (Japan).cue" "%directoryName%XML"

cd "..\..\..\no$psx"
Start NO$PSX.EXE "..\..\Wizardry VII - Guardia no Houju (Japan)\Wizardry VII - Guardia no Houju (Japan)\Wizardry VII - Guardia no Houju (Japan).cue"
REM WIZ7.au3 %textIndex%

REM taskkill /f /im NO$PSX.EXE