CC=g++
OPTS=-ggdb
SOURCES=Network.cpp Network_common.cpp Concurrency.cpp Timer.cpp
HEADERS=Version.h Network.h Network_common.h Concurrency.h Base.h Timer.h
MODULES=Network.o Concurrency.o Timer.o Network_common.o
LIBS=
SERVER=server
CLIENT=client
EXECUTABLES=$(SERVER) $(CLIENT)

all: server client
Network.o: Network.cpp Network.h
	$(CC) -c Network.cpp
Concurrency.o: Concurrency.cpp Concurrency.h
	$(CC) -c Concurrency.cpp
Timer.o: Timer.cpp Timer.h
	$(CC) -c Timer.cpp
Network_common.o: Network_common.cpp Network_common.h
	$(CC) -c Network_common.cpp
server: Server.cpp $(MODULES) $(HEADERS)
	$(CC) -o $(SERVER) Server.cpp $(MODULES) $(OPTS) $(LIBS)
client: Client.cpp $(MODULES) $(HEADERS)
	$(CC) -o $(CLIENT) Client.cpp $(MODULES) $(OPTS) $(LIBS)
clean:
	rm $(EXECUTABLES) $(MODULES)
