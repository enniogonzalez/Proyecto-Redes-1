#ifndef CRC_HPP
#define CRC_HPP


#include <iostream>
#include <fstream>
#include <string>
#include <bitset>

using namespace std;
typedef string binary;

class crc{
	
	private:
		short maxFrame;
		short maxLength;
		binary polynomial;
		binary MessageToBinary(string message);
		inline binary Header(int frameCounter, int lastFrame);
		binary BitsFilling(binary frame);
		binary RemoveBitsFilling(binary frame);
		binary CRC16(binary frameIn);
		bool ReadEmisor(string *message, ifstream *file);
		bool ReadReciever(binary *message, ifstream *file);
	public:
		crc(short frame, short length, binary poly);
		void Transmitter(ifstream *file);
		void Reciever(ifstream *file);
};

#endif

