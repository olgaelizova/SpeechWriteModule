#include "stdafx.h"

#ifdef _WIN32

#include <iostream>
#include <windows.h>
#include <mmsystem.h>
#include <fstream>

#pragma comment(lib,"winmm.lib") 

#include "WavReader.h"

using namespace std;

#define _CRT_SECURE_NO_WARNINGS

//const int NUMPTS = 22050 * 1 * 5;   // 5 seconds for one channel
//const int NUMPTS = 22050 * 2 * 5;  // 5 seconds for two channels
const int NUMPTS = 22050 * 1 * 2;   // 1 seconds for one channel
int sampleRate = 22050;
short int waveIn[NUMPTS];

struct WAVHEADER
{
	char riffId[4];
	unsigned long riffSize;
	char waveId[4];
	char fmtId[4];

	unsigned long fmtSize;
	unsigned short audioFormat;
	unsigned short numChannels;
	unsigned long sampleRate;
	unsigned long byteRate;
	unsigned short blockAlign;
	unsigned short bitsPerSample;

	char listId[4];
	unsigned int listSize;
	char infoInamId[8];
	unsigned int infoInamSize;

	char chunkId[4];
	unsigned long chunkSize;
}header;

#include <conio.h>
void pushButtonHandler(int &flag)
{
	char x;

	x = _getch();
	switch (x) //перед этим было switch(_getch())
	{
	case 32:
		//cout << "Space button" << endl;
		cout << endl;
		flag = 1;
		break;
	case 13:
		//cout << "Enter button" << endl;
		cout << endl;
		flag = 2;
		break;
	case 8:
		//cout << "Backspace button" << endl;
		cout << endl;
		flag = 3;
		break;
	}
}


void main() //main
{
	int flag = 0;

	//pushButtonHandler(flag);  //It is working

//// rabotaet no ne pishet v fail
		WAVEFORMATEX pFormat;
		pFormat.wFormatTag = WAVE_FORMAT_PCM;     // simple, uncompressed format
		pFormat.nChannels = 1;                    //  1=mono, 2=stereo
		pFormat.wBitsPerSample = 16;              //  16 for high quality, 8 for telephone-grade
		pFormat.nSamplesPerSec = sampleRate;
		pFormat.nAvgBytesPerSec = sampleRate * pFormat.nChannels * pFormat.wBitsPerSample / 8;
		pFormat.nBlockAlign = pFormat.nChannels * pFormat.wBitsPerSample / 8;
		pFormat.cbSize = 0;

		cout << "Press Space for write voice" << endl;

		pushButtonHandler(flag);

	if (flag == 1)
	{
		HWAVEIN hWaveIn;
		WAVEHDR waveInHdr;

		waveInOpen(&hWaveIn, WAVE_MAPPER, &pFormat, 0L, 0L, WAVE_FORMAT_DIRECT);

		waveInHdr.lpData = (LPSTR)waveIn;
		waveInHdr.dwBufferLength = NUMPTS * 2;
		waveInHdr.dwBytesRecorded = 0;
		waveInHdr.dwUser = 0L;
		waveInHdr.dwFlags = 0L;
		waveInHdr.dwLoops = 0L;

		waveInPrepareHeader(hWaveIn, &waveInHdr, sizeof(WAVEHDR));

		waveInAddBuffer(hWaveIn, &waveInHdr, sizeof(WAVEHDR));

		cout << "Voice writing started..." << endl;
		waveInStart(hWaveIn);

		// Wait until finished recording
		do{} while (waveInUnprepareHeader(hWaveIn, &waveInHdr, sizeof(WAVEHDR)) == WAVERR_STILLPLAYING);

		waveInClose(hWaveIn);

		cout << "Voice writing ended." << endl;

		Sleep(50); // for time until next input
		cout << endl;
		flag = 0;
	}

	cout << "Press:" << endl;
	cout << "Enter button for write voice into WAV file" << endl;
	cout << "Space button for playback voice" << endl;
	cout << "Backspace button for exit" << endl;

	pushButtonHandler(flag);

	if (flag == 2)
	{
		cout << "Writing WAV file..." << endl;

		////// writing into WAV file
		header.riffId[0] = 'R';
		header.riffId[1] = 'I';
		header.riffId[2] = 'F';
		header.riffId[3] = 'F';
		header.riffSize = header.chunkSize + 136 + 92; // +136  // add + 92
		header.waveId[0] = 'W';
		header.waveId[1] = 'A';
		header.waveId[2] = 'V';
		header.waveId[3] = 'E';
		header.fmtId[0] = 'f';
		header.fmtId[1] = 'm';
		header.fmtId[2] = 't';
		header.fmtId[3] = ' ';
		header.fmtSize = 16;
		header.audioFormat = pFormat.wFormatTag;
		header.sampleRate = sampleRate;
		header.bitsPerSample = pFormat.wBitsPerSample;
		header.numChannels = pFormat.nChannels;
		header.byteRate = pFormat.nAvgBytesPerSec;
		header.blockAlign = pFormat.nBlockAlign;

		header.listId[0] = 'L';
		header.listId[1] = 'I';
		header.listId[2] = 'S';
		header.listId[3] = 'T';
		header.listSize = 16;

		header.infoInamId[0] = 'I';
		header.infoInamId[1] = 'N';
		header.infoInamId[2] = 'F';
		header.infoInamId[3] = 'O';
		header.infoInamId[4] = 'I';
		header.infoInamId[5] = 'N';
		header.infoInamId[6] = 'A';
		header.infoInamId[7] = 'M';
		header.infoInamSize = 64 ; 

		header.chunkId[0] = 'd';
		header.chunkId[1] = 'a';
		header.chunkId[2] = 't';
		header.chunkId[3] = 'a';
		header.chunkSize = sizeof(waveIn);

		FILE* pf;

		fopen_s(&pf, "zap1.wav", "wb");
		fwrite((HPSTR)&header, sizeof(header), 1, pf);
		fwrite((HPSTR)waveIn, 1, sizeof(waveIn), pf);

		fclose(pf);

		//вывод всех параметров записи
		FILE *file;
		errno_t err;
		err = fopen_s(&file, "zap1.wav", "rb");

		if (err)
		{
			printf_s("Failed open file, error %d", err);
		}

		cout << "Output all parameters of new WAV file: " << endl;

		fread_s(&header, sizeof(WAVHEADER), sizeof(WAVHEADER), 1, file);
		printf_s("riffid %s\n", header.riffId);
		printf_s("riffsize %d\n", header.riffSize);
		printf_s("format %s\n", header.waveId);
		printf_s("fmtid %s\n", header.fmtId);
		printf_s("fmtsize: %d\n", header.fmtSize);
		printf_s("audioformat: %d\n", header.audioFormat);
		printf_s("blockalign: %d\n", header.blockAlign);
		printf_s("chunkId: %s\n", header.chunkId);
		printf_s("chunksize: %d\n", header.chunkSize);
		printf_s("Sample rate: %d\n", header.sampleRate);
		printf_s("Channels: %d\n", header.numChannels);
		printf_s("listid %s\n", header.listId);
		printf_s("listsize: %d\n", header.listSize);
		printf_s("infoInamid %s\n", header.infoInamId);
		printf_s("infoInamsize: %d\n", header.infoInamSize);
		printf_s("Bits per sample: %d\n", header.bitsPerSample);
		printf_s("Byterate %d\n", header.byteRate);

		fclose(file);

		//////

		Sleep(20); 

		flag = 3;  // flag for exit
	}

	if (flag == 1)
	{
		cout << "Voice palyback started" << endl;
		HWAVEOUT hWaveOut;
		WAVEHDR WaveOutHdr;

		waveOutOpen(&hWaveOut, WAVE_MAPPER, &pFormat, 0L, 0L, WAVE_FORMAT_DIRECT);

		WaveOutHdr.lpData = (LPSTR)waveIn;
		WaveOutHdr.dwBufferLength = NUMPTS * 2;
		WaveOutHdr.dwBytesRecorded = 0;
		WaveOutHdr.dwUser = 0L;
		WaveOutHdr.dwFlags = 0L;
		WaveOutHdr.dwLoops = 1L;

		waveOutPrepareHeader(hWaveOut, &WaveOutHdr, sizeof(WAVEHDR));

		waveOutSetVolume(hWaveOut, 0xFFFFFFFF);

		waveOutWrite(hWaveOut, &WaveOutHdr, sizeof(WAVEHDR));

		waveOutClose(hWaveOut);

		flag = 3; // flag for exit

		Sleep(5000);  // for playback without close console

	}

	if (flag==3)
	{
		system("pause");
	}

}

#else
#ifdef __unix__
//TODO realization for UNIX
#else
#error Unsupported Implementation
#endif
#endif
