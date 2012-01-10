CC=g++ -g3
SERVERBIN=	../src/Main/Server.cpp
SERVERSRC=	../src/Network/Server.cpp
SERVERHEADERS=	../src/Network/Server.h \
		../src/Game/ServerPlayer.h
SERVEROBJECTS=$(SERVERSRC:.cpp=.o)
		
LIBSRC=	$(wildcard ../src/Concurrency/*.cpp) \
	../src/Network/Network.cpp \
	../src/Network/Packet.cpp \
	$(wildcard ../src/Util/*.cpp)
LIBHEADERS=../src/Concurrency/*.h \
	../src/Network/Network.h \
	../src/Network/Packet.h \
	../src/Network/ThreadData.h \
	$(wildcard ../src/Util/*.h)
LIBOBJECTS=$(LIBSRC:.cpp=.shared.o)
INCLUDE=../src
CLIBS=-lpthread
FPIC=-fPIC

server:	$(SERVEROBJECTS)
	$(CC) $(SERVERBIN) -o $@ $(SERVEROBJECTS) -I $(INCLUDE) $(CLIBS) -L. -lpwskoag
libpwskoag.so: $(LIBOBJECTS)
	$(CC) -fPIC -shared -Wl,-soname,$@ -o $@ $(LIBOBJECTS)
%.o : %.cpp
	$(CC) -c -MMD $< -o $@ -I $(INCLUDE) $(CLIBS)
%.shared.o : %.cpp
	$(CC) -c -fPIC -MMD $< -o $@ -I $(INCLUDE) $(CLIBS)
$(LIBOBJECTS) : $(LIBSRC) $(LIBHEADERS)
$(SERVEROBJECTS): $(SERVERSRC) $(SERVERHEADERS)
clean:
	-rm libpwskoag.so
	-rm $(shell find ../src \( -name '*.shared.o' -o -name '*.d' \))

.PHONY: clean
