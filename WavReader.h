#include "stdafx.h"
#include <stdio.h>
#include <memory.h>
#include <iostream>
#include <fstream>

using namespace std;

#pragma once

typedef unsigned short	WORD;
typedef unsigned int	IWORD;
typedef short			INT16;

struct wav_header_t  // wav header
{
	char rId[4]; //"RIFF" = 0x46464952
	IWORD rLen; //28 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes] + sum(sizeof(chunk.id) + sizeof(chunk.size) + chunk.size)
	char wId[4]; //"WAVE" = 0x45564157
	char fId[4]; //"fmt " = 0x20746D66
	IWORD fLen; //16 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes]
	WORD wFormatTag;
	WORD nChannels; // kolvo kanalov, 1-mono, 2-stereo
	IWORD nSamplesPerSec;  // frequency - default 22050
	IWORD nAvgBytesPerSec;
	WORD nBlockAlign;	// bait na sempl
	WORD wBitsPerSample;  // tochnost zvuch. 8bit, 16bit...
	//[WORD wExtraFormatBytes;]
	//[Extra format bytes]
};

struct chunk_t  // dannye wav
{
	char id[4]; //"data" = 0x61746164
	IWORD size; //Chunk data bytes
};

double* wavread(const char* fullpath,int &samples_count); 