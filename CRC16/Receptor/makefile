# Variables o Macros
EXE=main
OBJS = main.o ../Libreria/crc.o
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
main.o:main.cpp 
crc.o:../Libreria/crc.cpp
