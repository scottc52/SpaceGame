/*
*
* Actual functionality.
*
*/
#include "Render.h"


//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
	//glViewport(viewport.w/2,viewport.h/2,viewport.w,viewport.h);// sets the rectangle that will be the window
	glViewport(0, 0, w, h);

	//glDepthFunc(GL_LEQUAL);
	//glLineWidth(1);
	//----------- setting the projection -------------------------
	// glOrtho sets left, right, bottom, top, zNear, zFar of the chord system


	// glOrtho(-1, 1 + (w-400)/200.0 , -1 -(h-400)/200.0, 1, 1, -1); // resize type = add
	// glOrtho(-w/400.0, w/400.0, -h/400.0, h/400.0, 1, -1); // resize type = center

	//glOrtho(-1, 1, -1, 1, 1, -1);	// resize type = stretch

	//------------------------------------------------------------
}


//****************************************************
// sets the window up
//****************************************************
void initScene(){
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear to black, fully transparent

	myReshape(640,480);
}


//***************************************************
// function that does the actual drawing
//***************************************************
MyMesh squareMesh(){
	MyMesh mesh;

	// generate vertices

	MyMesh::VertexHandle vhandle[8];

	vhandle[0] = mesh.add_vertex(MyMesh::Point(-1, -1,  1));
	vhandle[1] = mesh.add_vertex(MyMesh::Point( 1, -1,  1));
	vhandle[2] = mesh.add_vertex(MyMesh::Point( 0.5,  0.5,  1));
	vhandle[3] = mesh.add_vertex(MyMesh::Point(-1,  1,  1));
	vhandle[4] = mesh.add_vertex(MyMesh::Point(-1, -1, -1));
	vhandle[5] = mesh.add_vertex(MyMesh::Point( 0.5, -0.5, -1));
	vhandle[6] = mesh.add_vertex(MyMesh::Point( 1,  1, -1));
	vhandle[7] = mesh.add_vertex(MyMesh::Point(-1,  1, -1));


	// generate (quadrilateral) faces

	std::vector<MyMesh::VertexHandle>  face_vhandles;

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[3]);
	mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[7]);
	face_vhandles.push_back(vhandle[6]);
	face_vhandles.push_back(vhandle[5]);
	face_vhandles.push_back(vhandle[4]);
	mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[4]);
	face_vhandles.push_back(vhandle[5]);
	mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[5]);
	face_vhandles.push_back(vhandle[6]);
	mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[6]);
	face_vhandles.push_back(vhandle[7]);
	mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[7]);
	face_vhandles.push_back(vhandle[4]);
	mesh.add_face(face_vhandles);


	return mesh;
}

//function that actually does the drawing
void myDisplay() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();				// loading the identity matrix for the screen

	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	float aspect_ratio = (float) w / (float) h;
	gluPerspective(45.0f, aspect_ratio, 0.01f, 600.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear to black, fully transparent
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);


	glMatrixMode(GL_MODELVIEW);					// indicate we are specifying camera transformations
	glLoadIdentity();
	//gluLookAt(5.0f, 0.0f, -zoomOut, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	float cameraPos[4] = {0,0,4,1};
	Vector3f up = Vector3f(0,1,0);
	Vector3f pan = Vector3f(0,0,0);
	gluLookAt(cameraPos[0]+pan[0], cameraPos[1]+pan[1], cameraPos[2]+pan[2], pan[0], pan[1], pan[2], up[0], up[1], up[2]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// clear the color buffer (sets everything to black)

	//glMatrixMode(GL_MODELVIEW);					// indicate we are specifying camera transformations
	//glLoadIdentity();							// make sure transformation is "zero'd"




	//enabling lighting/ shading
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat ambientlight[] = {0.3,0.3,0.3,1.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientlight);

	//light colors
	GLfloat ambientcolor[] = {0.2, 0.2, 0.2, 1.0};
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientcolor);
	GLfloat diffusecolor[] = {0.1, 0.5, 0.8, 1.0};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffusecolor);
	GLfloat specularcolor[] = {0.8, 0.8, 0.8, 1.0};
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularcolor);

	GLfloat lightposition[] = {300.0, 300.0, 300.0, 1.0};
	glLightfv(GL_LIGHT0, GL_POSITION, lightposition);

	GLfloat lightdirection[] = {-1.0,-1.0,-1.0};
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightdirection);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0);




	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glBindTexture(GL_TEXTURE_2D, textureId);




	//----------------------- code to draw objects --------------------------
	MyMesh mesh = squareMesh();
	glBegin(GL_QUADS);
	glColor3f(.5,.5,.5);
	for (MyMesh::FaceIter it = mesh.faces_begin(); it != mesh.faces_end(); ++it) {

		MyMesh::HalfedgeHandle it2 = mesh.halfedge_handle(it.handle());
		Vec3f p = mesh.point(mesh.to_vertex_handle(it2));
		//shadeByCurvature(mesh.to_vertex_handle(it2));
		glVertex3f(p[0],p[1],p[2]);
		it2 = mesh.next_halfedge_handle(it2);
		p = mesh.point(mesh.to_vertex_handle(it2));
		//shadeByCurvature(mesh.to_vertex_handle(it2));
		glVertex3f(p[0],p[1],p[2]);
		it2 = mesh.next_halfedge_handle(it2);
		p = mesh.point(mesh.to_vertex_handle(it2));
		//shadeByCurvature(mesh.to_vertex_handle(it2));
		glVertex3f(p[0],p[1],p[2]);
		it2 = mesh.next_halfedge_handle(it2);
		p = mesh.point(mesh.to_vertex_handle(it2));
		//shadeByCurvature(mesh.to_vertex_handle(it2));
		glVertex3f(p[0],p[1],p[2]);
	}
	glEnd();
	//-----------------------------------------------------------------------

	glFlush();
	glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}

//****************************************************
// called by glut when there are no messages to handle
//****************************************************
void myIdle() {
	//cout << "myframemove called";
	//nothing here for now
	float dt;
	/* TODO: use glutGet(GLUT_ELAPSED_TIME) instead
	#ifdef _WIN32
	Sleep(10);						//give ~10ms back to OS (so as not to waste the CPU)
	DWORD currentTime = GetTickCount();
	dt = (float)(currentTime - lastTime)*0.001f; 
	lastTime = currentTime;
	#else
	timeval currentTime;
	gettimeofday(&currentTime, NULL);
	dt = (float)((currentTime.tv_sec - lastTime.tv_sec) + 1e-6*(currentTime.tv_usec - lastTime.tv_usec));
	#endif
	*/

	glutPostRedisplay(); // forces glut to call the display function (myDisplay())
}

void RenderGlutInitialize(){
	cout<<"Start SpaceGame!\n";
	//This initializes glut
	

	//This tells glut to use a double-buffered window with red, green, and blue channels 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	//The size and position of the window
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("SpaceGame!");

	initScene();							// quick function to set up scene

	//setup glut callback funtions
	glutDisplayFunc(myDisplay);				// function to run when its time to draw something
	glutReshapeFunc(myReshape);				// function to run when the window gets resized
	glutIdleFunc(myIdle);				// function to run when not handling any other task
	//glutMotionFunc(mouseMoved);
	//glutMouseFunc(mouse);
	//glutKeyboardFunc(keyboard);
	//glutKeyboardUpFunc(keyboardUp);
}