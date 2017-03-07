# Variables o Macros
EXE=Emisor
OBJS = Emisor.o ../Libreria/hamming.o
CC=g++
FLAGS=-Wall -g -std=c++0x

# Reglas explícitas
# Para linkear todos los objetos y generar un ejecutable
all:$(OBJS)
	$(CC) $(FLAGS)  $(OBJS) -o $(EXE)
#	./$(EXE) 

ddd:$(OBJS)
	$(CC) $(FLAGS)  $(OBJS) -o $(EXE)
	rm $(OBJS)
	ddd $(EXE)
# Generara todos los objetos apartir de las reglas implicitas
../Libreria/%.o:%.cpp
	$(CC) $(FLAGS) -c  $<
# Para eliminar .o y el ejecutable
clean:
	rm -rf $(OBJS) $(EXE) 
# Reglas implícitas
Emisor.o:Emisor.cpp 
hamming.o:../Libreria/hamming.cpp
