#ifndef HAMMING_HPP
#define HAMMING_HPP


#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <cmath>
#include <sstream>

using namespace std;
typedef string binary;

class hamming{
	
	private:
		//Atributos de la clase
		short maxFrame;
		short maxLength;
		
		//Metodos Privados
		binary MessageToBinary(string message);
		inline binary Header(int frameCounter, int lastFrame);
		binary BitsFilling(binary frame);
		binary RemoveBitsFilling(binary frame);
		binary Coder(const binary frameIn);
		binary Decoder(const binary frameIn,bool *error, bool *fixed);
		bool ReadEmisor(string *message, ifstream *file);
		bool ReadReciever(binary *message, ifstream *file);
		string Descomposicion(unsigned short number,unsigned short potencias);
		string ErrorManager(unsigned short errorNumber);
		
	public:
		//Metodos proveidos al usuario
		hamming(short frame, short length);
		void Transmitter(ifstream *file);
		void Reciever(ifstream *file);
};

#endif

