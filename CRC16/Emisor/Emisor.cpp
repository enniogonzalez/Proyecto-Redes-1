
#include "../Libreria/crc.hpp"

void ReadMessage(string *message);

int main(){
	crc Emisor(125,2000,"11000000000000101");
	ifstream file("entrada.txt");
	Emisor.Transmitter(&file);
	
	if(file.is_open())
		file.close();
	return 0;
}
