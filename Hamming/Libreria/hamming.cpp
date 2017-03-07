
#include "hamming.hpp"

//Constructor
hamming::hamming(short frame, short length):
		maxFrame(frame),
		maxLength(length){}


string hamming::ErrorManager(unsigned short errorNumber){
	string ErrorMessage;
	
	switch(errorNumber){
		case 1: ErrorMessage="El archivo entrada.txt contiene caracteres invalidos o esta vacio"; break;
		case 2: ErrorMessage="El archivo entrada.txt contiene una cantidad de caracteres no permitida"; break;
		case 3: ErrorMessage="El archivo canalh.txt contiene caracteres invalidos o esta vacio"; break;
		case 4: ErrorMessage="El archivo canalh.txt contiene un numero de tramas no permitidas"; break;
		case 5: ErrorMessage="El archivo canalh.txt contiene error en las banderas de las tramas"; break;
		case 6: ErrorMessage="Se detecto un error y fue corregido"; break;
		case 7: ErrorMessage="Se detecto un error y no fue corregido"; break;
		case 8: ErrorMessage="Se detecto tramas duplicadas"; break;
		case 9: ErrorMessage="Se detecto la ausencia de una trama"; break;
		case 10: ErrorMessage="No se detectaron tramas finalizadoras"; break;
		case 11: ErrorMessage="Se detectaron mas de 1 trama finalizadoras"; break;
	}
	
	return ErrorMessage;
}

//Metodo usado por el emisor		
void hamming::Transmitter(ifstream *file){
	size_t i=0,n=0;
	
	//Vectores con el tamaño maximo de tramas permitidas por
	//el protocolo elegido
	string framesString[this->maxLength/this->maxFrame];
	binary framesBinary[this->maxLength/this->maxFrame];
	string message="",errorMessage="";
	
	ofstream canalh("canalh.txt");
	//Se hace la lectura del archivo de entrada
	if (this->ReadEmisor(&message,file)){
		
		//Se verifica que el tamaño del mensaje sea menor a 
		//2000 caracteres
		if(message.size() <= 2000){
			
			//En caso de que el archivo de entrada este bien
			//se procede a formatear el archivo de salida (binario)
			if(canalh.is_open()){
				
				//Se hace la division del mensaje original en la 
				//cantidad de tramas apropiadas segun el protocolo
				while(i < message.size()){
					framesString[n] = message.substr(i,this->maxFrame);
					i+=this->maxFrame;
					n++;
				}
				
				//Se convierte el mensaje a binario, se le agrega
				//la cabecera, se le aplica hamming, relleno de bits
				//y se le agregan las banderas
				for(i=0;i<n;i++){
					framesBinary[i] =this->Header(i,i==n-1)+ this->MessageToBinary(framesString[i]);
					framesBinary[i] ="01111110"+this->BitsFilling(this->Coder(framesBinary[i]))+"01111110";
					canalh<<framesBinary[i]<<endl;
				}
				canalh.close();
			}
		}else
			errorMessage=this->ErrorManager(2);
	}
	else
		errorMessage=this->ErrorManager(1);
		
	if(errorMessage!=""){
		canalh<<errorMessage<<endl;
		cout<<errorMessage<<endl;		
	}
	
	if(canalh.is_open())
		canalh.close();	
}

//Metodo usado para el Receptor
void hamming::Reciever(ifstream *file){
	
	//Variables necesarias para el procedimiento
	binary message="";
	int max = this->maxLength/this->maxFrame;
	int counterFrame=0,i,auxI,aux2;
	int vector[max];
	bool vectorLleno[max]={0};
	bool band=true, ordenado = true,error=true,fixed=true;
	size_t indexS=0, indexI=0;
	string mensaje="",errorMessage="";
	binary framesBinary[max],auxB,orderedFrames[max];

	ofstream salidahamming("salidah.txt");
	
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
					if(counterFrame==max+1){
						band = false;
						errorMessage = this->ErrorManager(4);
					}
					else
						framesBinary[counterFrame-1] = message.substr(indexI,indexS-indexI);
					indexI = indexS+8;
				}else{
					band = false;
					errorMessage = this->ErrorManager(5);
				}
			}else{
				band = false;
				errorMessage = this->ErrorManager(5);
			}
		}	
		
		if(band){
			i = 0;
			//Se procede a quitar los bits de relleno y a verificar que
			//el hamming este apto y se guarda un vector con los numeros
			//de tramas en el orden en el que llegaron para despues
			//ordenar
			while(i<counterFrame && band){
				framesBinary[i]= Decoder(this->RemoveBitsFilling(framesBinary[i]),&error,&fixed);
				
				if(fixed){
					if(error){
						errorMessage = this->ErrorManager(6);
						
					}
					
					auxI = bitset<4>(framesBinary[i].substr(1,4)).to_ulong();
					
					if(vectorLleno[auxI]){
						band = false;
						errorMessage = this->ErrorManager(8);
					}else{
						vectorLleno[auxI]=true;
						orderedFrames[auxI]= framesBinary[i].substr(5,framesBinary[i].size());
					}
					
					i++;
				}else{
					band = false;
					errorMessage = this->ErrorManager(7);
				}
			}
			
			if(band){
				i=0;
				auxI=0;
				while(i<counterFrame && band){
					if(!vectorLleno[i]){
						errorMessage= this->ErrorManager(9);
						band = false;
					}else if(orderedFrames[i][0]=='1')
						auxI++;
					framesBinary[i]=orderedFrames[i];
					i++;
				}
				
				if(band && auxI != 1){
					errorMessage= this->ErrorManager((auxI==0)?10:11);
					band = false;
				}
				
				if(band && salidahamming.is_open()){
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
					//cout<<mensaje<<endl;
					salidahamming<<mensaje<<endl;
				}
			}
		}
	}else
		errorMessage = this->ErrorManager(3);
	
	
	if(errorMessage!=""){
		salidahamming<<errorMessage<<endl;
		cout<<errorMessage<<endl;		
	}
	
	if(salidahamming.is_open())
		salidahamming.close();	
	
}
	
//Leer canal
bool hamming::ReadReciever(binary *message,ifstream *file){
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
bool hamming::ReadEmisor(string *message, ifstream *file){
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
binary hamming::MessageToBinary(string message){
	binary b = "";
	for(size_t i = 0; i < message.size();i++)
		b += bitset<8>(message.c_str()[i]).to_string();
	return b;
}

//Agregar cabeceras
inline binary hamming::Header(int frameCounter, int lastFrame){
	return '0'+bitset<4>(frameCounter).to_string()+bitset<1>(lastFrame).to_string();
}

//Codificador Hamming
binary hamming::Coder(const binary frameIn){
	string mensaje;
	binary framesBinary[this->maxLength];
	string sumatorias[this->maxLength];
	bool binaryHamming[this->maxLength]={0};
	bool paridadTrama = false,paridadPotencia =false;
	unsigned short t=0,unos=0,pariedad,exponente;
	size_t positions,n=0,i=0;
	
	positions=frameIn.size();
	while(n<positions){
		if((i+1) == pow(2,t)){
			t++;
			sumatorias[i]="";
		}
		else{
			if(frameIn[n]=='1')
				paridadTrama = !paridadTrama;
			binaryHamming[i]=frameIn[n]-48;
			sumatorias[i]=this->Descomposicion(i+1,t);
			n++;
		}
		i++;
	}
	
	positions+=t;
	
	for(i=0;i<t;i++){
		std::stringstream aux;
		exponente =pow(2,i);
		aux<<','<<exponente<<',';
		paridadPotencia=false;
		for(n=0;n<positions;n++)
			if(sumatorias[n].find(aux.str(),0)!=string::npos)
				if(binaryHamming[n])
					paridadPotencia = !paridadPotencia;
		
		if(paridadPotencia){
			binaryHamming[exponente-1] =1;
			paridadTrama=!paridadTrama;
		}else
			binaryHamming[exponente-1] =0;
	}
	
	for(i=0;i<positions;i++)
		mensaje+=(binaryHamming[i]?'1':'0');
		
	return mensaje+((paridadTrama)?'1':'0');
}

//Decodificador Hamming
binary hamming::Decoder(const binary frameIn,bool *error, bool *fixed){
	string mensaje;
	binary framesBinary[this->maxLength];
	string sumatorias[this->maxLength];
	bool binaryHamming[frameIn.size()]={0};
	bool paridadSumas,paridadTrama=false;
	unsigned short t=0,unos=0,exponente,sumaHamming=0;
	size_t positions,n=0,i=0;
	
	positions=frameIn.size();
	while(i<positions){
		if((i+1) == pow(2,t)){
			t++;
			sumatorias[i]="";
		}else
			sumatorias[i]=this->Descomposicion(i+1,t);
		binaryHamming[i]=frameIn[i]-48;
		if(binaryHamming[i])
			paridadTrama=!paridadTrama;
		i++;
	}
	for(i=0;i<t;i++){
		std::stringstream aux;
		exponente =pow(2,i);
		aux<<','<<exponente<<',';
		paridadSumas=false;
		for(n=0;n<positions;n++)
			if(sumatorias[n].find(aux.str(),0)!=string::npos)
				if(binaryHamming[n])
					paridadSumas=!paridadSumas;
		if(paridadSumas != binaryHamming[exponente-1])
			sumaHamming+=exponente;
	}
	
	if(sumaHamming==0)
		*error = false;
	else if (paridadTrama == binaryHamming[positions-1])
		*fixed = false;
	else
		binaryHamming[sumaHamming-1] =!binaryHamming[sumaHamming-1];
	
	t=0;
	for(i=0;i<positions-1;i++)
		if ((i+1) == pow(2,t))
			t++;
		else
			mensaje+=(binaryHamming[i]?'1':'0');
			
	return mensaje;
	
}

string hamming::Descomposicion(unsigned short number,unsigned short potencias){
	std::stringstream aux;
	unsigned short i,p;
	i=0;
	while(number>0 && i<=potencias){
		p=pow(2,potencias);
		if(p<=number){
			number -=p;
			aux <<','<<p<<',';
		}
		potencias--;	
	}
    return aux.str();
}
		
//Relleno de bits
binary hamming::BitsFilling(binary frame){
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
binary hamming::RemoveBitsFilling(binary frame){
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
