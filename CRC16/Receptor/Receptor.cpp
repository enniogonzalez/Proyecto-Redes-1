
#include "../Libreria/crc.hpp"

void ReadMessage(string *message);

int main(){
	crc Receptor(125,2000,"11000000000000101");
	ifstream file("canalcrc.txt");
	Receptor.Reciever(&file);
	if(file.is_open())
		file.close();
	return 0;
}
