# GNU make file
# Author: Jon T

libs = -lglut -lGLU -lGL -lpthread
incl = -I ../OpenMesh-2.3.1/src/
flags = -Wall -g -Wwrite-strings -fpermissive
opts = $(incl) $(libs) $(flags)
#modules that can be compiled into intermediary binaries individualy (ie not a static library or an executable)
objs = GameRoom.o
hdrs = $(wildcard *.h)  

main: main.cpp $(objs) $(hdrs) 
	g++ -o $@ $(objs) $(opts)

room_test: room_test.cpp $(objs) $(hdrs)  
	g++ -o $@ $< $(objs) $(opts) 

%.o: %.cpp $(hdrs)
	g++ -o $@ -c $< $(opts)

.PHONY: clean

clean: 
	rm -rf *.o main room_test 

