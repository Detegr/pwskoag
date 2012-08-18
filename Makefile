CC=g++ -Os -Wall -Wextra -Werror -Wno-comment
CFLAGS=-I/usr/local/include/GL -I/usr/local/include -L /usr/local/lib
SERVERCFLAGS=-I/usr/local/include -L /usr/local/lib
SERVERLIBS=-lBox2D
LIBS=-lGL -lGLU -lglfw -lXrandr -lBox2D
OSXLIBS=-framework OpenGL -framework Cocoa -lglfw -lBox2D
SOURCES=client.cpp singleton.cpp renderer.cpp inputhandler.cpp shadermanager.cpp filereader.cpp modelmanager.cpp entity.cpp model.cpp gfxtimer.cpp
SERVERSOURCES=server.cpp serversingleton.cpp filereader.cpp modelmanager.cpp model.cpp physicsmanager.cpp physicalentity.cpp timer.cpp
SERVERHEADERS=$(SERVERSOURCES:.cpp=.h)
SERVEROBJECTS=$(SERVERSOURCES:.cpp=.o)
HEADERS=$(SOURCES:.cpp=.h)
OBJECTS=$(SOURCES:.cpp=.o)
SERVEREXE=server
CLIENTEXE=client

all: server client

server: $(SERVEROBJECTS)
	$(CC) $(SERVERCFLAGS) $(SERVEROBJECTS) -o $(SERVEREXE) $(SERVERLIBS)

client: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) gl3w.o -o $(CLIENTEXE) $(LIBS)

osx: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) gl3w.o -o $(FUU) $(OSXLIBS)

%.o : %.cpp %.h
	$(CC) $(CFLAGS) -c -MMD $< -o $@

clean:
	-rm $(EXE)
	mv gl3w.o gl3w.o.bak
	-rm *.o
	-rm *.d
	mv gl3w.o.bak gl3w.o

.PHONY: all clean
