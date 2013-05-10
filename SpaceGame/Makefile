# GNU make file
# Author: Jon T

libs = -lglut -lGLU -lGL -lpthread
incl = -I ../OpenMesh-2.3.1/src/ -I ./glew-1.9.0/include/ -I ./Eigen/
flags = -Wall -g -Wwrite-strings -fpermissive
opts = $(incl) $(libs) $(flags)
openmeshdir = ../OpenMesh-2.3.1/ 
openmesh = ../OpenMesh-2.3.1/build/Build/lib/OpenMesh/libOpenMeshCored.a
#modules that can be compiled into intermediary binaries individualy (ie not a static library or an executable)
objs = GameRoom.o
hdrs = $(wildcard *.h)  

main: main.cpp $(objs) $(hdrs) 
	g++ -o $@ $< $(objs) $(openmesh) $(opts)

room_test: room_test.cpp $(objs) $(hdrs)  
	g++ -o $@ $< $(objs) $(opts) 

%.o: %.cpp $(hdrs)
	g++ -o $@ -c $< $(opts)

$(openmesh):
	cd $(openmeshdir) && rm -rf ./build && cmake .. && make; 

.PHONY: clean

clean: 
	rm -rf *.o main room_test 

