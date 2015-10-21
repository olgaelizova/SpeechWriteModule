#include "stdafx.h"

#include <windows.h>
#include <mmsystem.h>
#include <iostream>

#pragma comment(lib,"winmm.lib") 

using namespace std;
/*
int main(){

	HWAVEIN microHandle;
	WAVEHDR waveHeader;

	const int NUMPTS = 22050 * 2;   // 2 seconds
	int sampleRate = 22050;
	short int waveIn[NUMPTS];   // 'short int' is a 16-bit type; I request 16-bit samples below
	// for 8-bit capture, you'd use 'unsigned char' or 'BYTE' 8-bit types

	MMRESULT result = 0;

	WAVEFORMATEX format;
	format.wFormatTag = WAVE_FORMAT_PCM;      // simple, uncompressed format
	format.wBitsPerSample = 8;                //  16 for high quality, 8 for telephone-grade
	format.nChannels = 1;                     //  1=mono, 2=stereo
	format.nSamplesPerSec = sampleRate;       //  22050
	format.nAvgBytesPerSec = format.nSamplesPerSec*format.nChannels*format.wBitsPerSample / 8;
	// = nSamplesPerSec * n.Channels * wBitsPerSample/8
	format.nBlockAlign = format.nChannels*format.wBitsPerSample / 8;
	// = n.Channels * wBitsPerSample/8
	format.cbSize = 0;

	result = waveInOpen(&microHandle, WAVE_MAPPER, &format, 0L, 0L, WAVE_FORMAT_DIRECT);

	if (result)
	{
		cout << "Fail step 1" << endl;
		cout << result << endl;
		Sleep(10000);
		return 0;
	}

	// Set up and prepare header for input
	waveHeader.lpData = (LPSTR)waveIn;
	waveHeader.dwBufferLength = NUMPTS * 2;
	waveHeader.dwBytesRecorded = 0;
	waveHeader.dwUser = 0L;
	waveHeader.dwFlags = 0L;
	waveHeader.dwLoops = 0L;
	waveInPrepareHeader(microHandle, &waveHeader, sizeof(WAVEHDR));

	// Insert a wave input buffer
	result = waveInAddBuffer(microHandle, &waveHeader, sizeof(WAVEHDR));

	if (result)
	{
		cout << "Fail step 2" << endl;
		cout << result << endl;
		Sleep(10000);
		return 0;
	}

	result = waveInStart(microHandle);

	if (result)
	{
		cout << "Fail step 3" << endl;
		cout << result << endl;
		Sleep(10000);
		return 0;
	}

	// Wait until finished recording
	do {} while (waveInUnprepareHeader(microHandle, &waveHeader, sizeof(WAVEHDR)) == WAVERR_STILLPLAYING);

	waveInClose(microHandle);

	//for (int i = 0; i < NUMPTS/2; i++)
	//	printf("%d\n",microHandle[i]);

	system("pause");

	return 0;
}
*/