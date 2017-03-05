
#include "../Libreria/crc.hpp"

void ReadMessage(string *message);

int main(){
	crc hola(125,2000,"11000000000000101");
	ifstream file("canalcrc.txt");
	hola.Reciever(&file);
	return 0;
}
