CC=g++
OPTS=-std=c++0x -ggdb
LIBS=-lsfml-network -lsfml-system
EXE=a.out

all: main.cpp Network.o
	$(CC) -o $(EXE) main.cpp Network.o $(LIBS) $(OPTS)
Network.o: Network.cpp
	$(CC) -c Network.cpp $(OPTS)
server: Server.cpp Network.o
	$(CC) -o server Server.cpp Network.o $(LIBS) $(OPTS)
clean:
	rm *.o $(EXE)
