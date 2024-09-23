This is just a quick extract of the Sega Saturn MSG Files using the same tools from the PSX translations (they worked, yay!).
Some explanation:
- all texts are huffman coded, PSX and SAT
- MISCJ.HDR contains the huffman table, MSGJ.HDR is the header file with index info, MSGJ.DBS holds all the texts. 
- DBS can be decoded via the function void FUN_decodeHuffman(...)
- the result is Shift JIS encoded for Sega Saturn and some unknown coding for PSX. For PSX I used in-game display to show all the strings.
- working with Saturn texts should be even easier because of the straightworfard Shift JIS encoding

Unfortunately for Saturn, the SCENARIJ file format is not the same. Seems to be split into multiple files:
- DB_ITEM6.BDA  (and 7)
- DB_MAZE6.BDA  (and 7)
- DB_MONS6.BDA  (and 7)
- DB_SPEC6.BDA  (and 7)
- DB_TREA6.BDA  (and 7)

The contents of the MAZE file were found completely in SCENARIJ.DBS
The structure of the ITEM (and probably MONS) file is the same as the respective section in the SCENARIJ.DBS but instead of the name there seems to be a pointer address
