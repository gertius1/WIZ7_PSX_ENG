This is just a quick extract of the Sega Saturn MSG Files using the same tools from the PSX translations (they worked, yay!).
Some explanation:
- all texts are huffman coded, PSX and SAT
- MISCJ.HDR contains the huffman table, MSGJ.HDR is the header file with index info, MSGJ.DBS holds all the texts.
  - without a number they are for Wizardry 7, with the 6 in the filename they are for Wizardry 6
- DBS can be decoded via the function void FUN_decodeHuffman(...)
- the result is Shift JIS encoded for Sega Saturn (it was some unknown coding for PSX. For PSX I had to use in-game display to show all the strings)
- working with Saturn texts should be even easier because of the straightworfard Shift JIS encoding
- Saturn MSGJ files seem to be even closer to the English DOS original than the PSX, but this is a first impression
- they don´t seem to contain any discernible function strings like the PSX version

Unfortunately for Saturn, the SCENARIJ file format is not the same. Seems to be split into multiple files:
- DB_ITEM6.BDA  (and 7)
- DB_MAZE6.BDA  (and 7)
- DB_MONS6.BDA  (and 7)
- DB_SPEC6.BDA  (and 7)
- DB_TREA6.BDA  (and 7)

The contents of the MAZE file were found completely in SCENARIJ.DBS
The structure of the ITEM (and probably MONS) file is the same as the respective section in the SCENARIJ.DBS but instead of the name there seems to be a pointer address.
It is unknown at the moment, where the pointerss point to, and where the Item/Monster/Spell names are stored.
