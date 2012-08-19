CC=g++ -O0 -g3 -Wall -Wextra -Werror -Wno-comment -std=c++0x
CFLAGS=-I/usr/local/include/GL -I/usr/local/include -L/usr/local/lib -Ldtglib/
SERVERCFLAGS=-I/usr/local/include -L /usr/local/lib -Ldtglib/
SERVERLIBS=-ldtglib -lBox2D -lpthread
LIBS=-ldtglib -lGL -lGLU -lglfw -lXrandr -lBox2D
OSXLIBS=-framework OpenGL -framework Cocoa -lglfw -lBox2D -ldtglib
SOURCES=singleton.cpp renderer.cpp inputhandler.cpp shadermanager.cpp filereader.cpp modelmanager.cpp entity.cpp model.cpp gfxtimer.cpp
MAIN=client.cpp
SERVERMAIN=server.cpp
SERVERSOURCES=serversingleton.cpp filereader.cpp modelmanager.cpp model.cpp physicsmanager.cpp physicalentity.cpp timer.cpp ConnectionManager.cpp
SERVERHEADERS=$(SERVERSOURCES:.cpp=.h)
SERVEROBJECTS=$(SERVERSOURCES:.cpp=.o)
HEADERS=$(SOURCES:.cpp=.h)
OBJECTS=$(SOURCES:.cpp=.o)
SERVEREXE=server
CLIENTEXE=client

all: server client

server: $(SERVERMAIN) $(SERVEROBJECTS)
	$(CC) $(SERVERMAIN) $(SERVERCFLAGS) $(SERVEROBJECTS) -o $(SERVEREXE) $(SERVERLIBS)

client: $(MAIN) $(OBJECTS)
	$(CC) $(MAIN) $(CFLAGS) $(OBJECTS) gl3w.o -o $(CLIENTEXE) $(LIBS)

osx: $(MAIN) $(OBJECTS)
	$(CC) $(MAIN) $(CFLAGS) $(OBJECTS) gl3w.o -o $(CLIENTEXE) $(OSXLIBS)

%.o : %.cpp %.h
	$(CC) $(CFLAGS) -c -MMD $< -o $@

clean:
	-rm $(SERVEREXE) $(CLIENTEXE)
	mv gl3w.o gl3w.o.bak
	-rm *.o
	-rm *.d
	mv gl3w.o.bak gl3w.o

.PHONY: all clean
