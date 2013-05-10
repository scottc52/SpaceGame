

CPP = g++
LD = g++
OPENMESH_DIR = OpenMesh-2.3.1/src
CPPFLAGS = -O3 -fPIC -I$(OPENMESH_DIR)
LDFLAGS = -O3 -framework GLUT -framework OpenGL -framework Cocoa -l pthread
INCLUDE = -Iinclude/
TARGET = SpaceGame-master

CPP_FILES = $(wildcard SpaceGame/*.cpp)
OBJ_FILES = $(patsubst SpaceGame/%.cpp, objs/%.o, $(CPP_FILES))

default: $(OBJ_FILES)
	$(LD) $(OBJ_FILES) $(LDFLAGS) $(LIB) -o $(TARGET)

objs/main.o: SpaceGame/main.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDE) SpaceGame/main.cpp -o objs/main.o

objs/GameRoom.o: SpaceGame/GameRoom.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDE) SpaceGame/GameRoom.cpp -o objs/GameRoom.o

clean:
	rm $(OBJ_FILES) SpaceGame-master -f
