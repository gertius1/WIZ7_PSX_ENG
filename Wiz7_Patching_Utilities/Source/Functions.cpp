/*
  ==============================================================================

    Functions.cpp
    Created: 5 Aug 2024 1:33:12pm
    Author:  Gertius

  ==============================================================================
*/

#include <JuceHeader.h>
#include <string>
#include <iostream>
#include "GhidraFunctionsDecompiled.h"

#include "Functions.h"
#include <bitset>
#include  <iomanip>
#include <string.h>

using namespace std;
using namespace juce;

int swapTextByIndex(int sourceIdx, int destIdx, bool JAP)
{
    String hdr;
    String hdr_orig;

    if (JAP)
    {
        hdr = "MSGJ.HDR";
        hdr_orig = "MSGJ_orig.HDR";
    }
    else
    {
        hdr = "MSG.HDR";
        hdr_orig = "MSG_orig.HDR";
    }

    //create fresh MSGJ.HDR
    File dest = File::getCurrentWorkingDirectory().getChildFile(hdr);
    if (!dest.existsAsFile())
    {
        std::cout << "ERROR: No .HDR File detected, exiting" << "\n";
        return 1;  // file doesn't exist
    }
    File src = File::getCurrentWorkingDirectory().getChildFile(hdr_orig);
    if (!src.existsAsFile())
    {
        std::cout << "ERROR: No _orig.HDR File detected, exiting" << "\n";
        return 1;  // file doesn't exist
    }

    //src.copyFileTo(dest);

    // open MSGJ.HDR
    // CREATE OUTFILE
    File Dir = File::getCurrentWorkingDirectory();
    File HDR = File::getCurrentWorkingDirectory()
        .getChildFile(hdr);
    if (!HDR.existsAsFile())
    {
        std::cout << "ERROR: No .HDR File detected, exiting" << "\n";
        return 1;  // file doesn't exist
    }
    FileInputStream inStreamHDR(HDR); // [2]
    if (!inStreamHDR.openedOk())
        return 1;  // failed to open
    FileOutputStream outStreamHDR(HDR); // [2]
    if (!outStreamHDR.openedOk())
        return 1;  // failed to open


    // determine indices 
    //int textIndex = 100;
    //int textIndex = atoi(argv[1]);
    int textIndex = sourceIdx;

    //std::cout << "textIndex: " << textIndex << "\n";
    //int textIndex = 0x0A;
    int position = 2; //skip first 2 bytes
    int retVal = 1;

    // determine position of textIndex in MSGJ.HDR
    for (int i = 0; i < 2000; i++)
    {
        inStreamHDR.setPosition(position);
        if (inStreamHDR.readShort() == textIndex)
        {
            retVal = 0;
            break;
        }
        position += 6; //index entries are 6 byte long with 2*index, 2*mem position, 1*subindex and 1*block nr
    }

    if (retVal == 1)
        return retVal;

    retVal = 1;

    //copy payLoad (memory and subindex info) from textIndex to windowIndex
    inStreamHDR.setPosition(position + 2);
    int payLoad = inStreamHDR.readInt();

    int addressBlock400 = (payLoad >> 24) & 0xff;
    int nrOfSubindices = (payLoad >> 16) & 0xff;
    int addressMSB = (payLoad >> 8) & 0xff;
    int addressLSB = payLoad & 0xff;

    // window index of debug text window in MSGJ.HDR
    // 15020 = 0x3AAC ("Walking through the trees you suddently stop...."), Position = 0x506
    //int windowIndex = 15105;
    int windowIndex = destIdx;
    position = 2;
    // determine position of windowIndex in MSGJ.HDR
    for (int i = 0; i < 2000; i++)
    {
        inStreamHDR.setPosition(position);
        if (inStreamHDR.readShort() == windowIndex)
        {
            retVal = 0;
            break;
        }
        position += 6; //index entries are 6 byte long with 2*index, 2*mem position, 1*subindex and 1*block nr
    }

    outStreamHDR.setPosition(position + 2);
    //outStreamHDR.writeInt(payLoad);

    outStreamHDR.writeByte(addressLSB);
    outStreamHDR.writeByte(addressMSB);
    outStreamHDR.writeByte(nrOfSubindices);
    outStreamHDR.writeByte(addressBlock400);


    std::cout << "windowIndex: " << windowIndex << "\n";
    std::cout << "retVal: " << retVal << "\n";

    return retVal;
}

int swapTextBySubIndex(int sourceSubIdx, int destIdx, bool JAP)
{
    //takes any index as sourceSubIdx, not just main indices and writes it´s text content address to the destIdx
    //destIdx has to be a main index still(also makes sense because in a real game window only a main index is being displayed)

    String hdr;
    String dbs;

    if (JAP)
    {
        hdr = "MSGJ_rewritten.HDR";
        dbs = "MSGJ_rewritten.DBS";
    }
    else
    {
        hdr = "MSG_rewritten.HDR";
        dbs = "MSG_rewritten.DBS";
    }

    //this parses a hdr file and writes a csv file with the contents
    File HDR = File::getCurrentWorkingDirectory()
        .getChildFile(hdr);
    if (!HDR.existsAsFile())
        return 1;  // file doesn't exist
    FileInputStream inStreamHDR(HDR); // [2]
    if (!inStreamHDR.openedOk())
        return 1;  // failed to open
    FileOutputStream outStreamHDR(HDR); // [2]
    if (!outStreamHDR.openedOk())
        return 1;  // failed to open

    File DBS = File::getCurrentWorkingDirectory()
        .getChildFile(dbs);
    if (!DBS.existsAsFile())
        return 1;  // file doesn't exist
    FileInputStream inStreamDBS(DBS); // [2]
    if (!inStreamDBS.openedOk())
        return 1;  // failed to open

    int address = getDBSAddressByHDRanyIndex(&inStreamHDR, &inStreamDBS, sourceSubIdx, JAP);

    int windowIndex = destIdx;
    int position = 2;

    // determine position of windowIndex in MSGJ.HDR
    for (int i = 0; i < 2000; i++)
    {
        inStreamHDR.setPosition(position);
        if (inStreamHDR.readShort() == windowIndex)
        {
            //write new address, taken from txt above
            outStreamHDR.setPosition(position+2);
            outStreamHDR.writeShort(address % 0x400);
            outStreamHDR.writeByte(0); //will be overwritten next loop, here as placeholder to advance write pos
            outStreamHDR.writeByte(address / 0x400);
            break;
        }
        position += 6; //index entries are 6 byte long with 2*index, 2*mem position, 1*subindex and 1*block nr
    }


    return 0;
}

int findMainIndexFromAnyIndex(FileInputStream* inStreamHDR, int anyIndex, bool JAP)
{
    int mainIndex = 0;
    int position = 2;
    short index = 0;
    bool found = false;

    do
    {
        do
        {
            inStreamHDR->setPosition(position);
            index = inStreamHDR->readShort();

            if (index == anyIndex)
            {
                mainIndex = index;
                found = true;
                break;
            }

            position += 6;


        } while (!inStreamHDR->isExhausted());
        //while (position < 0x3000);//for JAP version
    
        if (found)
            break;
        else
        {
            anyIndex--;
            position = 2;
        }
    } while (anyIndex);



    return mainIndex;
}

int findMainIndexAndPositionFromAnyIndex(FileInputStream* inStreamHDR, int inAnyIndex, int* outMainIndex, int* outPosition, bool JAP)
{
    int position = 2;
    short index = 0;
    bool found = false;

    do
    {
        do
        {
            inStreamHDR->setPosition(position);
            index = inStreamHDR->readShort();

            if (index == inAnyIndex)
            {
                *outMainIndex = index;
                *outPosition = position;
                found = true;
                break;
            }

            position += 6;


        } while (!inStreamHDR->isExhausted());
        //while (position < 0x3000);//for JAP version

        if (found)
            break;
        else
        {
            inAnyIndex--;
            position = 2;
        }

    } while (inAnyIndex);

    return 0;
}


int findMainIndexAndPositionFromAnyIndexOpt(FileInputStream* inStreamHDR, int inAnyIndex, int* outMainIndex, int* outPosition, bool JAP)
{
    int position = 2, prevPosition = 2;
    short index = 0, prevIndex = 0;
    bool found = false;

    do
    {
        inStreamHDR->setPosition(position);
        index = inStreamHDR->readShort();


        if (index <= inAnyIndex)
        {
            prevIndex = index;
            prevPosition = position;
            position += 6;
            continue;
        }
        else
        {
            *outMainIndex = prevIndex;
            *outPosition = prevPosition;
            found = true;
            break;
        }

        


    } while (!inStreamHDR->isExhausted());
    //while (position < 0x3000);//for JAP version

    if (found)
        ;
    else
    {
        inAnyIndex--;
        position = 2;
    }

    return 0;
}


int calculateDBSAddressFromHDRTextIdxposition(FileInputStream* inStreamHDR, int position)
{
    //copy payLoad (memory and subindex info) from textIndex to windowIndex
    //position is the start of an entry (indexNr), so we need to add+2 bytes to advance to the payload
    inStreamHDR->setPosition(position+2);
    int payLoad = inStreamHDR->readInt();

    //determine address in MSGJ.DBS from payload

    int addressBlock400 = (payLoad >> 24) & 0xff;
    int addressMSB = (payLoad >> 8) & 0xff;
    int addressLSB = payLoad & 0xff;

    int address = (int)(addressBlock400 * 0x400);
    address += (int)(addressMSB * 0x100);
    address += (int)addressLSB;

    return address;
}

int findHDRPositionByTextIndex(FileInputStream* inStream, int textIndex)
{
    // determine indices 

    //std::cout << "textIndex: " << textIndex << "\n";

    int position = 2; //skip first 2 bytes
    int retVal = 1;

    // determine position of textIndex in MSGJ.HDR
    for (int i = 0; i < 2000; i++)
    {
        inStream->setPosition(position);
        if (inStream->readShort() == textIndex)
        {
            retVal = 0;
            break;
        }
        position += 6; //index entries are 6 byte long with 2*index, 2*mem position, 1*subindex and 1*block nr
    }

    if (retVal == 1)
        return 0;
    else
        return position;
}

const int bufSize = 0x1000;

char inputBufHuff[bufSize];
char testOutputBufferHuff[bufSize]; //just collecting all the inputBufHuff bytes
char testOutputBufferASCII[bufSize];
char testOutputBufferZENKAKU[bufSize];

int getDBSStringByHDRposition(FileInputStream* inStreamHDR, FileInputStream* inStreamDBS, int position, bool JAP, bool zenkaku)
{
    int address = calculateDBSAddressFromHDRTextIdxposition(inStreamHDR, position);
    int nextAddress = calculateDBSAddressFromHDRTextIdxposition(inStreamHDR, position + 6);
    int stringSizeMainIndex = nextAddress - address;
    int indexOutputBufferASCII = 0;
    int indexOutputBufferZenkaku = 0;

    int j = address;
    int i = 0;

    for (j; j < nextAddress; j)
    {
        zeromem(inputBufHuff, bufSize);
        // first byte is nrOfInBytes (how many huff bytes will follow)
        inStreamDBS->setPosition(j);
        int nrOfInBytes = inStreamDBS->readByte() & 0xff;
        testOutputBufferHuff[j - address] = nrOfInBytes;
        j++;

        for (i = 0; i < nrOfInBytes; i++)
        {
            inStreamDBS->setPosition(j);
            inputBufHuff[i] = inStreamDBS->readByte();
            testOutputBufferHuff[j-address] = inputBufHuff[i];
            j++;
        }

        int nrOfOutBytes = inputBufHuff[0] & 0xff;

        inputBufHuff[nrOfInBytes] = 0;
        FUN_decodeHuffman(inputBufHuff, testOutputBufferASCII + indexOutputBufferASCII, JAP);
        indexOutputBufferASCII += nrOfOutBytes;
    }

    if (JAP && zenkaku)
        FUN_ASCIItoZENKAKU(testOutputBufferASCII, testOutputBufferZENKAKU, 0);

    return 0;
}

int getDBSStringByHDRtextIndex(FileInputStream* inStreamHDR, FileInputStream* inStreamDBS, int textIndex, int subIndex, bool JAP, bool zenkaku, int* DBSsize)
{
    /*convert and get single substring denoted by textIndex and subIndex*/

    int position = findHDRPositionByTextIndex(inStreamHDR, textIndex);
    int address = calculateDBSAddressFromHDRTextIdxposition(inStreamHDR, position);

    inStreamHDR->setPosition(position + 4);
    int nrOfSubIndices = inStreamHDR->readByte();

    if (subIndex > nrOfSubIndices)
        return 1;

    int i = 0;
    int jumpVal;
    int j = address;
    int nrOfInBytes;

    inStreamDBS->setPosition(j);
    nrOfInBytes = inStreamDBS->readByte() & 0xff;

    //jump to correct subindex by evaluating the lengths and moving forward accordingly
    for (i = 0; i < subIndex; i++)
    {
        j += nrOfInBytes+1;
        inStreamDBS->setPosition(j);
        nrOfInBytes = inStreamDBS->readByte() & 0xff;
    }
    j++;

    zeromem(inputBufHuff, bufSize);
    zeromem(testOutputBufferASCII, bufSize);
    zeromem(testOutputBufferHuff, bufSize);


    for (i = 0; i < nrOfInBytes; i++)
    {
        inStreamDBS->setPosition(j);
        inputBufHuff[i] = inStreamDBS->readByte();
        j++;
    }

    int nrOfOutBytes = inputBufHuff[0] & 0xff;

    inputBufHuff[nrOfInBytes] = 0;
    FUN_decodeHuffman(inputBufHuff, testOutputBufferASCII, JAP);

    *DBSsize = nrOfInBytes;

    /* DEBUG */
    //FUN_encodeHuffman(testOutputBufferASCII, (uint8_t*)testOutputBufferHuff);
    //if (strcmp(inputBufHuff, testOutputBufferHuff+1) != 0)
    //    int testDebug = 1;
    /* DEBUG */

    if (JAP && zenkaku)
        FUN_ASCIItoZENKAKU(testOutputBufferASCII, testOutputBufferZENKAKU, 0);

    return 0;
}

int getDBSAddressByHDRanyIndex(FileInputStream* inStreamHDR, FileInputStream* inStreamDBS, int anyIndex, bool JAP)
{
    
    int mainIndex;
    int mainIndexPosition;

    findMainIndexAndPositionFromAnyIndexOpt(inStreamHDR, anyIndex, &mainIndex, &mainIndexPosition, JAP);

    int mainIdxAddress = calculateDBSAddressFromHDRTextIdxposition(inStreamHDR, mainIndexPosition);

    inStreamHDR->setPosition(mainIndexPosition + 4);
    int nrOfSubIndices = inStreamHDR->readByte();
    int subIndex = anyIndex - mainIndex;

    if (subIndex > nrOfSubIndices)
        return 1;

    int i = 0;
    int jumpVal;
    int j = mainIdxAddress;
    int nrOfInBytes;

    inStreamDBS->setPosition(j);
    nrOfInBytes = inStreamDBS->readByte() & 0xff;

    //jump to correct subindex by evaluating the lengths and moving forward accordingly
    for (i = 0; i < subIndex; i++)
    {
        j += nrOfInBytes + 1;
        inStreamDBS->setPosition(j);
        nrOfInBytes = inStreamDBS->readByte() & 0xff;
    }


    return j;
}




int extractTextByIndex(int sourceIdx, bool JAP, bool zenkaku)
{
    String hdr;
    String hdr_orig;
    String dbs;


    if (JAP)
    {
        hdr = "MSGJ.HDR";
        hdr_orig = "MSGJ_orig.HDR";
        dbs = "MSGJ.DBS";
    }
    else
    {
        hdr = "MSG.HDR";
        hdr_orig = "MSG_orig.HDR";
        dbs = "MSG.DBS";
    }

    //create fresh MSGJ.HDR
    File dest = File::getCurrentWorkingDirectory().getChildFile(hdr);
    if (!dest.existsAsFile())
        return 1;  // file doesn't exist
    File src = File::getCurrentWorkingDirectory().getChildFile(hdr_orig);
    if (!src.existsAsFile())
        return 1;  // file doesn't exist
    src.copyFileTo(dest);

    // open MSGJ.HDR
    // CREATE OUTFILE
    File HDR = File::getCurrentWorkingDirectory()
        .getChildFile(hdr);
    if (!HDR.existsAsFile())
        return 1;  // file doesn't exist
    FileInputStream inStreamHDR(HDR); // [2]
    if (!inStreamHDR.openedOk())
        return 1;  // failed to open
    FileOutputStream outStreamHDR(HDR); // [2]
    if (!outStreamHDR.openedOk())
        return 1;  // failed to open


    File DBS = File::getCurrentWorkingDirectory()
        .getChildFile(dbs);
    if (!DBS.existsAsFile())
        return 1;  // file doesn't exist
    FileInputStream inStreamDBS(DBS); // [2]
    if (!inStreamDBS.openedOk())
        return 1;  // failed to open


    int position = findHDRPositionByTextIndex(&inStreamHDR, sourceIdx);
    if (position == 0)
        return 1;

    int retVal = 1;

    getDBSStringByHDRposition(&inStreamHDR, &inStreamDBS, position, JAP, zenkaku);

    return retVal;
}


int parseHDRfile(bool JAP)
{
    String hdr;
    String dbs;
    String csv; 

    if (JAP)
    {
        hdr = "MSGJ.HDR";
        dbs = "MSGJ.DBS";
        csv = "MSGHDRJ.csv";
    }
    else
    {
        hdr = "MSG_rewritten.HDR";
        dbs = "MSG_rewritten.DBS";
        csv = "MSGHDR.csv";
    }

    //this parses a hdr file and writes a csv file with the contents
    File HDR = File::getCurrentWorkingDirectory()
        .getChildFile(hdr);
    if (!HDR.existsAsFile())
        return 1;  // file doesn't exist
    FileInputStream inStreamHDR(HDR); // [2]
    if (!inStreamHDR.openedOk())
        return 1;  // failed to open

    File DBS = File::getCurrentWorkingDirectory()
        .getChildFile(dbs);
    if (!DBS.existsAsFile())
        return 1;  // file doesn't exist
    FileInputStream inStreamDBS(DBS); // [2]
    if (!inStreamDBS.openedOk())
        return 1;  // failed to open

    File CSV = File::getCurrentWorkingDirectory()
        .getChildFile(csv);
    if (!CSV.existsAsFile())
        CSV.create();
    else
    {
        CSV.deleteFile();
        CSV.create();
    }
    FileOutputStream outStreamCSV(CSV); // [2]
    if (!outStreamCSV.openedOk())
        return 1;  // failed to open


    int position = 2; //skip first 2 bytes
    int textIndex = 0;
    int prevAddress = 1;
    int address, nextAddress = 0;
    int nrOfSubIndices = 0;
    int blockTransition = false;
    int isMainIndex = true;
    int DBSsize = 0;

    String delimiter = "\\";
    //outStreamCSV.writeText("Index" + delimiter +"Address" + delimiter +"Size" + delimiter +"SubIndices" + delimiter +"Text\n", false, false, 0);

    bool japaneseOutputZenkaku = false;

    int prevTextIndex, prevNrOfSubIndices = 0;
    //position += 6 * 3; //skip first 3 jap indices

    do {
        prevTextIndex = textIndex;
        prevNrOfSubIndices = nrOfSubIndices;

        inStreamHDR.setPosition(position);
        textIndex = inStreamHDR.readShort();
        if (position>6)
            prevAddress = calculateDBSAddressFromHDRTextIdxposition(&inStreamHDR, position-6);
        address = calculateDBSAddressFromHDRTextIdxposition(&inStreamHDR, position);
        nextAddress = calculateDBSAddressFromHDRTextIdxposition(&inStreamHDR, position+6);

        if (prevAddress / 0x400 == address / 0x400)
            blockTransition = 0;
        else
            blockTransition = 1;

        //calculation if main index or subindex, subindex if it connectes to the previous index and has a memblock transition
        if (blockTransition && (prevTextIndex + prevNrOfSubIndices == textIndex - 1))
            isMainIndex = 0;
        else
            isMainIndex = 1;


        inStreamHDR.setPosition(position + 4);
        nrOfSubIndices = inStreamHDR.readByte();
        int i = 0;

        //getDBSStringByHDRposition(&inStreamHDR, &inStreamDBS, position, JAP, japaneseOutputZenkaku);
        
        //this for loop is for printing all the subindices, takes 30 seconds
        for (i = 0; i <= nrOfSubIndices; i++)
        {
            getDBSStringByHDRtextIndex(&inStreamHDR, &inStreamDBS, textIndex, i, JAP, japaneseOutputZenkaku, &DBSsize);
       
            //if (isMainIndex)
            {
                
                outStreamCSV.writeText(String(textIndex + i), false, false, 0);
                outStreamCSV.writeText(delimiter, false, false, 0);

                
                //outStreamCSV.writeText("0x", false, false, 0);
                outStreamCSV.writeText(String(address), false, false, 0);
                outStreamCSV.writeText(delimiter, false, false, 0);
                /*
                outStreamCSV.writeText("0x", false, false, 0);
                outStreamCSV.writeString(String::toHexString(nextAddress - address));
                outStreamCSV.writeText(delimiter, false, false, 0);
                outStreamCSV.writeString(String(nrOfSubIndices));
                outStreamCSV.writeText(delimiter, false, false, 0);
                outStreamCSV.writeString(String(isMainIndex));
                outStreamCSV.writeText(delimiter, false, false, 0);
                */

                //outStreamCSV.write(testOutputBufferASCII, strlen(testOutputBufferASCII));
                outStreamCSV.writeText("\n", false, false, 0);
                
            }
            address += DBSsize+1;
        }
        position += 6; //index entries are 6 byte long with 2*index, 2*mem position, 1*subindex and 1*block nr


    }// while (!inStreamHDR.isExhausted());
    while (position<0x3000);//for JAP version

    return 0;
}

int createHuffmanTable(bool JAP)
{
    char testOutputBuffer[bufSize]; //just collecting all the inputBufHuff bytes
    char testOutputBufferPrev;

    unsigned int x = 0b10000001;
    char bitLength = 0;

    for (x = 0; x < 0x20000; x++)
    {

        inputBufHuff[0] = 1;
        inputBufHuff[1] = (x & 0x1FFFF) >> 9;
        inputBufHuff[2] = (x & 0x01FF) >> 1;
        inputBufHuff[3] = (x & 0x0001) << 7;

        std::bitset<17> bitSet(x);

        //if (bitSet == 0b1111011010000)
        FUN_decodeHuffman(inputBufHuff, testOutputBuffer, JAP);

        if (testOutputBufferPrev != testOutputBuffer[0])
            std::cout <<
            "{0x" << setfill('0') << setw(2) << right << hex << (int)testOutputBuffer[0]
            << dec << ", 0b" << bitSet << ", " << std::hex << (int)bitLength << "},\n";
        testOutputBufferPrev = testOutputBuffer[0];
    }
    return 0;
}

int writeDBSFromCSV(bool JAP)
{
    //this parses a csv file and writes a hdr/dbs file with the contents

    String hdr;
    String dbs;
    String csv;

    if (JAP)
    {
        return 1; //function only meant for ENG at this time
    }
    else
    {
        hdr = "MSG_rewritten.HDR";
        dbs = "MSG_rewritten.DBS";
        csv = "../../../../../../MSGHDR_indexText.txt"; //RELEASE
        //csv = "../../../MSGHDR_indexText.txt"; //DEBUG
    }


    File CSV = File::getCurrentWorkingDirectory()
        .getChildFile(csv);
    if (!CSV.existsAsFile())
    {
        std::cout << "ERROR: No CSV File detected, exiting" << "\n";
        return 1;  // file doesn't exist
    }
    FileInputStream inStreamCSV(CSV); // [2]
    if (!inStreamCSV.openedOk())
        return 1;  // failed to open


    File HDR = File::getCurrentWorkingDirectory()
        .getChildFile(hdr);
    if (!HDR.existsAsFile())
    {
        std::cout << "ERROR: No HDR File detected, exiting" << "\n";
        return 1;  // file doesn't exist
    }
    FileOutputStream outStreamHDR(HDR); // [2]
    if (!outStreamHDR.openedOk())
        return 1;  // failed to open

    outStreamHDR.setPosition(0);
    outStreamHDR.truncate();

    File DBS = File::getCurrentWorkingDirectory()
        .getChildFile(dbs);
    if (!DBS.existsAsFile())
    {
        std::cout << "ERROR: No DBS File detected, exiting" << "\n";
        return 1;  // file doesn't exist
    }
    FileOutputStream outStreamDBS(DBS); // [2]
    if (!outStreamDBS.openedOk())
        return 1;  // failed to open

    outStreamDBS.setPosition(0);
    outStreamDBS.truncate();

    int linePos = 0;

    int HDRdbsSize = 0, prevHDRdbsSize = 0, HDRsubindices = 0, HDRprevIndex = 0;
    int blockTransition = 0, writeNewIndex = 0, HDRposition = 2, HDRlastSubindexPosition = 0;
    short HDRcounter = 0;


    do
    {
        auto line = inStreamCSV.readNextLine();
        linePos = inStreamCSV.getPosition();

        String indexString = line.upToFirstOccurrenceOf("\\", false, false);
        int index = atoi((indexString.trimCharactersAtEnd("*")).getCharPointer());
        String textContent = line.fromFirstOccurrenceOf("\\", false, false);

        //************** DBS writing **************
        do //handle textContent over multiple lines if there are extra linebreaks
        {
            line = inStreamCSV.readNextLine();

            // no regular line, just append contents to line before
            if (!line.contains("\\"))
            {
                linePos = inStreamCSV.getPosition();
                textContent += "\n"; //append previous linefeed which was dropped by readNextLine();
                textContent += line;
            }
            else
                inStreamCSV.setPosition(linePos); //restore position for next line read

            if (inStreamCSV.isExhausted())
                break;

        } while (!line.contains("\\"));

        const char* debug = textContent.toRawUTF8();
        zeromem(testOutputBufferHuff, bufSize);
        FUN_encodeHuffman(textContent, (uint8_t*)testOutputBufferHuff);

        outStreamDBS.write(testOutputBufferHuff, testOutputBufferHuff[0] + 1);

        //************** HDR writing **************


        if (prevHDRdbsSize / 0x400 == HDRdbsSize / 0x400)
            blockTransition = 0;
        else
            blockTransition = 1;

        prevHDRdbsSize = HDRdbsSize;

        //calculation if main index or subindex, subindex if it connectes to the previous index and has a memblock transition
        if (blockTransition || (HDRprevIndex != index - 1))
            writeNewIndex = 1;
        else
            writeNewIndex = 0;

        HDRprevIndex = index;

        if (writeNewIndex)
        {
            //write previous subindices
            outStreamHDR.setPosition(HDRlastSubindexPosition);
            outStreamHDR.writeByte(HDRsubindices);

            //write new index
            outStreamHDR.setPosition(HDRposition);
            outStreamHDR.writeShort(index);
            outStreamHDR.writeShort(HDRdbsSize % 0x400);
            outStreamHDR.writeByte(HDRsubindices); //will be overwritten next loop, here as placeholder to advance write pos
            outStreamHDR.writeByte(HDRdbsSize / 0x400);

            HDRlastSubindexPosition = HDRposition + 4;
            HDRsubindices = 0;
            HDRposition += 6;
            HDRcounter++;
        }
        else
            HDRsubindices++;

        HDRdbsSize += testOutputBufferHuff[0] + 1;

    } while (!inStreamCSV.isExhausted());

    //closing of file

    // write previous subindices
    outStreamHDR.setPosition(HDRlastSubindexPosition);
    outStreamHDR.writeByte(HDRsubindices);
    // write nr of entries at beginning
    outStreamHDR.setPosition(0);
    outStreamHDR.writeShort(HDRcounter);

    //show size of the DBS
    std::cout << "HDRdbsSize used: " << HDRdbsSize << "\n";

    if (HDRdbsSize < 0x40000)
    {
        std::cout << "HDRdbsSize available: " << 0x40000 - HDRdbsSize << "\n";
    }
    else
    {
        std::cout << "ERROR: DBS size exceeds limit by " << HDRdbsSize - 0x40000 << "\n";
        return 1;
    }
    return 0;
}


int markMainIndices(bool JAP)
{
    // quick helper function. takes japanese translation of all subindices and also csv file where all mainindices are stored.
    // then marks all mainindices in translation file for easiser referencing
    // create english csv only for referencing, because with the mark the building of DBS would have to be adapted


    String inMainIdx, inTxt, outTxt;

    if (JAP)
    {
        inMainIdx = "MSGHDRJ_cleanup.csv";
        inTxt = "ocr_out_GoogleVision_formatiert_GoogleTranslate_ENG.txt";
        outTxt = "ocr_out_GoogleVision_formatiert_GoogleTranslate_ENG_markedMain.txt";
    }
    else
    {
        inMainIdx = "MSGHDR_mainIdxOnly_cleanup.csv";
        inTxt = "MSGHDR_indexText.txt";
        outTxt = "MSGHDR_indexText_markedMain.txt";
    }

    File INMAIN = File::getCurrentWorkingDirectory()
        .getChildFile(inMainIdx);
    if (!INMAIN.existsAsFile())
        return 1;  // file doesn't exist
    FileInputStream inStreamMAIN(INMAIN); // [2]
    if (!inStreamMAIN.openedOk())
        return 1;  // failed to open

    File INTXT = File::getCurrentWorkingDirectory()
        .getChildFile(inTxt);
    if (!INTXT.existsAsFile())
        return 1;  // file doesn't exist
    FileInputStream inStreamTXT(INTXT); // [2]
    if (!inStreamTXT.openedOk())
        return 1;  // failed to open


    File OUTTXT = File::getCurrentWorkingDirectory()
        .getChildFile(outTxt);
    if (!OUTTXT.existsAsFile())
        return 1;  // file doesn't exist
    FileOutputStream outStreamTXT(OUTTXT); // [2]
    if (!outStreamTXT.openedOk())
        return 1;  // failed to open

    //parse MAIN, for each line parse TXT, if index exists, rewerite line with* else rewrite line

    int indexMain, indexTxt;
    bool isMainIdx;

    do
    {
        auto txtLine = inStreamTXT.readNextLine();

        if (!txtLine.containsChar('\\')) //only check lines with indices, not continued lines
            continue;

        String indexStringTxt = txtLine.upToFirstOccurrenceOf("\\", false, false);
        indexTxt = atoi(indexStringTxt.getCharPointer());
        String textContent = txtLine.fromFirstOccurrenceOf("\\", false, false);

        isMainIdx = false;
        inStreamMAIN.setPosition(0);

        do
        {
            auto mainLine = inStreamMAIN.readNextLine();

            String indexString = mainLine.upToFirstOccurrenceOf("\\", false, false);
            indexMain = atoi(indexString.getCharPointer());

            if (indexTxt == indexMain)
            {
                isMainIdx = true;
                break;
            }

            if (indexMain > indexTxt)
                break;


        } while (!inStreamMAIN.isExhausted());


        String outLine = indexStringTxt;
        if (isMainIdx == true)
            outLine.append("*", 1);
        outLine.append("\\", 1);
        outLine.append(textContent, textContent.length());
        outLine.append("\n", 1);


        outStreamTXT.writeText(outLine, false, false, 0);

    } while (!inStreamTXT.isExhausted());

    return 0;
}


int parseTalkScr()
{
    File INTALK = File::getCurrentWorkingDirectory()
        .getChildFile("TALK.SCR");
    if (!INTALK.existsAsFile())
        return 1;  // file doesn't exist
    FileInputStream inStreamTALK(INTALK); // [2]
    if (!inStreamTALK.openedOk())
        return 1;  // failed to open


    File OUTTXT = File::getCurrentWorkingDirectory()
        .getChildFile("TALK.TXT");
    if (!OUTTXT.existsAsFile())
        return 1;  // file doesn't exist
    FileOutputStream outStreamTXT(OUTTXT); // [2]
    if (!outStreamTXT.openedOk())
        return 1;  // failed to open

    short pointer = 0, pointerNext = 0;
    int position = 2; //skip first two bytes
    int dataSize = 0;
    char outLine[100];

    inStreamTALK.setPosition(position); 

    do
    {
        pointer = inStreamTALK.readShort();
        inStreamTALK.setPosition(position+2);
        pointerNext = inStreamTALK.readShort();

        if (pointerNext > pointer)
            dataSize = pointerNext - pointer;
        else
            dataSize = 0x13; //for last string

        inStreamTALK.setPosition(pointer);
        inStreamTALK.read((void*)outLine, dataSize);

        outStreamTXT.write(outLine, dataSize);
        outStreamTXT.write("\n", 1);

        position += 2;
        inStreamTALK.setPosition(position);


    } while (position < 0xC2); //length of pointer table

    return 0;
}


int writeTalkScr()
{
    File INTALK = File::getCurrentWorkingDirectory()
        .getChildFile("..\\..\\..\\..\\..\\..\\TALK.TXT");
    if (!INTALK.existsAsFile())
        return 1;  // file doesn't exist
    FileInputStream inStreamTALK(INTALK); // [2]
    if (!inStreamTALK.openedOk())
        return 1;  // failed to open


    File OUTSCR = File::getCurrentWorkingDirectory()
        .getChildFile("TALK_OUT.SCR");
    if (!OUTSCR.existsAsFile())
        return 1;  // file doesn't exist
    FileOutputStream outStreamSCR(OUTSCR); // [2]
    if (!outStreamSCR.openedOk())
        return 1;  // failed to open

    char endCheckRingBuf[3];

    unsigned int pointerPosition = 2;
    unsigned int dataPosition = 0xC2;
    

    // read byte
    // put byte inte SCR, but also in three-byte ringbuf. 
    // if ringbuf == NUL NUL LF, then update pointer table, drop LF and continue with next block
    outStreamSCR.setPosition(0);
    outStreamSCR.writeShort(0x355);

    outStreamSCR.setPosition(pointerPosition);
    outStreamSCR.writeShort(dataPosition);
    pointerPosition += 2;

    outStreamSCR.setPosition(dataPosition);

    do {
        char inByte = inStreamTALK.readByte();
        

        endCheckRingBuf[2] = endCheckRingBuf[1];
        endCheckRingBuf[1] = endCheckRingBuf[0];
        endCheckRingBuf[0] = inByte;

        if (endCheckRingBuf[0]==0x0A &&
            endCheckRingBuf[1] == 0x00 &&
            endCheckRingBuf[2] == 0x00) //reached end of pointer block
        {
            //update pointer position
            outStreamSCR.setPosition(pointerPosition);
            outStreamSCR.writeShort(dataPosition);
            pointerPosition += 2;

            outStreamSCR.setPosition(dataPosition);
            
        }
        else
        {
            outStreamSCR.writeByte(inByte); //putting the write here drops the \n which is what we want
            dataPosition++;
        }


    } while (pointerPosition <= 0xC2);

    //set last pointer to null as in original, here it would still have a next position calculated
    outStreamSCR.setPosition(pointerPosition-2);
    outStreamSCR.writeShort(0x0000);

    return 0;
}

bool stringContainsFunctionCode(String inStringComplete, String inStringPartial, String *outString, int* outIndexOfCode)
{
    Array<String> compareStringArray;
    
    compareStringArray.add("*=9*,*"); //enable text option, linking info about topic and reply selection appearing
    compareStringArray.add("*=0*,*"); // nothing apparent, maybe text options? only one occurence total
    //compareStringArray.add("*=D*,*"); //return from Sign to normal text, not taken over for size constraints
    compareStringArray.add("*=E*,*"); //Animation scene e.g. Slot machine
    compareStringArray.add("*=F*,*"); //Animation scene
    compareStringArray.add("*=G*,*"); //Small scene? Image of bowl with stone bean, Search light
    //compareStringArray.add("*=H*,*"); //Plaque, not taken over for size constraints
    compareStringArray.add("*=I*,*");
    compareStringArray.add("*=J*,*"); //the texts sound like jump, propel, but nothing happens in debug environment
    //compareStringArray.add("*=K*,*"); //Sign, not taken over for size constraints
    compareStringArray.add("*=L*,*"); //Animation scene, Music?
    compareStringArray.add("*=P*,*"); //Rotate viewpoint
    compareStringArray.add("*=S*,*"); //Play Sound
    compareStringArray.add("*=W*,*"); //Fade to white


    bool codeFound = false;
    int tempIndex = 1000000;

    for (int i = 0; i < compareStringArray.size(); i++)
    {
        //find smallest index
        if (inStringPartial.matchesWildcard(compareStringArray[i], false))
        {
            codeFound = true;
            if (tempIndex > inStringPartial.indexOf(compareStringArray[i].substring(1, 3)))
                tempIndex = inStringPartial.indexOf(compareStringArray[i].substring(1, 3));
        }
    }

    if (codeFound)
    {
        *outString = inStringPartial.substring(tempIndex);
        *outString = outString->upToFirstOccurrenceOf(",", true, true);
        *outIndexOfCode = inStringComplete.indexOf(*outString);
        return true;
    }

    return false;
}

struct textEntry
{
    int index;         // Member (int variable)
    String text;
};

struct functionEntry
{
    int index;
    String functionCode;
    int codePosition;
};

int findArrayPosFromTextIndex(textEntry* myArray, int myArraySize, int textIndex)
{
    for (int i = 0; i < myArraySize; i++)
    {
        if (myArray[i].index == textIndex)
            return i;
    }

    return -1;
}

int compareFunctionCodes()
{
    String inENG = "..\\..\\..\\MSGHDR_indexText.txt";
    String outENG = "MSGHDR_indexText_funCodes.txt";

    String inJAP = "MSGHDRJ_indexText.csv";


    File inFileENG = File::getCurrentWorkingDirectory()
        .getChildFile(inENG);
    if (!inFileENG.existsAsFile())
    {
        std::cout << "ERROR: No inENG File detected, exiting" << "\n";
        return 1;  // file doesn't exist
    }
    FileInputStream inStreamENG(inFileENG); // [2]
    if (!inStreamENG.openedOk())
        return 1;  // failed to open

    File outFileENG = File::getCurrentWorkingDirectory()
        .getChildFile(outENG);
    if (!outFileENG.existsAsFile())
    {
        std::cout << "ERROR: No outENG File detected, exiting" << "\n";
        return 1;  // file doesn't exist
    }
    FileOutputStream outStreamENG(outFileENG); // [2]
    if (!outStreamENG.openedOk())
        return 1;  // failed to open


    File inFileJAP = File::getCurrentWorkingDirectory()
        .getChildFile(inJAP);
    if (!inFileJAP.existsAsFile())
    {
        std::cout << "ERROR: No inJAP File detected, exiting" << "\n";
        return 1;  // file doesn't exist
    }
    FileInputStream inStreamJAP(inFileJAP); // [2]
    if (!inStreamJAP.openedOk())
        return 1;  // failed to open


    //parse both files and put them into table


    // Read ENG
    const int engSize = 10954;
    textEntry textTableENG[engSize];
    int i = 0;
    String functionCode;
    int codePositionENG;
    String tempText;

    Array<functionEntry> funTableENG;
    functionEntry tempFunEntry;
    
    do
    { 
        codePositionENG = -1;
        auto line = inStreamENG.readNextLine();

        String indexString = line.upToFirstOccurrenceOf("\\", false, false);
        int index = atoi((indexString.trimCharactersAtEnd("*")).getCharPointer());
        String textContent = line.fromFirstOccurrenceOf("\\", false, false);

        textTableENG[i].index = index;
        textTableENG[i].text = textContent;

        bool containsFunctionCode = false;

        tempText = textContent;

        do
        {
            containsFunctionCode = stringContainsFunctionCode(textContent, tempText, &functionCode, &codePositionENG);

            if (containsFunctionCode)
            {
                tempFunEntry.index = index;
                tempFunEntry.functionCode = functionCode;
                tempFunEntry.codePosition = codePositionENG;
                funTableENG.add(tempFunEntry);

                //remove function code to recheck the rest of the string in next iteration
                tempText = tempText.fromFirstOccurrenceOf(",", false, false);
            }


        } while (containsFunctionCode);


        i++;

    } while (!inStreamENG.isExhausted() && (i < engSize));
    

    // Read JAP
    const int japSize = 7316;
    textEntry textTableJAP[japSize];
    i = 0;
    int codePositionJAP = -1;

    Array<functionEntry> funTableJAP;
    //functionEntry tempFunEntry;

    do
    {
        codePositionJAP = -1;
        auto line = inStreamJAP.readNextLine();

        String indexString = line.upToFirstOccurrenceOf("\\", false, false);
        int index = atoi((indexString.trimCharactersAtEnd("*")).getCharPointer());
        String textContent = line.fromFirstOccurrenceOf("\\", false, false);

        textTableJAP[i].index = index;
        textTableJAP[i].text = textContent;

        bool containsFunctionCode = false;

        tempText = textContent;

        do
        {
            containsFunctionCode = stringContainsFunctionCode(textContent, tempText, &functionCode, &codePositionJAP);

            if (containsFunctionCode)
            {
                tempFunEntry.index = index;
                tempFunEntry.functionCode = functionCode;
                tempFunEntry.codePosition = codePositionJAP;
                funTableJAP.add(tempFunEntry);

                //remove function code to recheck the rest of the string in next iteration
                tempText = tempText.fromFirstOccurrenceOf(",", false, false);
            }


        } while (containsFunctionCode);

        i++;

    } while (!inStreamJAP.isExhausted() && (i < japSize));


    //display function codes
    int count;
    /*
    count = 0;
    for (int j = 0; j < funTableJAP.size(); j++)
    {
        //if (funTableJAP[j].codePosition != 0)
            std::cout << count++ << " " << funTableJAP[j].index << ": " << funTableJAP[j].codePosition << ": " << funTableJAP[j].functionCode << "\n";

    }
    count = 0;
    for (int j = 0; j < funTableENG.size(); j++)
    {
        //if (funTableJAP[j].codePosition != 0)
        std::cout << count++ << " " << funTableENG[j].index << ": " << funTableENG[j].codePosition << ": " << funTableENG[j].functionCode << "\n";
    }
    */


    /*
    //match indices
    std::cout << "JAP indices missing in ENG version:" << "\n";
    for (int j = 0; j < japSize; j++)
    {
        int textIndexJAP = textTableJAP[j].index;
        int arrayIndexENG = findArrayPosFromTextIndex((textEntry*)textTableENG, engSize, textIndexJAP);

        if (arrayIndexENG == -1) //index doesn´t exist
        {
            std::cout << textIndexJAP << "\n";
        }

    }

    return 0;
   
       */

    //match function Codes
    for (int i = 0; i < funTableJAP.size(); i++)
    {
        int idx = funTableJAP[i].index;
        String funCode = funTableJAP[i].functionCode;

        std::cout << count++ << " " << funTableJAP[i].index << ": " << funTableJAP[i].codePosition << ": " << funTableJAP[i].functionCode << "\t\t\t";


        for (int j = 0; j < funTableENG.size(); j++)
        {
            if ((funTableENG[j].index == idx)&&(funTableENG[j].functionCode == funTableJAP[i].functionCode))
                std::cout << funTableENG[j].index << ": " << funTableENG[j].codePosition << ": " << funTableENG[j].functionCode;
        }

        std::cout << "\n";

    }


    return 0;


    
    //Rebuild English text file with function codes
    for (int k = 0; k < engSize; k++)
    {
        int index = textTableENG[k].index;
        bool indexExistsInENG = false;

        outStreamENG.writeText(String(index), false, false, 0);
        outStreamENG.writeText(String("\\"), false, false, 0);

        for (int i = 0; i < funTableJAP.size(); i++)
        {
            for (int j = 0; j < funTableENG.size(); j++)
            {
                if (funTableENG[j].index == index)
                    indexExistsInENG = true;
            }

            if ((funTableJAP[i].index == index) && (funTableJAP[i].codePosition == 0) && (!indexExistsInENG))
                outStreamENG.writeText(String(funTableJAP[i].functionCode), false, false, 0);

        }


        outStreamENG.writeText(String(textTableENG[k].text), false, false, 0);
        outStreamENG.writeText(String("\n"), false, false, 0);
    }


    
    return 0;
}
