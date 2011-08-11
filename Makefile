CC=g++
OPTS=-std=c++0x -ggdb
SOURCES=Network.cpp Network_common.cpp
LIBS=-lsfml-network -lsfml-system
EXE=a.out
SERVER=server
CLIENT=client

all: main.cpp $(SOURCES)
	$(CC) -o $(EXE) main.cpp $(SOURCES) $(OPTS) $(LIBS)
server: Server.cpp $(SOURCES)
	$(CC) -o $(SERVER) Server.cpp $(SOURCES) $(OPTS) $(LIBS)
client: Client.cpp $(SOURCES)
	$(CC) -o $(CLIENT) Client.cpp $(SOURCES) $(OPTS) $(LIBS)
clean:
	rm $(EXE) $(SERVER) $(CLIENT)
