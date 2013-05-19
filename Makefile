

CPP = g++
LD = g++
OPENMESH_DIR = OpenMesh-2.3.1/src
OPENMESH_LIB_DIR = OpenMesh-2.3.1/build/Build/lib/OpenMesh
EIGEN_DIR = SpaceGame/Eigen
GLEW_DIR = SpaceGame/glew-1.9.0/include/GL
GLEW_LIB_DIR = SpaceGame/glew-1.9.0/lib
CPPFLAGS = -O3 -fPIC -I$(OPENMESH_DIR) -I$(EIGEN_DIR) -I$(GLEW_DIR)
LDFLAGS = -O3 -framework GLUT -framework OpenGL -framework Cocoa -l pthread
LIB = -lOpenMeshCored -lOpenMeshToolsd -lGLEW -L$(OPENMESH_LIB_DIR) -L$(GLEW_LIB_DIR)
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
