# Variables o Macros
EXE=Receptor
OBJS = Receptor.o ../Libreria/hamming.o
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
Receptor.o:Receptor.cpp 
hamming.o:../Libreria/hamming.cpp
