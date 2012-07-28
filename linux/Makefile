CC=g++ -g3
#********************************************
# CLIENT
CLIENTBIN=	../src/Main/gfxtest.cpp
CLIENTSRC=	../src/Network/Client.cpp \
		../src/Graphics/Graphics.cpp \
		../src/Game/ClientPlayer.cpp
CLIENTHEADERS=	../src/Network/Client.h \
		../src/Game/ClientPlayer.h \
		../src/Graphics/PlayerGfx.h \
		../src/Graphics/Drawable.h \
		../src/Graphics/Graphics.h \
CLIENTOBJECTS=$(CLIENTSRC:.cpp=.o)

#********************************************
# SERVER
SERVERBIN=	../src/Main/Server.cpp
SERVERSRC=	../src/Network/Server.cpp \
			../src/Game/ServerPlayer.cpp
SERVERHEADERS=	../src/Network/Server.h \
				../src/Game/ServerPlayer.h
SERVEROBJECTS=$(SERVERSRC:.cpp=.o)
		
#********************************************
# LIBPWSKOAG
LIBSRC= $(wildcard ../src/Util/*.cpp)
LIBHEADERS= ../src/Network/ThreadData.h \
	../src/Game/Player.h \
	../src/Game/Keyboard.h \
	$(wildcard ../src/Util/*.h)
LIBOBJECTS=$(LIBSRC:.cpp=.shared.o)

#********************************************

INCLUDE=-I../src -I../
CLIBS=-L. -lpthread -lpwskoag
FPIC=-fPIC
ALL=client server libpwskoag.so

all: $(ALL)
client: $(CLIENTBIN) $(CLIENTOBJECTS) libpwskoag.so
	$(CC) $(CLIENTBIN) -Wl,-rpath,.,-rpath,/usr/local/lib -o $@ $(CLIENTOBJECTS) $(INCLUDE) $(CLIBS) $(SFML)
server:	$(SERVERBIN) $(SERVEROBJECTS) libpwskoag.so
	$(CC) $(SERVERBIN) -Wl,-rpath,. -o $@ $(SERVEROBJECTS) $(INCLUDE) $(CLIBS)
libpwskoag.so: $(LIBOBJECTS)
	$(CC) -fPIC -shared -Wl,-soname,$@ -o $@ $(LIBOBJECTS)
%.o : %.cpp
	$(CC) -c -MMD $< -o $@ $(INCLUDE) $(CLIBS)
%.shared.o : %.cpp
	$(CC) -c -fPIC -MMD $< -o $@ $(INCLUDE) $(CLIBS)
$(LIBOBJECTS) : $(LIBSRC) $(LIBHEADERS)
$(SERVEROBJECTS): $(SERVERSRC) $(SERVERHEADERS)
$(CLIENTOBJECTS): $(CLIENTSRC) $(CLIENTHEADERS)
clean:
	-rm $(ALL)
	-rm $(shell find ../src \( -name '*.shared.o' -o -name '*.d' -o -name '*.o' \))

.PHONY: clean all
