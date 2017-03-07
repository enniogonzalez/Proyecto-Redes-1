
#include "../Libreria/hamming.hpp"

void ReadMessage(string *message);

int main(){
	hamming Emisor(125,2000);
	ifstream file("entrada.txt");
	Emisor.Transmitter(&file);
	
	if(file.is_open())
		file.close();
	return 0;
}
