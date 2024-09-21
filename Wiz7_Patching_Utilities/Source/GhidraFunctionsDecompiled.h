/*
  ==============================================================================

    GhidraFunctions.h
    Created: 1 Aug 2024 4:22:50am
    Author:  Zaq

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <string>

using namespace juce;
using namespace std;

//Huffman tables
extern char DAT_MISC[0x1E4];
extern char DAT_MISCJ[0x400];

void FUN_decodeHuffman(char* inHuffString, char* outASCII, bool JAP);

// the Huffman Encoder functions are not from Ghidra, but put them here anyway to match decoder location
int FUN_encodeHuffman(String inASCIIString, uint8_t* outHuff);
void packBitfields(uint32_t* bitfields, uint8_t* lengths, size_t count, uint8_t* output, size_t* output_size);
int examplePackBitfields();

void FUN_ASCIItoZENKAKU(char* input, char* output, int flag);