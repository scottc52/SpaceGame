

CPP = g++
LD = g++
OPENMESH_DIR = OpenMesh-2.3.1/src
EIGEN_DIR = SpaceGame/Eigen
CPPFLAGS = -O3 -fPIC -I$(OPENMESH_DIR) -I$(EIGEN_DIR)
LDFLAGS = -O3 -framework GLUT -framework OpenGL -framework Cocoa -l pthread
INCLUDE = -Iinclude/
TARGET = SpaceGame-master

CPP_FILES = $(wildcard SpaceGame/*.cpp)
OBJ_FILES = $(patsubst SpaceGame/%.cpp, objs/%.o, $(CPP_FILES))

default: $(OBJ_FILES)
	$(LD) $(OBJ_FILES) $(LDFLAGS) $(LIB) -o $(TARGET)

objs/%.o: SpaceGame/%.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDE) $< -o $@

clean:
	rm $(OBJ_FILES) SpaceGame-master -f
