/*
  ==============================================================================

    Functions.h
    Created: 5 Aug 2024 1:33:12pm
    Author:  Zaq

  ==============================================================================
*/

#pragma once

int findMainIndexFromAnyIndex(FileInputStream* inStreamHDR, int anyIndex, bool JAP);

// takes any index, not only main indices
int findMainIndexAndPositionFromAnyIndex(FileInputStream* inStreamHDR, int inAnyIndex, int* outMainIndex, int* outPosition, bool JAP);
int findMainIndexAndPositionFromAnyIndexOpt(FileInputStream* inStreamHDR, int inAnyIndex, int* outMainIndex, int* outPosition, bool JAP);


int calculateDBSAddressFromHDRTextIdxposition(FileInputStream* inStreamHDR, int position);
int findHDRPositionByTextIndex(FileInputStream* inStream, int textIndex);
int getDBSStringByHDRposition(FileInputStream* inStreamHDR, FileInputStream* inStreamDBS, int position, bool JAP, bool zenkaku);
int getDBSStringByHDRtextIndex(FileInputStream* inStreamHDR, FileInputStream* inStreamDBS, int textIndex, int subIndex, bool JAP, bool zenkaku, int* DBSsize);

// takes any index, not only main indices
int getDBSAddressByHDRanyIndex(FileInputStream* inStreamHDR, FileInputStream* inStreamDBS, int anyIndex, bool JAP);


//manipulates HDR file to move string address from one index to another for displaying text ingame at the same gameworld postion
int swapTextByIndex(int sourceIdx, int destIdx, bool JAP);

//takes any index, not just main indices and writes it´s text content address to the destIdx
int swapTextBySubIndex(int sourceSubIdx, int destIdx, bool JAP);

//finds textstring in DBS file, decodes Huffman to ASCII and optionally converts the ASCII to ZENKAKU
int extractTextByIndex(int sourceIdx, bool JAP, bool alsoZenkaku);

int parseHDRfile(bool JAP);

int createHuffmanTable(bool JAP);

int writeDBSFromCSV(bool JAP);

int markMainIndices(bool JAP);

// parses the pointer table in TALK.SCR and writes the strings into a txt
int parseTalkScr();

// parses TALK.TXT and writes a new TALK.SCR, while updating the pointer table
int writeTalkScr();

// parses MSGHDRJ_indexText.csv for function codes and checks if they esxist in MSGHDR_indexText.txt
int compareFunctionCodes();