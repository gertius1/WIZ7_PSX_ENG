/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <JuceHeader.h>
#include <string>
#include <iostream>
#include "GhidraFunctionsDecompiled.h"
#include "Functions.h"
#include <bitset>
#include  <iomanip>

using namespace std;

using namespace juce;

//input needs to be huffman string without first byte (total input length)
char testHuffman[8] = { 0x05,0xEF ,0xFD ,0xDF ,0x7F,0x8E ,0xF0 }; //HUMAN
char testHuffmanJap[9] = {0x0A ,0xD6 ,0xB5 ,0xAD ,0x6B ,0x5A ,0xD6 ,0x80 };
char testHuffmanJap2[9] = { 0x08,0xBA,0x2B,0xE6,0xCC,0x2E,0x78 };
char testHuffmanJap3[0x90] = { 0xB8,0x86,0x20,0x52,0x7D,0xAB,0xE8,0x8C,0x08,0x63,0xF7,0x4C,0x1B,0xF1,0x0F,0xB0,0xB3,0x95,0x3E,0xA7,0x05,0xF3,0x66,0xB7,0x3A,0xC5,0x5F,0x8B,0xF2,0xF0,0x67,0xB5,0xE5,0x1C,0x9A,0xAA,0x7D,0x07,0xDD,0x4F,0xB6,0xC8,0x5D,0xE7,0xAE,0xAB,0xFE,0x2E,0x17,0xC8,0x2A,0xC7,0xB0,0x5C,0xB4,0xE9,0xAC,0x85,0x7D,0x39,0x48,0x40,0xB6,0xEF,0xC3,0xEF,0x04,0xBD,0xAE,0x5A,0x17,0x66,0x39,0x35,0xB9,0x84,0x09,0x19,0x97,0xCC,0x57,0x5F,0x3A,0x7E,0x74,0x7D,0x88,0x5F,0xCC,0xE4,0x8A,0x9F,0x44,0x61,0x3E,0x3E,0x44,0x3F,0xEE,0xFC,0x00,0x8C,0xA7,0xB5,0x4E,0xA9,0xF5,0x35,0x5E,0x4E,0x47,0xEB,0xCA,0x67,0xA7,0x4D,0x0F,0x92,0x40,0x3E,0x31,0x22,0xF7,0x34,0xEF,0xF8,0x78,0x78,0xB8,0x48,0x80};
char testHuffmanJap4[0x35] = { 0x48,0x0C,0x7B,0x16,0x23,0x8C,0xA7,0xC8,0xF0,0x0B,0x6E,0xA8,0x0B,0xAC,0x5C,0xDD,0xD0,0xD2,0xB7,0x6D,0x91,0xB1,0x9D,0x98,0xEC,0xEE,0x39,0xD1,0x95,0xF1,0x5A,0x2E,0xB9,0xAA,0xF2,0x80,0xD7,0x10,0xB4,0x9E,0x27,0x71,0x55,0xF3,0x86,0xF5,0x5B,0x17,0x37,0x74,0x2C,0x17,0x20 };


//==============================================================================
int main(int argc, char* argv[])
{
    std::cout << "\n\nWiz7 Patching Utilities\n\n";
    std::cout << "Created with JUCE 7.0.8.\n\n\n";

    int retVal = 0;
   
    //int textIndex = 14300;
    //int textIndex = atoi(argv[1]);
    int windowIndex = 15105;
    bool JAP = false;

    //select function here

    //retVal = swapTextByIndex(textIndex, windowIndex, JAP);
    //retVal = extractTextByIndex(textIndex, JAP, false);    
    //parseHDRfile(JAP);
    //writeDBSFromCSV(JAP);
    //markMainIndices(JAP);
    //writeTalkScr();
    compareFunctionCodes();

    return retVal;
}
