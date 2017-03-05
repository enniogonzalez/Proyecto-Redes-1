
#include "../Libreria/crc.hpp"

void ReadMessage(string *message);

int main(){
	crc hola(125,2000,"11000000000000101");
	ifstream file("entrada.txt");
	hola.Transmitter(&file);
	return 0;
}
