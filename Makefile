CC=g++
OPTS=-ggdb
SOURCES=Network.cpp Network_common.cpp Concurrency.cpp Timer.cpp
HEADERS=Network.h Network_common.h Network_commands.h Concurrency.h Base.h Timer.h
LIBS=
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
