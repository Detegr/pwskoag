CC=g++
OPTS=-std=c++0x -ggdb
SOURCES=Network.cpp Network_common.cpp
LIBS=-lsfml-network -lsfml-system
EXE=a.out

all: main.cpp $(SOURCES)
	$(CC) -o $(EXE) main.cpp $(SOURCES) $(OPTS) $(LIBS)
server: Server.cpp $(SOURCES)
	$(CC) -o server Server.cpp $(SOURCES) $(OPTS) $(LIBS)
clean:
	rm *.o $(EXE)
