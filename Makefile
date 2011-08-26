CC=g++
OPTS=-std=c++0x -ggdb
SOURCES=Network.cpp Network_common.cpp Concurrency.cpp
HEADERS=Network.h Network_common.h Network_commands.h Concurrency.h Base.h
LIBS=-lsfml-network -lsfml-system
EXE=a.out
SERVER=server
CLIENT=client

all: server client
server: Server.cpp $(SOURCES) $(HEADERS)
	$(CC) -o $(SERVER) Server.cpp $(SOURCES) $(OPTS) $(LIBS)
client: Client.cpp $(SOURCES) $(HEADERS)
	$(CC) -o $(CLIENT) Client.cpp $(SOURCES) $(OPTS) $(LIBS)
clean:
	rm $(EXE) $(SERVER) $(CLIENT)
