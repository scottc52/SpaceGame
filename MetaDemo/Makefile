

CPP = g++
LD = g++
OPENMESH_DIR = OpenMesh-2.3.1/src
OPENMESH_LIB_DIR = OpenMesh-2.3.1/build/Build/lib/OpenMesh
EIGEN_DIR = Eigen

CPPFLAGS = -O3 -fPIC -I$(OPENMESH_DIR) -I$(EIGEN_DIR)
LDFLAGS = -O3  -Wall -framework GLUT -framework OpenGL -framework Cocoa -l pthread -framework CoreFoundation 
LIB = -lOpenMeshCored -lOpenMeshToolsd -L$(OPENMESH_LIB_DIR)
INCLUDE = -Iinclude/
TARGET = MetaDemo

CPP_FILES = $(wildcard src/*.cpp)
OBJ_FILES = $(patsubst src/%.cpp, objs/%.o, $(CPP_FILES))

default: $(OBJ_FILES)
	$(LD) $(OBJ_FILES) $(LDFLAGS) $(LIB) -o $(TARGET)

objs/%.o: src/%.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDE) $< -o $@

clean:
	rm $(OBJ_FILES) MetaDemo -f
