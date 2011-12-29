CC=g++
OPTS=-ggdb
MODULE_DIR=build/modules
UTIL_DIR=src/Util
CONCURRENCY_DIR=src/Concurrency
NETWORK_DIR=src/Network
SOURCES=$(NETWORK_DIR)/Network.cpp $(NETWORK_DIR)/Network_common.cpp $(CONCURRENCY_DIR)/Concurrency.cpp $(UTIL_DIR)/Timer.cpp
HEADERS=$(UTIL_DIR)/Version.h $(NETWORK_DIR)/Network.h $(NETWORK_DIR)/Network_common.h $(CONCURRENCY_DIR)/Concurrency.h $(UTIL_DIR)/Base.h $(UTIL_DIR)/Timer.h
MODULES=Timer.o Concurrency.o Network_common.o Network.o
INCLUDEDIR=src/
LIBS=
EXECUTABLES=build/*


all: server client
Network.o: $(NETWORK_DIR)/Network.cpp $(NETWORK_DIR)/Network.h
	$(CC) -c $(NETWORK_DIR)/Network.cpp -o $(MODULE_DIR)/Network.o -I $(INCLUDEDIR)
Concurrency.o: $(CONCURRENCY_DIR)/Concurrency.cpp $(CONCURRENCY_DIR)/Concurrency.h 
	$(CC) -c $(CONCURRENCY_DIR)/Concurrency.cpp -o $(MODULE_DIR)/Concurrency.o -I $(INCLUDEDIR)
Timer.o: $(UTIL_DIR)/Timer.cpp $(UTIL_DIR)/Timer.h
	$(CC) -c $(UTIL_DIR)/Timer.cpp -o $(MODULE_DIR)/Timer.o -I$(INCLUDEDIR)
Network_common.o: $(NETWORK_DIR)/Network_common.cpp $(NETWORK_DIR)/Network_common.h
	$(CC) -c $(NETWORK_DIR)/Network_common.cpp -o $(MODULE_DIR)/Network_common.o -I $(INCLUDEDIR)
server: src/Main/Server.cpp $(MODULES) $(HEADERS)
	$(CC) -o build/server src/Main/Server.cpp $(foreach m,$(MODULES),$(MODULE_DIR)/$(m)) $(OPTS) $(LIBS) -I $(INCLUDEDIR)
client: src/Main/Client.cpp $(MODULE_DIR)/$(MODULES) $(HEADERS)
	$(CC) -o build/client src/Main/Client.cpp $(foreach m,$(MODULES),$(MODULE_DIR)/$(m)) $(OPTS) $(LIBS) -I $(INCLUDEDIR)
clean:
	rm $(EXECUTABLES) $(MODULES)
