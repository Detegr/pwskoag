CC=g++
OPTS=-ggdb -g3
MODULE_DIR=modules
UTIL_DIR=../src/Util
CONCURRENCY_DIR=../src/Concurrency
NETWORK_DIR=../src/Network
GFX_DIR=../src/Graphics
GAME_DIR=../src/Game
HEADERS=$(UTIL_DIR)/Version.h $(NETWORK_DIR)/Network.h $(NETWORK_DIR)/Network_common.h $(CONCURRENCY_DIR)/Concurrency.h $(UTIL_DIR)/Base.h $(UTIL_DIR)/Timer.h $(NETWORK_DIR)/Packet.h $(GFX_DIR)/Graphics.h $(GAME_DIR)/Player.h $(GAME_DIR)/ClientPlayer.h $(NETWORK_DIR)/ThreadData.h
MODULES=Timer.o Concurrency.o Network_common.o Network.o Packet.o Graphics.o Player.o
INCLUDEDIR=../src/
LIBS=-lpthread -framework Cocoa -framework OpenGL -framework SFML -framework sfml-graphics -framework sfml-window -framework sfml-system
FRAMEWORKS=-F/Xcode3.1.3/SDKs/MacOSX10.5.sdk/System/Library/Frameworks/ 
EXECUTABLES=client server


all: server client gfxtest
Network.o: $(NETWORK_DIR)/Network.cpp $(NETWORK_DIR)/Network.h
	$(CC) -c $(NETWORK_DIR)/Network.cpp $(OPTS) -o $(MODULE_DIR)/Network.o -I $(INCLUDEDIR) $(FRAMEWORKS)
Concurrency.o: $(CONCURRENCY_DIR)/Concurrency.cpp $(CONCURRENCY_DIR)/Concurrency.h 
	$(CC) -c $(CONCURRENCY_DIR)/Concurrency.cpp $(OPTS) -o $(MODULE_DIR)/Concurrency.o -I $(INCLUDEDIR)
Timer.o: $(UTIL_DIR)/Timer.cpp $(UTIL_DIR)/Timer.h
	$(CC) -c $(UTIL_DIR)/Timer.cpp $(OPTS) -o $(MODULE_DIR)/Timer.o -I$(INCLUDEDIR)
Packet.o: $(NETWORK_DIR)/Packet.cpp $(NETWORK_DIR)/Packet.h
	$(CC) -c $(NETWORK_DIR)/Packet.cpp $(OPTS) -o $(MODULE_DIR)/Packet.o -I $(INCLUDEDIR)
Network_common.o: $(NETWORK_DIR)/Network_common.cpp $(NETWORK_DIR)/Network_common.h
	$(CC) -c $(NETWORK_DIR)/Network_common.cpp $(OPTS) -o $(MODULE_DIR)/Network_common.o -I $(INCLUDEDIR)
Graphics.o: $(GFX_DIR)/Graphics.cpp $(GFX_DIR)/Graphics.h
	$(CC) -c $< $(OPTS) -o $(MODULE_DIR)/$@ -I $(INCLUDEDIR) $(FRAMEWORKS)
Player.o: $(GAME_DIR)/Player.cpp $(GAME_DIR)/Player.h
	$(CC) -c $< $(OPTS) -o $(MODULE_DIR)/$@ -I $(INCLUDEDIR) $(FRAMEWORKS)
server: ../src/Main/Server.cpp $(HEADERS) $(MODULES)
	$(CC) -o server ../src/Main/Server.cpp $(foreach m,$(MODULES),$(MODULE_DIR)/$(m)) $(OPTS) $(LIBS) -I $(INCLUDEDIR)
client: ../src/Main/Client.cpp $(HEADERS) $(MODULES)
	$(CC) -o client ../src/Main/Client.cpp $(foreach m,$(MODULES),$(MODULE_DIR)/$(m)) $(OPTS) $(LIBS) -I $(INCLUDEDIR)
gfxtest: ../src/Main/gfxtest.cpp $(HEADERS) $(MODULES)
	$(CC) -o gfxtest ../src/Main/gfxtest.cpp $(foreach m,$(MODULES),$(MODULE_DIR)/$(m)) $(OPTS) $(LIBS) -I $(INCLUDEDIR) $(FRAMEWORKS)
clean:
	rm $(EXECUTABLES) $(MODULE_DIR)/$(MODULES)
