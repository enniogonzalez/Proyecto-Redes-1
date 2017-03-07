
#include "../Libreria/hamming.hpp"

void ReadMessage(string *message);

int main(){
	hamming Receptor(125,2000);
	ifstream file("canalh.txt");
	Receptor.Reciever(&file);
	if(file.is_open())
		file.close();
	return 0;
}
