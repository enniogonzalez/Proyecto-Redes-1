
#include "crc.hpp"

//Constructor
crc::crc(short frame, short length, binary poly):
		maxFrame(frame),
		maxLength(length),
		polynomial(poly){}
		
//Metodo usado por el emisor		
void crc::Transmitter(ifstream *file){
	size_t i=0,n=0;
	
	//Vectores con el tamaño maximo de tramas permitidas por
	//el protocolo elegido
	string framesString[this->maxLength/this->maxFrame];
	binary framesBinary[this->maxLength/this->maxFrame];
	string message="";
	
	//Se hace la lectura del archivo de entrada
	if (this->ReadEmisor(&message,file)){
		
		//Se verifica que el tamaño del mensaje sea menor a 
		//2000 caracteres
		if(message.size() <= 2000){
			
			//En caso de que el archivo de entrada este bien
			//se procede a formatear el archivo de salida (binario)
			ofstream canalcrc("canalcrc.txt");
			if(canalcrc.is_open()){
				
				//Se hace la division del mensaje original en la 
				//cantidad de tramas apropiadas segun el protocolo
				while(i < message.size()){
					framesString[n] = message.substr(i,this->maxFrame);
					i+=this->maxFrame;
					n++;
				}
				
				//Se convierte el mensaje a binario, se le agrega
				//la cabecera, se le aplica CRC, relleno de bits
				//y se le agregan las banderas
				for(i=0;i<n;i++){
					framesBinary[i] =this->Header(i,i==n-1)+ this->MessageToBinary(framesString[i]);
					framesBinary[i] = "01111110"+this->BitsFilling(framesBinary[i]+this->CRC16(framesBinary[i]))+"01111110";
					canalcrc<<framesBinary[i]<<endl;
				}
				canalcrc.close();
			}
		}
	}
	else
		cout<<"El archivo entrada.txt contiene caracteres invalidos"<<endl;
}

//Metodo usado para el Receptor
void crc::Reciever(ifstream *file){
	
	//Variables necesarias para el procedimiento
	binary message="";
	int max = this->maxLength/this->maxFrame;
	int counterFrame=0,i,auxI,aux2;
	int vector[max];
	bool band=true, ordenado = true;
	size_t indexS=0, indexI=0;
	string mensaje="";
	binary framesBinary[max],auxB;
	
	//Se hace la lectura del canal para validar que sea valido
	//el contenido
	if(this->ReadReciever(&message,file)){
		
		//Se localizan las banderas y se proceden a armar las tramas
		//segun las banderas para aplicar el proceso inverso que hizo
		//el emisor
		while(counterFrame <= max+1 && band && indexI<message.size()){
			if(message.substr(indexI,8)=="01111110"){
				indexI = indexI+8;
				indexS = message.find("01111110",indexI);
				if(indexS != string::npos){
					counterFrame++;
					if(counterFrame==max+1)
						band = false;
					else
						framesBinary[counterFrame-1] = message.substr(indexI,indexS-indexI);
					indexI = indexS+8;
				}else
					band = false;
			}else
				band = false;
		}	
		
		if(!band)
			cout<<"Error en las trama"<<endl;
		else{
			i = 0;
			
			//Se procede a quitar los bits de relleno y a verificar que
			//el CRC este apto y se guarda un vector con los numeros
			//de tramas en el orden en el que llegaron para despues
			//ordenar
			while(i<counterFrame && band){
				framesBinary[i]= this->RemoveBitsFilling(framesBinary[i]);
				if(this->CRC16(framesBinary[i]) == "0000000000000000"){
					vector[i]= bitset<4>(framesBinary[i].substr(1,4)).to_ulong();
					if(ordenado and i>0 and vector[i]<vector[i-1])
						ordenado=false;
					framesBinary[i] = framesBinary[i].substr(5,framesBinary[i].size()-21);
					i++;
				}else
					band =false;
			}
			if(!band)
				cout<<"Error en las tramas"<<endl;
			else{
				
				//En caso de que las tramas lleguen desordenadas,
				//se ordenan
				if(!ordenado){
					for(i=0;i<counterFrame-1;i++){
						auxI = i;
						auxB = framesBinary[i];
						for(int j=i+1;j<counterFrame;j++)
							if(vector[auxI]>vector[j])
								auxI = j;
								
						aux2 = vector[auxI];
						vector[auxI] = vector[i];
						vector[i]=aux2;
						auxB = framesBinary[auxI];
						framesBinary[auxI] = framesBinary[i];
						framesBinary[i]=auxB;
						
					}
				}
				ofstream salidacrc("salidacrc.txt");
				
				if(salidacrc.is_open()){
					i=0;
					//Se pasan de binario a caracteres las tramas,
					//y se leen hasta encontrar el caracter de
					//finalizacion
					while(i<counterFrame && band){
						for(size_t j=1;j<framesBinary[i].size();j+=8)
							mensaje += (char)(bitset<8>(framesBinary[i].substr(j,8)).to_ulong());
						if(framesBinary[i][0] == '1')
							band = false;
						i++;
					}
					cout<<mensaje<<endl;
					salidacrc<<mensaje<<endl;
					salidacrc.close();
				}
			}
		}
	}else
		cout<<"Archivo vacio"<<endl;
}
	
//Leer canal
bool crc::ReadReciever(binary *message,ifstream *file){
	char charter;
	bool error = false;
	
	//Solo se leen 0's y 1's
	if(file->is_open()){
		while(!file->eof() and !error){
			charter = file->get();
			if(charter=='1' or charter=='0')
				*message += charter;
		}
	}
	
	file->close();	
	
	//Se verifica que el canal no este vacio
	return message->size()>0;
}

//Leer la entrada
bool crc::ReadEmisor(string *message, ifstream *file){
	char charter;
	bool error = false;
	
	if(file->is_open()){
		while(!file->eof() and !error){
			charter = file->get();
			/*if(charter<32 or charter > 126){
				error = true;
				cout<<(int)charter;
			}*/
			
			if(charter>=32 and charter <= 126)
				*message += charter;
		}
	}
	file->close();
	message->c_str();
	
	return message->size() > 0;
}

//Convertir mensaje en binario
binary crc::MessageToBinary(string message){
	binary b = "";
	for(size_t i = 0; i < message.size();i++)
		b += bitset<8>(message.c_str()[i]).to_string();
	return b;
}

//Agregar cabeceras
inline binary crc::Header(int frameCounter, int lastFrame){
	return '0'+bitset<4>(frameCounter).to_string()+bitset<1>(lastFrame).to_string();
}

//Aplicar CRC
binary crc::CRC16(binary frameIn){
	binary frame,resto="",aux="";
	size_t lengthPol = this->polynomial.size();
	frame=frameIn+"0000000000000000";
	
	for(size_t i=0;i<frame.size()-16;i++){
		if (frame[i]=='1'){
			aux=frame.substr(i,lengthPol);
			resto="";
			for (size_t j = 0; j < lengthPol; j++) {
			   if(aux[j]==this->polynomial[j])
				 resto=resto+'0';
			   else
				 resto=resto+'1';
			}
			frame=frame.substr(0,i)+resto+frame.substr(i+lengthPol);
		}
	}
    return frame.substr(frameIn.size());
}

//Relleno de bits
binary crc::BitsFilling(binary frame){
	binary out="";
	int counter=0;
	for(size_t i = 0;i<frame.size();i++){
		counter = (frame[i]=='0' ? 0:counter+1);
		out+=frame[i];
		if(counter==5){
			out+='0';
			counter=0;
		}
	}
	return out;
}

//Quitar bits de relleno
binary crc::RemoveBitsFilling(binary frame){
	binary out="";
	int counter=0;
	size_t i=0;
	while(i<frame.size()){
		counter = (frame[i]=='0' ? 0:counter+1);
		out+=frame[i];
		if(counter==5){
			counter=0;
			i+=2;
		}else
			i++;
	}
	return out;
}
