#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <Eigen/Dense>
using namespace Eigen;

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "MetaballEnemy.h"
#include "MetalineEnemy.h"
#include "MetaplaneEnemy.h"

/* Constants */
const int FLAT_SHADING = 0;
const int SMOOTH_SHADING = 1;

const bool MOVING_ENABLED = false;
const float BLOB_CREATURE_STATIONARY_BOUNDARY = 1.f;
const float BLOB_CREATURE_MOVE_BOUNDARY = 4.f;

/* Global Variables */
AI *bc;

static GLfloat ambientLight[] = {0.1, 0.1, 0.1, 1.0};
//static GLfloat ambientLight[] = {1.00, 1.00, 1.00, 1.0};
static GLfloat diffuseLight[]  = {1.00, 1.00, 1.00, 1.0};
static GLfloat specularLight[] = {1.00, 1.00, 1.00, 1.0};

/*float materialAmbient[] = {0.2, 0.2, 0.6, 1.0};
float materialDiffuse[]  = {0.2, 0.2, 0.6, 1.0};
float materialSpecular[] = {0.8, 0.8, 0.8, 1.0};
float shininess = 8.0;*/
static float spotPosition[] = {10.0, 10.0, 10.0, 1.0};
static float spotDirection[] = {-10.0, -10.0, -10.0, 1.0};
static int spotAngle = 50;

int shadingMode;
bool lighting;

GLuint textureID;

/* Inline Functions */
inline float max (float a, float b) {
	return a > b ? a : b;
}

/* Function Prototypes */
void init();
bool loadTexture(const char *filename);

int win_width = 512;
int win_height = 512;

// Stored mouse position for camera rotation, panning, and zoom.
int gPreviousMouseX = -1;
int gPreviousMouseY = -1;
int gMouseButton = -1;
Vector3f mCameraTranslation;
float mCameraAzimuth;
float mCameraElevation;

void resetCamera()
{
	mCameraTranslation = Vector3f(0.f, 0.f, 0.f);
    mCameraAzimuth = 0.f;
    mCameraElevation = 65.0f;
}

/**
 * Camera adjustment methods
 */
void AdjustCameraAzimuthBy(float delta)
{
    mCameraAzimuth += delta;
}

void AdjustCameraElevationBy(float delta)
{
    mCameraElevation += delta;
}

void AdjustCameraTranslationBy(Vector3f delta)
{
    mCameraTranslation += delta;
}

void display( void )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	/*float maxRange = max(maxX - minX, maxY - minY);
	float centerX = (maxX + minX) / 2;
	float centerY = (maxY + minY) / 2;
	
	glOrtho(centerX - (maxRange / 2) - MIN_DISTANCE_TO_WINDOW_EDGE,
			centerX + (maxRange / 2) + MIN_DISTANCE_TO_WINDOW_EDGE,
			centerY - (maxRange / 2) - MIN_DISTANCE_TO_WINDOW_EDGE,
			centerY + (maxRange / 2) + MIN_DISTANCE_TO_WINDOW_EDGE, -40.0, 40.0);*/
	
	if (MOVING_ENABLED)
	{
		glOrtho(-BLOB_CREATURE_MOVE_BOUNDARY, BLOB_CREATURE_MOVE_BOUNDARY,
				-BLOB_CREATURE_MOVE_BOUNDARY, BLOB_CREATURE_MOVE_BOUNDARY,
				-BLOB_CREATURE_MOVE_BOUNDARY, BLOB_CREATURE_MOVE_BOUNDARY);
	}
	else
	{
		glOrtho(-BLOB_CREATURE_STATIONARY_BOUNDARY, BLOB_CREATURE_STATIONARY_BOUNDARY,
				-BLOB_CREATURE_STATIONARY_BOUNDARY, BLOB_CREATURE_STATIONARY_BOUNDARY,
				-BLOB_CREATURE_STATIONARY_BOUNDARY, BLOB_CREATURE_STATIONARY_BOUNDARY);
	}
	
	glViewport(0, 0, win_width, win_height);
	
    Vector3f trans = -mCameraTranslation;
    
    glTranslatef(trans(0), trans(1), trans(2));
    
    glRotatef(-mCameraElevation, 1, 0, 0);
    glRotatef(-mCameraAzimuth, 0, 1, 0);
    
    glRotatef(-90.0f, 1, 0, 0);
    glScalef(1.0, -1.0, 1.0);
	
	glColor4f(0.5, 0.5, 0.5, 1.0);
	
	bc->update();
	
	bc->render();
	
	/*Blob *blobs = bc->getBlobs();
	
	int numBlobs = bc->getNumBlobs();
	for (int i = 0; i < numBlobs; i++) {
		Blob curBlob = blobs[i];
		glPushMatrix();
		glTranslatef(curBlob.center.x + bc->getCenter().x,
					 curBlob.center.y + bc->getCenter().y,
					 curBlob.center.z + bc->getCenter().z);
		glutSolidSphere(sqrt(curBlob.radiusSquared), 50, 50);
		glPopMatrix();
	}*/
	
	/*glColor4f(1.f, 1.f, 1.f, 1.f);
	
	glBegin(GL_QUADS);
	
	glVertex3f(-10.f, -10.f, -10.f);
	glVertex3f(10.f, -10.f, -10.f);
	glVertex3f(10.f, -10.f, 10.f);
	glVertex3f(-10.f, -10.f, 10.f);
	
	glVertex3f(-10.f, -10.f, -10.f);
	glVertex3f(-10.f, 10.f, -10.f);
	glVertex3f(-10.f, 10.f, 10.f);
	glVertex3f(-10.f, -10.f, 10.f);
	
	glVertex3f(10.f, -10.f, -10.f);
	glVertex3f(10.f, 10.f, -10.f);
	glVertex3f(10.f, 10.f, 10.f);
	glVertex3f(10.f, -10.f, 10.f);
	
	glVertex3f(-10.f, 10.f, -10.f);
	glVertex3f(10.f, 10.f, -10.f);
	glVertex3f(10.f, 10.f, 10.f);
	glVertex3f(-10.f, 10.f, 10.f);
	
	glVertex3f(-10.f, -10.f, -10.f);
	glVertex3f(10.f, -10.f, -10.f);
	glVertex3f(10.f, 10.f, -10.f);
	glVertex3f(-10.f, 10.f, -10.f);
	
	glVertex3f(-10.f, -10.f, 10.f);
	glVertex3f(10.f, -10.f, 10.f);
	glVertex3f(10.f, 10.f, 10.f);
	glVertex3f(-10.f, 10.f, 10.f)*
	
	glEnd();*/
	
	glutSwapBuffers();
}

void reshape( int w, int h )
{	
	win_width = w;
	win_height = h;
	
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	
	/*float maxRange = max(maxX - minX, maxY - minY);
	float centerX = (maxX + minX) / 2;
	float centerY = (maxY + minY) / 2;
	
	glOrtho(centerX - (maxRange / 2) - MIN_DISTANCE_TO_WINDOW_EDGE,
			centerX + (maxRange / 2) + MIN_DISTANCE_TO_WINDOW_EDGE,
			centerY - (maxRange / 2) - MIN_DISTANCE_TO_WINDOW_EDGE,
			centerY + (maxRange / 2) + MIN_DISTANCE_TO_WINDOW_EDGE,
			minZ - MIN_DISTANCE_TO_WINDOW_EDGE, maxZ + MIN_DISTANCE_TO_WINDOW_EDGE);*/
	if (MOVING_ENABLED)
	{
		glOrtho(-BLOB_CREATURE_MOVE_BOUNDARY, BLOB_CREATURE_MOVE_BOUNDARY,
				-BLOB_CREATURE_MOVE_BOUNDARY, BLOB_CREATURE_MOVE_BOUNDARY,
				-BLOB_CREATURE_MOVE_BOUNDARY, BLOB_CREATURE_MOVE_BOUNDARY);
	}
	else
	{
		glOrtho(-BLOB_CREATURE_STATIONARY_BOUNDARY, BLOB_CREATURE_STATIONARY_BOUNDARY,
				-BLOB_CREATURE_STATIONARY_BOUNDARY, BLOB_CREATURE_STATIONARY_BOUNDARY,
				-BLOB_CREATURE_STATIONARY_BOUNDARY, BLOB_CREATURE_STATIONARY_BOUNDARY);
	}
	glViewport(0, 0, win_width, win_height);
}

void specialKeyboard(int key, int x, int y)
{
    switch(key) {
        case GLUT_KEY_LEFT:
            AdjustCameraTranslationBy(Vector3f(-0.05,0,0));
            break;
        case GLUT_KEY_RIGHT:
            AdjustCameraTranslationBy(Vector3f(0.05,0,0));
            break;
        case GLUT_KEY_DOWN:
            AdjustCameraTranslationBy(Vector3f(0,-0.05,0));
            break;
        case GLUT_KEY_UP:
            AdjustCameraTranslationBy(Vector3f(0,0.05,0));
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key) {
		case 'l':
			if (lighting) {
				glDisable(GL_LIGHTING);
				lighting = false;
			} else {
				glEnable(GL_LIGHTING);
				lighting = true;
			}
			break;
		case 'o':
			glShadeModel(GL_FLAT);
			shadingMode = FLAT_SHADING;
			break;
		case 'p':
			glShadeModel(GL_SMOOTH);
			shadingMode = SMOOTH_SHADING;
			break;
		case 'r':
			resetCamera();
			break;
		case 'q':
			delete bc;
			exit(0);
		case 27: // Escape key
			delete bc;
			exit(0);
		case '1':
			delete bc;
			bc = new MetaballEnemy(Vector3f(0.f, 0.f, 0.f));
			break;
		case '2':
			delete bc;
			bc = new MetalineEnemy(Vector3f(0.f, 0.f, 0.f));
			break;
		case '3':
			delete bc;
			bc = new MetaplaneEnemy(Vector3f(0.f, 0.f, 0.f));
			break;
		default:
			break;
    }
	
    // glutPostRedisplay();
}


/**
 * Mouse event handler
 */
void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON || button == GLUT_RIGHT_BUTTON)
    {
        gMouseButton = button;
    } else
    {
        gMouseButton = -1;
    }
    
    if (state == GLUT_UP)
    {
        gPreviousMouseX = -1;
        gPreviousMouseY = -1;
    }
}

/**
 * Mouse active motion callback (when button is pressed)
 /*/
void motion(int x, int y)
{
    if (gPreviousMouseX >= 0 && gPreviousMouseY >= 0)
    {
        //compute delta
        float deltaX = x-gPreviousMouseX;
        float deltaY = y-gPreviousMouseY;
        gPreviousMouseX = x;
        gPreviousMouseY = y;
        
        float zoomSensitivity = 0.2f;
        float rotateSensitivity = 0.5f;
        
        //orbit or zoom
        if (gMouseButton == GLUT_LEFT_BUTTON)
        {
            AdjustCameraAzimuthBy(-deltaX*rotateSensitivity);
            AdjustCameraElevationBy(-deltaY*rotateSensitivity);
            
        } else if (gMouseButton == GLUT_RIGHT_BUTTON)
        {
            Vector3f zoom(0,0,deltaX);
            AdjustCameraTranslationBy(zoom * zoomSensitivity);
        }
        
    } else
    {
        gPreviousMouseX = x;
        gPreviousMouseY = y;
    }
    
}

int main (int argc, char *argv[]) {
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(win_width, win_height);
	
	glutCreateWindow("Assignment 2");
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(specialKeyboard);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutIdleFunc(display);
	
	if (argc == 1/*argc == 2 || argc == 3*/) {
		/*texture = false;
		if (argc == 3) {
			glEnable(GL_TEXTURE_2D);
			loadTexture(argv[2]);
			texture = true;
		}*/
		init();
	} else {
		cout << "Improper number of arguments." << endl;
		exit(0);
	}
	
	glutMainLoop();
}

void init(void) {
	glClearColor(0.9, 0.9, 0.9, 1.0);
	
	/*float maxRange = max(maxX - minX, maxY - minY);
	float centerX = (maxX + minX) / 2;
	float centerY = (maxY + minY) / 2;
	
	glOrtho(centerX - (maxRange / 2) - MIN_DISTANCE_TO_WINDOW_EDGE,
			centerX + (maxRange / 2) + MIN_DISTANCE_TO_WINDOW_EDGE,
			centerY - (maxRange / 2) - MIN_DISTANCE_TO_WINDOW_EDGE,
			centerY + (maxRange / 2) + MIN_DISTANCE_TO_WINDOW_EDGE,
			minZ - MIN_DISTANCE_TO_WINDOW_EDGE, maxZ + MIN_DISTANCE_TO_WINDOW_EDGE);*/
	if (MOVING_ENABLED)
	{
		glOrtho(-BLOB_CREATURE_MOVE_BOUNDARY, BLOB_CREATURE_MOVE_BOUNDARY,
				-BLOB_CREATURE_MOVE_BOUNDARY, BLOB_CREATURE_MOVE_BOUNDARY,
				-BLOB_CREATURE_MOVE_BOUNDARY, BLOB_CREATURE_MOVE_BOUNDARY);
	}
	else
	{
		glOrtho(-BLOB_CREATURE_STATIONARY_BOUNDARY, BLOB_CREATURE_STATIONARY_BOUNDARY,
				-BLOB_CREATURE_STATIONARY_BOUNDARY, BLOB_CREATURE_STATIONARY_BOUNDARY,
				-BLOB_CREATURE_STATIONARY_BOUNDARY, BLOB_CREATURE_STATIONARY_BOUNDARY);
	}
	glViewport(0, 0, win_width, win_height);
	
	resetCamera();
	
	glShadeModel(GL_FLAT);
	shadingMode = FLAT_SHADING;
	
	glEnable(GL_LIGHTING);
	lighting = true;
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	/*glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);*/
	glLightfv(GL_LIGHT0, GL_POSITION, spotPosition);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDirection);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spotAngle);
	
	glEnable(GL_DEPTH_TEST);
	
	//glEnable(GL_NORMALIZE);
	glEnable(GL_RESCALE_NORMAL);
	
	bc = new MetaballEnemy(Vector3f(0.f, 0.f, 0.f));
}

bool loadTexture(const char *filename) {
	FILE *in;
	in = fopen(filename, "rb");
	
	if (in == NULL) {
		printf("Could not open bitmap file.");
		return false;
	}
	
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, in);
	
	int width = *(int *) &info[18];
	int height = *(int *) &info[22];
	
	int size = 4 * width * height;
	unsigned char *data = new unsigned char[size];
	fread(data, sizeof(unsigned char), size, in);
	
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
				 GL_RGB, GL_UNSIGNED_BYTE, data);
	
	delete data;
	
	return true;
}