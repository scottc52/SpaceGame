/*
* Author Jon Tatum
* File: Main.cpp
* 
*/
#include <iostream>
#include <string>
#include <vector>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include "main.h"
#include "Mesh.h"
#include "Bitmap.h"
#include "Animation.h"
#include <ctime>
#include "TaskQueue.h"

using namespace std;

struct window{
	int width;
	int height;
	const char *title;
	int mesh;
	vector<Mesh *> meshes;
	GLfloat z;
	GLfloat y;
	GLfloat x;
	GLfloat theta;
	GLfloat phi;
	GLfloat wrot;
	GLfloat yrot;  
	GLfloat scale;
	GLuint texture;
	int lighting;
	int mat;
	clock_t stime;
	Animation *an;   
	bool tex, norm, clear;
	TaskQueue *taskQueue;     
} win; 


inline unsigned long millisPassed(clock_t c){
	return (unsigned long)(((long double) clock() - c) / ((long double )CLOCKS_PER_SEC)*1000.0);
}

Mesh *curMesh(){
	return win.meshes[win.mesh];
}

void nextMesh(){
	unsigned int mesh = (unsigned int) win.mesh + 1;
	if (mesh == win.meshes.size()){
		cout << "subdivide!" << endl;
		Mesh *next = win.meshes[win.mesh]->parallelSubdivide(win.taskQueue, 8);
		next->computeNormals();
		cout << "Done!" << endl;
		win.meshes.push_back(next); 
	}
	win.mesh = mesh;
}

void prevMesh(){
	win.mesh = (win.mesh > 0)? win.mesh -1 : 0;	
}

GLuint loadTexture(const char *fname){
	Bitmap *image = Bitmap::loadBitmap(fname); 
	if (!image){
		return 0;
	}
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture( GL_TEXTURE_2D, texture );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
        GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
        GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	delete image; 
	return texture; 
}

void transform(){
	
	glTranslatef(win.x * 0.2, win.y * 0.2, 2.0 + win.z *0.2);
	glRotatef(5*win.phi, 0, 1.0, 0);
	glRotatef(5*win.theta, 1.0, 0, 0);
	GLfloat s = 0.1 * win.scale; 
	glScalef(s,s,s);
}

void light(GLenum l, GLfloat *m){ 
	glLightfv(l, GL_AMBIENT, &m[0]);
	glLightfv(l, GL_DIFFUSE, &m[4]);
	glLightfv(l, GL_SPECULAR, &m[8]);
	glLightfv(l, GL_POSITION, &m[12]);
}
#define vec(a) (&a[0][0])  
void setUpLights(){
	glEnable(GL_LIGHTING);
	
	//GLfloat ambientlight[] = {0.3, 0.3 , 0.3, 1.0};
	///glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientlight);
	

	GLfloat l1[][4] = {
		{0.2, 0.2, 0.2, 1.0},
		{0.8, 0.8, 0.8, 1.},
		{0.1, 0.1, 0.1, 1.},
		{1, 1, 0, 0}
	};
	light(GL_LIGHT0, vec(l1)); 
	glEnable(GL_LIGHT0);
	
	GLfloat l2[][4] = {
		{0.2, 0.2, 0.2, 1.},
		{1, 1, 1, 1.},
		{1., 1., 1., 1.},
		{-30, -5, 20, 1}
	};
	light(GL_LIGHT1, vec(l2));
	GLfloat dir[] = {1, -1., 0};

	glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,30.0);

    glLightf(GL_LIGHT1,GL_SPOT_EXPONENT,3.0);

    glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION, &dir[0]);


    glEnable(GL_LIGHT1);

	GLfloat l3[][4] = {
		{0.2, 0.2, 0.2, 1.},
		{1, 1, 1 , 1.},
		{1., 1., 1., 1.},
		{0, 10, 20, 1}
	};
	light(GL_LIGHT2, vec(l3));
	GLfloat dir2[] = {0, -1, -0.5};
	glLightf(GL_LIGHT2,GL_SPOT_CUTOFF,20.0);

    //glLightf(GL_LIGHT1,GL_SPOT_EXPONENT,3.0);

    glLightfv(GL_LIGHT2,GL_SPOT_DIRECTION, &dir2[0]);

    glEnable(GL_LIGHT2);

    //glEnable(GL_COLOR_MATERIAL);
}

void init(){
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(win.width, win.height);
	glutInitWindowPosition(500,500);
	glutCreateWindow("Project 1");
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glViewport(0,0,win.width,win.height);
	
	GLfloat aspect = (GLfloat)win.width / (GLfloat)win.height;
	gluPerspective(75.0f,  aspect,1.0f,30.0f);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0.0f,0.0f,-1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);
	setUpLights();
	glEnable (GL_DEPTH_TEST);
	win.taskQueue = new TaskQueue(8);
}

void switchLights(){
	if (win.lighting == 1){
		win.lighting = 0; 
		glShadeModel(GL_FLAT);
		cout<<"flat now"<<endl;
	}else{
		win.lighting = 1;
		glShadeModel(GL_SMOOTH);
		cout <<"smooth now" << endl;
	}

}

void keyevent(unsigned char key, int xpos, int ypos){
	switch (key){
		case 'a': win.x += 1; break;  
		case 'd': win.x += -1; break;
		case 'w': win.y += 1; break;
		case 's': win.y += -1; break;
		case 'q': win.z += 1; break;
		case 'e': win.z += -1; break; 
		case 'z': win.theta += -1; break;
		case 'x': win.theta += 1; break;
		case 'c':win.phi += -1; break;
		case 'v': win.phi += 1; break;
		case 'r': win.scale += 1; break;
		case 'f': win.scale += -1; break;
		case 'o': nextMesh(); break;
		case 'l': prevMesh(); break;
		case 'm': switchLights(); break;
		case 'n': win.mat++;win.mat %= 3; break;
		case 'b': win.stime = clock(); break;
		case 't': win.clear = !win.clear; break;
		case 'u': win.wrot+=1; break;

	}
}

void Material(int i){
	if (i == 0){
		GLfloat ac[] = {0.1, 0.3, 0.1, 1} ;
		GLfloat dc[] = {0.1, 0.6, 0.1, 1};
		GLfloat sc[] = {1, 1, 1, 1};

		glMaterialfv(GL_FRONT, GL_AMBIENT, &ac[0]);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, &dc[0]);
		glMaterialfv(GL_FRONT, GL_SPECULAR, &sc[0]);
		glMaterialf(GL_FRONT, GL_SHININESS, 100.f);
	}else if(i == 1){
		GLfloat ac[] = {0.0, 0.0, 0.1, 0.9};
		GLfloat dc[] = {0.5, 0, 1, 0.9};
		GLfloat sc[] = {0.1, 0.1, 0.1, 1};

		glMaterialfv(GL_FRONT, GL_AMBIENT, &ac[0]);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, &dc[0]);
		glMaterialfv(GL_FRONT, GL_SPECULAR, &sc[0]);
		glMaterialf(GL_FRONT, GL_SHININESS, 100.f);
	}else if(i == 2){
		GLfloat ac[] = {0.1, 0.0, 0.0, 1} ;
		GLfloat dc[] = {0.6, 0.1, 0.1, 1};
		GLfloat sc[] = {1, 1, 1, 1};

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &ac[0]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &dc[0]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &sc[0]);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.f);
	}
}

void lighting(){
	
	
	setUpLights(); 
	
	Material(win.mat); 
}
void drawPlane(){
	
	glBegin(GL_QUADS);
		Material(1);

		glNormal3f(0,1.,0);
		GLfloat xstep = 80./100.;
		GLfloat zstep = 39./100.;
		
		GLfloat y = -3;
		GLfloat z = 1;  
						
		for (int i = 0; i< 100; i++){
			GLfloat x= -40;
			for (int j = 0; j < 100; j ++){
				glNormal3f(0,1.,0);
				glVertex3f(x, y, z);
				glNormal3f(0,1.,0);
				glVertex3f(x, y, z+zstep);
				glNormal3f(0,1.,0);
				glVertex3f(x+xstep, y, z+zstep);
				glNormal3f(0,1.,0);
				glVertex3f(x+xstep, y, z);
				x+=xstep;
			}
			z+=zstep;
		}
		Material(win.mat);
	glEnd();
}

void animate(int val){
	//long delta = millisPassed(win.stime);	
	glutPostRedisplay(); 	
}

int X = -1, Y = -1; 
void mouseClick(int button, int state, int x, int y ){
	if (state == GLUT_UP)
		X=Y=-1; 
	else{
		X = x;
		Y= y;
	}
} 

void mouseMove(int x, int y){
	if (X == -1 )
		return; 
	int dx = x - X;
	int dy = y - Y;
	X = x;
	Y = y; 
	win.wrot += ((float)dx) * 0.5;
	win.yrot += ((float)dy) *0.5;    
}

void display(){
	if (win.clear){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
	}
	//glColor4f(0.1f, 0.2f, 0.6f, 0.1f);
	unsigned long f = millisPassed(win.stime); 
	glPushMatrix(); 
	glTranslatef(0, 0, 15.0);
	glRotatef(win.yrot*1, 1.0, 0, 0);
	glRotatef(win.wrot*5, 0, 1.0, 0 );

	glTranslatef(0, 0, -15.0);
	lighting();
	glPushMatrix();
	transform();
	
	if (win.an){
		win.an->drawFrame(f);
	}else{ 

	if (win.tex){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, win.texture);
		curMesh()->draw(win.lighting);
		glDisable(GL_TEXTURE_2D);
	} else{
		glColor4f(0, 0, 0, 1.0f);
		//curMesh()->drawWireframe();
		glColor4f(0.5f, 0.3f, 0.7f, 0.5f);
		curMesh()->draw(win.lighting);
	}}
	glPopMatrix(); 
	drawPlane();
	
	glPopMatrix();
	glutSwapBuffers(); 
	unsigned long f2 = millisPassed(win.stime)-f;  
	unsigned long wait = 2 * mSPF60; 
	wait = (wait > f2)? wait - f2: 1;
	glutTimerFunc(wait, animate, 0);
}	

void idle(){ 

}


bool contains(char *s, const string &c){
	string tmp(s); 
	int p = (int) tmp.find(c); 
	if (p < (int)tmp.length() && p >= 0)
		return true;
	else
		return false;
}

int main(int argc,char **argv){
	if (argc < 2){
		cerr << "Error: no object file specified" << endl;  
		return(-1);  
	}
	char *fname = argv[1];
	Animation *a = NULL;
	Mesh *m = NULL;;  
	if (contains(fname, ".an")){
		a = Animation::loadFile(fname);
		if (a == NULL){
			cerr << "error could not load specified '.an' file" << endl;
			return -1;
		} 
		m = a->obj; 
	}else if (contains(fname, ".obj")){ 
		m = Mesh::loadObj(fname);   
	}else {
		cerr << "Error unknown file type. cannot display:" << fname << endl; 
		return -1;
	}

	//set params for the main execution loop
	win.height = 600; 
	win.width = 800;
	win.title = "Project1";
	win.mesh = 0;
	win.meshes.push_back(m);
	win.x = 0;
	win.y = 0;
	win.z = 0;
	win.theta = 0;
	win.phi = 0;  
	win.scale = 2;  
	win.lighting = 0;
	win.clear = true;
	win.wrot = 0;
	win.yrot = 0; 
	if(a){
		a->obj->computeDeltas(2.0f);
		win.an = a; 
	}

	win.mat = 0; 

	glutInit(&argc, argv);
	init();
	//try to load textures
	if (argc >= 3){
		char *tfname = argv[2];
		win.texture = loadTexture(tfname);
		win.tex = true;
		win.norm = true; 
	}else{
		cout << "no tex file specified" << endl; 
		win.tex = win.norm = false;
		win.meshes[0]->computeNormals();  
	}
	glutDisplayFunc(display);
	glutKeyboardFunc(keyevent);
	glutTimerFunc(mSPF60, animate, 0);
	glutMotionFunc(mouseMove);
	glutMouseFunc(mouseClick);  
	glutMainLoop(); 
}
