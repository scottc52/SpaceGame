

CPP = g++
LD = g++
EIGEN_DIR = Eigen

CPPFLAGS = -O3 -fPIC -I$(EIGEN_DIR)
LDFLAGS = -O3  -Wall -framework GLUT -framework OpenGL -framework Cocoa -l pthread -framework CoreFoundation
INCLUDE = -Iinclude/
TARGET = MetaDemo

CPP_FILES = $(wildcard src/*.cpp)
OBJ_FILES = $(patsubst src/%.cpp, objs/%.o, $(CPP_FILES))

default: $(OBJ_FILES)
	$(LD) $(OBJ_FILES) $(LDFLAGS) -o $(TARGET)

objs/%.o: src/%.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDE) $< -o $@

clean:
	rm $(OBJ_FILES) MetaDemo -f
