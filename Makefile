

CPP = g++
LD = g++
OPENMESH_DIR = OpenMesh-2.3.1/src
OPENMESH_LIB_DIR = OpenMesh-2.3.1/build/Build/lib/OpenMesh
EIGEN_DIR = SpaceGame/Eigen

GLEW_DIR = SpaceGame/glew-1.9.0/include/GL
GLEW_LIB_DIR = SpaceGame/glew-1.9.0/lib
SDL_LIB_DIR = SpaceGame/SDL-1.2.15/include
SDL_MIXER_LIB_DIR = SpaceGame/SDL_mixer-1.2.12/include
SOIL_DIR = SpaceGame/SOIL/include
SOIL_LIB_DIR = SpaceGame/SOIL/

CPPFLAGS = -O3 -fPIC -I$(OPENMESH_DIR) -I$(EIGEN_DIR) -I$(GLEW_DIR) -I$(SDL_LIB_DIR) -I$(SDL_MIXER_LIB_DIR) 

LDFLAGS = -O3  -Wall -framework GLUT -framework OpenGL -framework Cocoa -l pthread -framework CoreFoundation 

LIB = -lOpenMeshCored -lOpenMeshToolsd -lGLEW -L$(OPENMESH_LIB_DIR) -L$(GLEW_LIB_DIR) -L$(SDL_LIB_DIR) -L$(SDL_MIXER_LIB_DIR) 

INCLUDE = -Iinclude/ 

GLEW_DIR = SpaceGame/glew-1.9.0/include/
GLEW_LIB_DIR = SpaceGame/glew-1.9.0/lib
SOIL_DIR = SpaceGame/SOIL/include
SOIL_LIB_DIR = SpaceGame/SOIL/lib
CPPFLAGS = -O3 -fPIC -I$(OPENMESH_DIR) -I$(EIGEN_DIR) -I$(GLEW_DIR) -I$(SOIL_DIR)
LDFLAGS = -O3 -framework GLUT -framework OpenGL -framework Cocoa -l pthread
LIB = -lOpenMeshCored -lOpenMeshToolsd -lGLEW -L$(OPENMESH_LIB_DIR) -L$(GLEW_LIB_DIR) -L$(SOIL_LIB_DIR)
INCLUDE = -Iinclude/
TARGET = SpaceGame-master

CPP_FILES = $(wildcard SpaceGame/*.cpp)
OBJ_FILES = $(patsubst SpaceGame/%.cpp, objs/%.o, $(CPP_FILES)) $(SOIL_LIB_DIR)libSOIL.a  

default: $(OBJ_FILES)
	$(LD) $(OBJ_FILES) $(LDFLAGS) $(LIB) -o $(TARGET)

objs/%.o: SpaceGame/%.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDE) $< -o $@

clean:
	rm $(OBJ_FILES) SpaceGame-master -f
