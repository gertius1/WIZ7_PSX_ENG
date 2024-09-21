REM ======= xdelta =================
i:
cd "I:\Projects\Romhacking\TOOLS_PSX\xdeltaUI"

set pathMod=I:\Projects\Romhacking\Wizardry VII - Guardia no Houju (Japan)\Wizardry VII - Guardia no Houju (Japan)
set pathOrig=I:\Projects\Romhacking\Wizardry VII - Guardia no Houju (Japan)\Wizardry VII - Guardia no Houju (Japan)_orig

REM make xdelta patch: xdelta3.exe -e -s orig_file mod_file delta_file
xdelta.exe -f -e -s "%pathOrig%\Wizardry VII - Guardia no Houju (Japan)_orig.bin" "%pathMod%\Wizardry VII - Guardia no Houju (Japan).bin" "%pathMod%\Wiz7_patch.xdelta"

REM apply patch: xdelta3.exe -d -s orig_file delta_file decoded_mod_file
xdelta.exe -f -d -s "%pathOrig%\Wizardry VII - Guardia no Houju (Japan)_orig.bin"  "%pathMod%\Wiz7_patch.xdelta" "%pathMod%\Wizardry VII - Guardia no Houju (Japan)_xDelta.bin"