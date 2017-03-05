
#include "crc.hpp"

crc::crc(short frame, short length, binary poly):
		maxFrame(frame),
		maxLength(length),
		polynomial(poly){}
		
		
void crc::Transmitter(ifstream *file){
	size_t i=0,n=0;
	string framesString[this->maxLength/this->maxFrame];
	binary framesBinary[this->maxLength/this->maxFrame];
	string message="";
	
	if (this->ReadEmisor(&message,file)){
		if(message.size() <= 2000){
			ofstream canalcrc("canalcrc.txt");
			if(canalcrc.is_open()){
				while(i < message.size()){
					framesString[n] = message.substr(i,this->maxFrame);
					i+=this->maxFrame;
					n++;
				}
				
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

void crc::Reciever(ifstream *file){
	binary message="";
	int max = this->maxLength/this->maxFrame;
	int counterFrame=0,i,auxI,aux2;
	int vector[max];
	bool band=true;
	size_t indexS=0, indexI=0;
	string mensaje="";
	binary framesBinary[max],auxB;
	
	if(this->ReadReciever(&message,file)){
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
			cout<<"Error en las trama1"<<endl;
		else{
			i = 0;
			while(i<counterFrame && band){
				framesBinary[i]= this->RemoveBitsFilling(framesBinary[i]);
				if(this->CRC16(framesBinary[i]) == "0000000000000000"){
					vector[i]= bitset<4>(framesBinary[i].substr(1,4)).to_ulong();
					framesBinary[i] = framesBinary[i].substr(5,framesBinary[i].size()-21);
					i++;
				}else
					band =false;
			}
			if(!band)
				cout<<"Error en las trama1"<<endl;
			else{
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
				ofstream salidacrc("salidacrc.txt");
				
				if(salidacrc.is_open()){
					i=0;
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
	
bool crc::ReadReciever(binary *message,ifstream *file){
	char charter;
	bool error = false;
	if(file->is_open()){
		while(!file->eof() and !error){
			charter = file->get();
			if(charter=='1' or charter=='0')
				*message += charter;
		}
	}
	
	file->close();	
	return message->size()>0;
}

binary crc::MessageToBinary(string message){
	binary b = "";
	for(size_t i = 0; i < message.size();i++)
		b += bitset<8>(message.c_str()[i]).to_string();
	return b;
}

inline binary crc::Header(int frameCounter, int lastFrame){
	return '0'+bitset<4>(frameCounter).to_string()+bitset<1>(lastFrame).to_string();
}

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
	
	return !error;
}

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
