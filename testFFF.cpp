#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>

//TODO: ��������� ������� ������   

class Chunk {
public:

	typedef std::vector<char> STREAM;
	//�� ������ � int
	static unsigned int inline getIDByName(const char * nameString) {
		return *reinterpret_cast < const unsigned int * > (nameString);
	}

	//�� int � ������
	static std::string inline getNameByID(unsigned int value) {
		char * pNow = reinterpret_cast < char * > (&value);
		return std::string(pNow, pNow + sizeof(value));
	}

	//������������
	Chunk() : data(0, 0, STREAM()) {}

	Chunk(int nameID, int size, const STREAM & dataStream) : data(nameID, size, dataStream) {
	}

	std::string inline get_name() const {
		return getNameByID(data.name);
	}

	unsigned int inline get_nameID() const {
		return data.name;
	}
	unsigned int inline get_size() const {
		return data.size;
	}
	void set_nameID(unsigned int value) {
		data.name = value;
	}
	void set_size(unsigned int value) {
		data.size = value;
		data.data.resize(value);
	}
	void set_stream(const STREAM & value) {
		data.data = value;
		data.size = value.size();
	}
	STREAM  get_data() {
		return data.data;
	}

private:
	struct chunk {
		unsigned int name;
		unsigned int size;
		STREAM data;
		chunk(int name_, int size_, const STREAM & data_) : name(name_), size(size_), data(data_) {
		}
	};
	chunk data;
};


class WaveFile {

private:
	Chunk RIFF;
	unsigned int RIFFTYPE;
	struct fmt{
		unsigned short audioFormat; //���� �������� �� ����� 1 , ������ ������������ �����-�� ������. PCM == 1
		unsigned short numChannels; //���������� �������
		unsigned int sampleRate; //������� �������������
		unsigned int byteRate; //���������� ���� ���������� �� ������� ���������������
		unsigned short blockAlign; //���������� ���� ��� ������ ������, ������� ��� ������
		unsigned short bitsPerSample; //���������� ��� � ������. ������� ��������.
		unsigned short extraFmtBytes; //���������� �������������� ����
	};
	std::vector < Chunk > collection;

public:

	//�����������, �������� Wav �����
	WaveFile(const std::string & filePath) {
		std::ifstream iFile(filePath, std::ios_base::binary);
		unsigned int temp_riffNameId;
		unsigned int temp_riffSize;

		//RIFF
		iFile.read(reinterpret_cast < char * > (&temp_riffNameId), sizeof(temp_riffNameId));
		//�������� �� ������������ RIFF
		if (RIFF.getNameByID(temp_riffNameId) != "RIFF"){
			throw std::string("Error. Not RIFF");
		}

		//������
		iFile.read(reinterpret_cast < char * > (&temp_riffSize), sizeof(temp_riffSize));

		RIFF.set_nameID(temp_riffNameId);
		RIFF.set_size(temp_riffSize);

		iFile.read(reinterpret_cast < char * > (&RIFFTYPE), sizeof(RIFFTYPE));
		//�������� �� ������������ WAVE
		if (RIFF.getNameByID(RIFFTYPE) != "WAVE"){
			throw std::string("Error. Not WAVE");
		}

		while (iFile.read(reinterpret_cast < char * > (&temp_riffNameId), sizeof(temp_riffNameId))) {
			iFile.read(reinterpret_cast < char * > (&temp_riffSize), sizeof(temp_riffSize));
			Chunk::STREAM vec(temp_riffSize);
			iFile.read(reinterpret_cast < char * > (&vec[0]), temp_riffSize);
			collection.push_back(Chunk(temp_riffNameId, temp_riffSize, vec));
		}
	}


	static char* StreamToChar(const Chunk::STREAM &v_data){
		//int size = v_data.size();

		char *data = new char[v_data.size()];

		for (unsigned int i = 0; i<v_data.size(); ++i){
			data[i] = v_data[i];
		}

		return data;
	}

	static Chunk::STREAM CharsToStream(const char* data, int size){
		return Chunk::STREAM(data, data + size);
	}

	fmt* CharsToFmt(char * data_){
		fmt *dat;
		return dat = reinterpret_cast<fmt* >(data_);
	}

	void ShowInfo(std::ostream & out) {

		out << RIFF.get_name() << "\n"
			<< RIFF.get_size() << "\n"
			<< Chunk::getNameByID(RIFFTYPE) << "\n";
		for (std::vector < Chunk > ::const_iterator it = collection.begin(); it != collection.end(); ++it) {
			out << it->get_name() << "\n"
				<< it->get_size() << std::endl;
		}
		out << std::endl;

		char *data = StreamToChar(collection[0].get_data());
		fmt * dat = CharsToFmt(data);

		out << "audio format: " << dat->audioFormat << "\n"
			<< "num channels: " << dat->numChannels << "\n"
			<< "sample rate: " << dat->sampleRate << "\n"
			<< "byte rate: " << dat->byteRate << "\n"
			<< "block Align: " << dat->blockAlign << "\n"
			<< "bits per sample: " << dat->bitsPerSample << "\n"
			<< "extra fmt bytes: " << dat->extraFmtBytes << std::endl;
		delete[] data;
	}


	inline int getIndexChunkByName(const std::string & chunkName){
		int num = -1;
		for (unsigned int i = 0; i<collection.size(); i++){
			if (collection[i].get_name() == chunkName){
				num = i;
				break;
			}
		}
		return num;
	}

	inline int getSizeAData(){
		int size = collection[getIndexChunkByName("data")].get_size();
		return size;
	}

	inline char* getAData() {
		//�������� ������ ����� data � �������� �� � char*
		char *data = StreamToChar(collection[getIndexChunkByName("data")].get_data());
		return data;

	}


	void setAData(Chunk::STREAM data){
		collection[getIndexChunkByName("data")].set_stream(data);
	}

	//FMT!!!!!!!!!!!!!!!!!!!!!!
	void setFMTData(fmt * fmt){
		collection[getIndexChunkByName("fmt ")].set_stream(CharsToStream(reinterpret_cast<char*>(fmt), collection[getIndexChunkByName("fmt ")].get_size()));

	}

	void SaveADataInTxt(const std::string &nameFile){
		std::ofstream file(nameFile, std::ios_base::binary);

		const int size = getSizeAData();
		char *data = getAData();
		file.write(data, size);
		delete[] data;
		file.close();
	}

	void CreateAndWrite(const std::string &fileName){
		std::ofstream file(fileName, std::ios_base::binary);
		int size = collection.size();

		unsigned int temp_riffNameId = RIFF.get_nameID();
		unsigned int temp_riffSize = RIFF.get_size();

		file.write(reinterpret_cast<char*>(&temp_riffNameId), sizeof(temp_riffNameId));
		file.write(reinterpret_cast<char*>(&temp_riffSize), sizeof(temp_riffSize));
		file.write(reinterpret_cast<char*>(&RIFFTYPE), sizeof(RIFFTYPE));

		for (int i = 0; i < size; i++){

			temp_riffNameId = collection[i].get_nameID();
			temp_riffSize = collection[i].get_size();

			file.write(reinterpret_cast<char*>(&temp_riffNameId), sizeof(temp_riffNameId));
			file.write(reinterpret_cast<char*>(&temp_riffSize), sizeof(temp_riffSize));

			char *data = StreamToChar(collection[i].get_data());
			file.write(data, temp_riffSize);
		}
		file.close();
	}

	//��������� �����������


	//��������/����������/������������ �����

	/*
	void SoundNormalization( unsigned short val , unsigned short bits_per_sample ){
	//TODO: ������������� ����
	int MAX, MIN, SILENT;

	if(bits_per_sample == 16){ // ��� 16-�������
	MAX = 32767;  // ������������ ��������
	MIN = -32768; // ����������� ��������
	SILENT = 0;   // ����� ������
	}

	}
	*/


	void SoundEnhancement(unsigned short int k){
		//���������!!!!!!!!!!!

		char *data_ = StreamToChar(collection[0].get_data());
		fmt * dat = CharsToFmt(data_);

		char* data = getAData();
		unsigned int size = getSizeAData();
		char *pData = new char[size];

		short temp_sample;
		for (unsigned int i = 0; i<size; i += dat->blockAlign){

			if (dat->blockAlign == 2){
				temp_sample = 256 * data[i] + data[i + 1];
			}
			else if (dat->blockAlign == 1){
				temp_sample = data[i];
			}

			temp_sample *= k;

			if (dat->blockAlign == 2){
				pData[i] = temp_sample / 256;
				pData[i + 1] = temp_sample - 256 * pData[i];
			}
			else if (dat->blockAlign == 1){
				pData[i] = temp_sample;
			}

		}

		setAData(CharsToStream(pData, size));
		delete[] data_;
		delete[] data;
	}

	//���������� �����
	void AttenuatesTheSound(unsigned short int k){
		//���������!!!!!!!!!!!!!!!!!!
		if (k == 0) throw std::string("Division by zero");
		char *data_ = StreamToChar(collection[0].get_data());
		fmt * dat = CharsToFmt(data_);

		char* data = getAData();
		unsigned int size = getSizeAData();
		char *pData = new char[size];

		short temp_sample;
		for (unsigned int i = 0; i<size; i += dat->blockAlign){

			if (dat->blockAlign == 2){
				temp_sample = 256 * data[i] + data[i + 1];
			}
			else if (dat->blockAlign == 1){
				temp_sample = data[i] - 128; //�������� ����� ������ � ������� ������� ��������
			}

			temp_sample /= k;

			if (dat->blockAlign == 2){
				pData[i] = temp_sample / 256;
				pData[i + 1] = temp_sample - 256 * pData[i];
				//pData[i]=reinterpret_cast<char*>(temp_sample);
			}
			else if (dat->blockAlign == 1){
				pData[i] = temp_sample + 128; //�������� �������� ����� ������ == 128
			}

		}

		setAData(CharsToStream(pData, size));
		delete[] data_;
		delete[] data;
	}

	//������� �������������
	void UpSampleRate(){//���������� � 2 ����
		//TODO: ��������� � ��������� ������������ ������ �����������/2!!!!!!!!!
		Chunk::STREAM dataOld = collection[getIndexChunkByName("data")].get_data();
		Chunk::STREAM fmtBuffer = collection[getIndexChunkByName("fmt ")].get_data();
		fmt * pFMT = reinterpret_cast < fmt* > (fmtBuffer.data());
		Chunk::STREAM dataNew(dataOld.size() * 2);

		for (size_t i = 0, j = 0; i < dataOld.size(); i += pFMT->blockAlign, j += pFMT->blockAlign * 2) {
			for (size_t d = 0; d < pFMT->blockAlign; ++d) {
				dataNew[j + d] = dataOld[i + d];
				dataNew[j + d + pFMT->blockAlign] = dataOld[i + d];
			}
		}
		setAData(dataNew);
		RIFF.set_size(RIFF.get_size() + (dataNew.size() - dataOld.size()));
		pFMT->sampleRate *= 2;
		pFMT->byteRate *= 2;
		setFMTData(pFMT);
	}

	short LinearInterpolation(const double x1, const short y1, const double x2, const short y2, const double x){
		return y1 + ((y2 - y1) / (x2 - x1))*(x - x1); //������� �������� ������������
	}

	//������� �������������
	void DownSampleRate(){//���������� � 2 ����
		Chunk::STREAM dataOld = collection[getIndexChunkByName("data")].get_data();
		Chunk::STREAM fmtBuffer = collection[getIndexChunkByName("fmt ")].get_data();
		fmt * pFMT = reinterpret_cast < fmt* > (fmtBuffer.data());
		Chunk::STREAM dataNew;
		dataNew.resize(dataOld.size() / 2 + dataOld.size() % 2 + pFMT->blockAlign);

		for (size_t i = 0, j = 0; i < dataOld.size(); i += pFMT->blockAlign * 2, j += pFMT->blockAlign) {
			for (size_t d = 0; d < pFMT->blockAlign; ++d)
				dataNew[j + d] = dataOld[i + d];
			std::cout << dataOld.size() << "\n" << dataNew.size() << "\n" << i << "\n" << j << std::endl;
		}

		setAData(dataNew);
		RIFF.set_size(RIFF.get_size() + (dataNew.size() - dataOld.size()));
		pFMT->sampleRate /= 2;
		pFMT->byteRate /= 2;
		setFMTData(pFMT);
	}



	//������ �������� ����������
	unsigned int inline XorShift(unsigned int &x, unsigned int &y, unsigned int &z, unsigned int &w){

		unsigned int t;
		t = (x ^ (x << 11));
		x = y; y = z; z = w;
		return(w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
	}

	unsigned int GenerationKey(unsigned int &x, unsigned int &y, unsigned int &z, unsigned int &w){//!!!!!
		return XorShift(x, y, z, w);
	}
	unsigned char* getBufferFromKey(unsigned int &value){//!!!!!
		return reinterpret_cast <unsigned char * > (&value);
	}

	void ImpositionOfNoise(){
		//���������� ����������� � ������ ����� � ��������� ����
		std::ofstream file("key.txt", std::ios_base::binary);
		unsigned int x = 0x440000A4;
		unsigned int y = 0x00127C00;
		unsigned int z = 0x0400FFA4;
		unsigned int w = 0x10235011;

		unsigned int keyUInt;
		unsigned char * key;

		int size = getSizeAData();
		char * data = getAData();

		for (int i = 0; i < size; i++){
			if (i % 4 == 0) {
				keyUInt = GenerationKey(x, y, z, w);
				key = getBufferFromKey(keyUInt);
			}
			data[i] ^= key[i % 4];
			file.write((char*)&key[i % 4], 1);
		}
		collection[getIndexChunkByName("data")].set_stream(CharsToStream(data, size));

	}


	void RemovingNoise(const std::string name){
		//������ ����
		std::ifstream file(name, std::ios_base::binary);

		int size = getSizeAData();
		char * data = getAData();
		//std::string key;
		char* key = new char[size];
		file.read(key, size);

		for (int i = 0; i < size; i++){
			data[i] ^= key[i];//!!!!!
		}
		delete[] key;
		collection[getIndexChunkByName("data")].set_stream(CharsToStream(data, size));
	}


	/*
	void IntroductionTxtFile8bit(const std::string name){
	//�������� txt ����� � 8������ wav, ���������� 2 ������� ����
	//TODO: ������� �� ���
	std::ifstream file(name, std::ios_base::binary);
	int num_collection = getNumChunkData();

	char *data_ = StreamToChar(collection[0].get_data());
	fmt * dat = CharsToFmt(data_);
	//delete[] data_;

	char* data = getAData();
	int size = getSizeAData();

	char *f_data = new char[size];
	if(file){
	file.read(f_data, size);

	for(int i = 0, j = 0; i < size; i+= dat->blockAlign*4, j++){
	std::bitset<8> byte_1(data[i]);
	std::bitset<8> byte_2(data[i+1]);
	std::bitset<8> byte_3(data[i+2]);
	std::bitset<8> byte_4(data[i+3]);

	std::bitset<8> byte_from_file(f_data[j]);

	byte_1[6] = byte_from_file[0];
	byte_1[7] = byte_from_file[1];

	byte_2[6] = byte_from_file[2];
	byte_2[7] = byte_from_file[3];

	byte_3[6] = byte_from_file[4];
	byte_3[7] = byte_from_file[5];

	byte_4[6] = byte_from_file[6];
	byte_4[7] = byte_from_file[7];

	data[i] = static_cast<char>(byte_1.to_ulong());
	data[i+1] = static_cast<char>(byte_2.to_ulong());
	data[i+2] = static_cast<char>(byte_3.to_ulong());
	data[i+3] = static_cast<char>(byte_4.to_ulong());
	}
	}
	delete[] f_data;
	setAData(CharsToStream(data, size));
	}



	void ExtractionTxtFile8bit(){
	//TODO: ������� txt ����
	std::ofstream file("out_stego8bit.txt", std::ios_base::binary);

	char *data_ = StreamToChar(collection[0].get_data());
	fmt * dat = CharsToFmt(data_);
	//delete[] data_;

	int size = getSizeAData();
	char *data = getAData();

	char *f_data = new char[size];
	int i = 0, j = 0;
	//for(int i = 0, j = 0; i < size; i+= dat->bitsPerSample*4, j++){
	while(i<size){

	std::bitset<8> byte_1(data[i]);
	std::bitset<8> byte_2(data[i+1]);
	std::bitset<8> byte_3(data[i+2]);
	std::bitset<8> byte_4(data[i+3]);

	std::bitset<8> byte_from_file(f_data[j]);

	byte_from_file[0] = byte_1[6];
	byte_from_file[1] = byte_1[7];

	byte_from_file[2] = byte_2[6];
	byte_from_file[3] = byte_2[7];

	byte_from_file[4] = byte_3[6];
	byte_from_file[5] = byte_3[7];

	byte_from_file[6] = byte_4[6];
	byte_from_file[7] = byte_4[7];

	f_data[j] = static_cast<char>(byte_from_file.to_ulong());

	i+= dat->blockAlign*4;
	j++;

	}
	file.write(f_data, size);

	delete[] data;
	delete[] f_data;


	}
	*/

	bool IntroductionFile(const std::string &  fileName, size_t count_bit) {
		std::ifstream file(fileName, std::ios_base::binary);

		Chunk::STREAM streamFmt(collection[getIndexChunkByName("fmt ")].get_data());
		fmt * pFmt = reinterpret_cast<fmt*>(&(streamFmt[0]));
		Chunk::STREAM streamData(collection[getIndexChunkByName("data")].get_data());
		unsigned char * pData = reinterpret_cast<unsigned char*>(&(streamData[0]));

		int bitPerSample = pFmt->bitsPerSample;
		int samplesCount = streamData.size() / bitPerSample;

		file.seekg(0, std::ios_base::end);//������� ������ ������� � ����� � ����� �����
		int fileSize = file.peek() + 1;//��������� ������� �������
		unsigned int bitsInFile = fileSize * 8;
		if (bitsInFile > (samplesCount * count_bit)) {
			return false;
		}

		unsigned char * pText = new unsigned char[fileSize];
		pText[fileSize - 1] = 0;
		file.seekg(0, std::ios_base::beg);//������� ������� � ����� �� ������
		file.read((char*)pText, fileSize - 1);

		unsigned char mask = -1; //��������� ����� ��������� ���������
		mask <<= count_bit;//��������� ������� ���� ����� ������
		for (int i = bitPerSample / 8 - 1, textPos = 0; i < samplesCount; i += bitPerSample / 8, textPos += count_bit){
			pData[i] &= mask;//��������� ���� ������ �������� �� �����

			int nowByte = textPos / 8;
			unsigned char mult = (pText[nowByte] << (textPos % 8)) >> (8 - textPos % 8);
			if ((textPos % 8 + count_bit) > 8) {
				unsigned char adv_mask = -1;
				adv_mask <<= (textPos % 8 + count_bit) - 8;
				adv_mask &= (pText[nowByte + 1] >> (8 - ((textPos % 8 + count_bit) - 8)));
				mult <<= (textPos % 8 + count_bit) - 8;
				mult &= adv_mask;
			}
			pData[i] &= mult;
		}

		delete[] pText;

		return true;
	}
};

/**int main(){

	try{
		WaveFile wf("test.wav");
		wf.ShowInfo(std::cout);

		std::cout << "\n\n" << std::endl;
		//  wf.SaveADataInTxt("out.txt");

		//  wf.SoundEnhancement(4);
		//  wf.CreateAndWrite("tralala.wav");
		//  wf.AttenuatesTheSound(5);
		//  wf.CreateAndWrite("tralala.wav");
		//  wf.ImpositionOfNoise();
		//  wf.CreateAndWrite("tralala.wav");
		//wf.SaveADataInTxt("out_noise.txt");


		wf.DownSampleRate();
		wf.CreateAndWrite("sampleRate_down.wav");
		wf.ShowInfo(std::cout);

		std::cout << "\n\n" << std::endl;



		wf.UpSampleRate();
		wf.CreateAndWrite("sampleRate_up.wav");
		wf.ShowInfo(std::cout);


		//  wf.RemovingNoise("key.txt");
		//  wf.SaveADataInTxt("out_no_noise.txt");
		//  wf.CreateAndWrite("tralala_withoutnoise.wav");

		//wf.IntroductionTxtFile8bit("sli.txt");
		//wf.CreateAndWrite("sli.wav");

		//wf.ExtractionTxtFile8bit();

	}
	catch (std::string msg){
		std::cerr << msg << std::endl;
	};

}
*/