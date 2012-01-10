CC=g++ -g3
#********************************************
# CLIENT
CLIENTBIN=	../src/Main/gfxtest.cpp
CLIENTSRC=	../src/Network/Client.cpp \
		../src/Graphics/Graphics.cpp
CLIENTHEADERS=	../src/Network/Client.h \
		../src/Game/ClientPlayer.h \
		../src/Graphics/PlayerGfx.h \
		../src/Graphics/Drawable.h \
		../src/Graphics/Graphics.h
CLIENTOBJECTS=$(CLIENTSRC:.cpp=.o)

#********************************************
# SERVER
SERVERBIN=	../src/Main/Server.cpp
SERVERSRC=	../src/Network/Server.cpp
SERVERHEADERS=	../src/Network/Server.h \
		../src/Game/ServerPlayer.h
SERVEROBJECTS=$(SERVERSRC:.cpp=.o)
		
#********************************************
# LIBPWSKOAG
LIBSRC=	$(wildcard ../src/Concurrency/*.cpp) \
	../src/Network/Network.cpp \
	../src/Network/Packet.cpp \
	../src/Game/Player.cpp \
	$(wildcard ../src/Util/*.cpp)
LIBHEADERS=../src/Concurrency/*.h \
	../src/Network/Network.h \
	../src/Network/Packet.h \
	../src/Network/ThreadData.h \
	../src/Game/Player.h \
	../src/Game/Keyboard.h \
	$(wildcard ../src/Util/*.h)
LIBOBJECTS=$(LIBSRC:.cpp=.shared.o)

#********************************************

INCLUDE=../src
CLIBS=-L. -lpthread -lpwskoag
SFML=-lsfml-system -lsfml-window -lsfml-graphics
FPIC=-fPIC
ALL=client server libpwskoag.so

all: $(ALL)
client: $(CLIENTOBJECTS) libpwskoag.so
	$(CC) $(CLIENTBIN) -Wl,-rpath,.,-rpath,/usr/local/lib -o $@ $(CLIENTOBJECTS) -I $(INCLUDE) $(CLIBS) $(SFML)
server:	$(SERVEROBJECTS) libpwskoag.so
	$(CC) $(SERVERBIN) -Wl,-rpath,. -o $@ $(SERVEROBJECTS) -I $(INCLUDE) $(CLIBS)
libpwskoag.so: $(LIBOBJECTS)
	$(CC) -fPIC -shared -Wl,-soname,$@ -o $@ $(LIBOBJECTS)
%.o : %.cpp
	$(CC) -c -MMD $< -o $@ -I $(INCLUDE) $(CLIBS)
%.shared.o : %.cpp
	$(CC) -c -fPIC -MMD $< -o $@ -I $(INCLUDE) $(CLIBS)
$(LIBOBJECTS) : $(LIBSRC) $(LIBHEADERS)
$(SERVEROBJECTS): $(SERVERSRC) $(SERVERHEADERS)
clean:
	-rm $(ALL)
	-rm $(shell find ../src \( -name '*.shared.o' -o -name '*.d' \))

.PHONY: clean all
