#Author: Jdtatum
#Makefile for project 1

FLAGS = -Wall -g 
BIN = project2
#MATH = MMath.a
LIBS = -lglut -lGLU -lGL -lpthread
#ALIBS = $(MATH) $(LIBS)
OBJS = Mesh.o Bitmap.o EdgeMap.o MMath.o CMSpline.o Animation.o TaskQueue.o
H_S = Mesh.h Bitmap.h EdgeMap.h CMSpline.h MMath.h MMath.inl CMSpline.h Animation.h TaskQueue.h

$(BIN): main.cpp $(OBJS) $(H_S) 
	g++ -o $(BIN) main.cpp $(OBJS) $(LIBS) $(FLAGS)

%.o: %.cpp $(H_S)
	g++ -o $@ -c $< $(LIBS) $(FLAGS)

TEST: test.cpp MMath.o CMSpline.o TaskQueue.o $(H_S)
	g++ -o $@ test.cpp MMath.o CMSpline.o TaskQueue.o $(LIBS) $(FLAGS)

.PHONY: clean
clean:
	rm -f *.o $(BIN) TEST

