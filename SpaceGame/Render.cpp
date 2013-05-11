/*
*
* Actual functionality.
*
*/
#include "Render.h"

//****************************************************
// classes
//****************************************************
class Material{
public:
	float Ns;
	float Tr;
	float Ka[3];
	float Kd[3];
	float Ks[3];
	float Ke[3];
};
Material exampleMaterial(){
	Material material;
	material.Ns = 10.0;
	material.Tr = 0.0;
	material.Ka[0] = 0.588; material.Ka[1] = 0.588; material.Ka[2] = 0.588;
	material.Kd[0] = 0.588; material.Kd[1] = 0.588; material.Kd[2] = 0.588;
	material.Ks[0] = 0.000; material.Ks[1] = 0.000; material.Ks[2] = 0.000; 
	material.Ke[0] = 0.000; material.Ke[1] = 0.000; material.Ke[2] = 0.000;

	return material;
}

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
	vhandle[2] = mesh.add_vertex(MyMesh::Point( 1,  1,  1));
	vhandle[3] = mesh.add_vertex(MyMesh::Point(-1,  1,  1));
	vhandle[4] = mesh.add_vertex(MyMesh::Point(-1, -1, -1));
	vhandle[5] = mesh.add_vertex(MyMesh::Point( 1, -1, -1));
	vhandle[6] = mesh.add_vertex(MyMesh::Point( 1,  1, -1));
	vhandle[7] = mesh.add_vertex(MyMesh::Point(-1,  1, -1));


	// generate (quadrilateral) faces

	std::vector<MyMesh::VertexHandle>  face_vhandles;

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[2]);
	//face_vhandles.push_back(vhandle[3]);
	mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[0]);
	//face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[3]);
	mesh.add_face(face_vhandles);


	face_vhandles.clear();
	face_vhandles.push_back(vhandle[7]);
	face_vhandles.push_back(vhandle[6]);
	face_vhandles.push_back(vhandle[5]);
	//face_vhandles.push_back(vhandle[4]);
	mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[7]);
	//face_vhandles.push_back(vhandle[6]);
	face_vhandles.push_back(vhandle[5]);
	face_vhandles.push_back(vhandle[4]);
	mesh.add_face(face_vhandles);


	face_vhandles.clear();
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[4]);
	//face_vhandles.push_back(vhandle[5]);
	mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[1]);
	//face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[4]);
	face_vhandles.push_back(vhandle[5]);
	mesh.add_face(face_vhandles);


	face_vhandles.clear();
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[5]);
	//face_vhandles.push_back(vhandle[6]);
	mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[2]);
	//face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[5]);
	face_vhandles.push_back(vhandle[6]);
	mesh.add_face(face_vhandles);


	face_vhandles.clear();
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[6]);
	//face_vhandles.push_back(vhandle[7]);
	mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[3]);
	//face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[6]);
	face_vhandles.push_back(vhandle[7]);
	mesh.add_face(face_vhandles);


	face_vhandles.clear();
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[7]);
	//face_vhandles.push_back(vhandle[4]);
	mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[0]);
	//face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[7]);
	face_vhandles.push_back(vhandle[4]);
	mesh.add_face(face_vhandles);

	 mesh.request_vertex_normals();
	 mesh.request_face_normals();
	 mesh.update_normals();
	return mesh;
}

//function that actually does the drawing
void myDisplay() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();				// loading the identity matrix for the screen

	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	float aspect_ratio = (float) w / (float) h;
	float fieldOfView = 45.0f; //TODO
	gluPerspective(fieldOfView, aspect_ratio, 0.01f, 600.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear to black, fully transparent
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);


	glMatrixMode(GL_MODELVIEW);					// indicate we are specifying camera transformations
	glLoadIdentity();
	Vector3f cameraPos = Vector3f(0,0,4); //TODO
	Vector3f up = Vector3f(0,1,0); //TODO
	Vector3f direction = Vector3f(0,0,-1); //TODO
	gluLookAt(cameraPos[0], cameraPos[1], cameraPos[2], cameraPos[0]+direction[0], cameraPos[1]+direction[1], cameraPos[2]+direction[2], up[0], up[1], up[2]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// clear the color buffer (sets everything to black)

	//glMatrixMode(GL_MODELVIEW);					// indicate we are specifying camera transformations
	//glLoadIdentity();							// make sure transformation is "zero'd"




	//enabling lighting/ shading
	glEnable(GL_LIGHTING);
	unsigned int gl_lights[] = {GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4,
		GL_LIGHT5, GL_LIGHT6, GL_LIGHT7};
	int numLights = 1; //TODO
	for(int i = 0; i<numLights; i++){
		//TODO: all these variables must depend on game room lights 
		glEnable(gl_lights[0]);
		GLfloat ambientlight[] = {0.3,0.3,0.3,1.0};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientlight);

		//light colors
		GLfloat ambientcolor[] = {0.2, 0.2, 0.2, 1.0};
		glLightfv(gl_lights[0], GL_AMBIENT, ambientcolor);
		GLfloat diffusecolor[] = {0.8, 0.8, 0.4, 1.0};
		glLightfv(gl_lights[0], GL_DIFFUSE, diffusecolor);
		GLfloat specularcolor[] = {0.2, 0.2, 0.2, 1.0};
		glLightfv(gl_lights[0], GL_SPECULAR, specularcolor);

		GLfloat lightposition[] = {300.0, 300.0, 300.0, 1.0};
		if(true){ //positional
			lightposition[3] = 1.0;
		}else{//directional
			lightposition[3] = 0.0;
		}
		glLightfv(gl_lights[0], GL_POSITION, lightposition);

		if(true){ // should have a setting for this
			GLfloat lightdirection[] = {-1.0,-1.0,-1.0};
			glLightfv(gl_lights[0], GL_SPOT_DIRECTION, lightdirection);
			glLightf(gl_lights[0], GL_SPOT_CUTOFF, 30.0);
		}
	}





	//----------------------- code to draw objects --------------------------
	
	glEnable(GL_NORMALIZE);
	int numObjects = 1;
	for(int i = 0; i<numObjects;i++){
		MyMesh mesh = squareMesh();
		bool useCustomShader = false; //TODO
		if(useCustomShader){ //check if you should use custom shader
			GLuint programID = 1; //TODO
			//glUseProgram(programID); //TODO: this gives errors. Not sure why. Something to do with linking with glew.
		}else{
			//glUseProgram(0); //standard openGL shader
		}
		
		//set materials and textures
		//TODO all material properties should be read in from object
		Material material = exampleMaterial();
		GLfloat ambientmat[] = { material.Ka[0], material.Ka[1], material.Ka[2], 1.0-material.Tr };
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambientmat);
		GLfloat diffusemat[] = { material.Kd[0], material.Kd[1], material.Kd[2], 1.0-material.Tr };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffusemat);
		GLfloat specularmat[] = { material.Ks[0], material.Ks[1], material.Ks[2], 1.0-material.Tr };
		glMaterialfv(GL_FRONT, GL_SPECULAR, specularmat);
		GLfloat shinimat[] = {material.Ns};
		glMaterialfv(GL_FRONT, GL_SHININESS, shinimat);
		GLfloat emmisionmat[] = { material.Ke[0], material.Ke[1], material.Ke[2], 1.0-material.Tr };
		glMaterialfv(GL_FRONT, GL_EMISSION, emmisionmat);

		bool useTexture = false; //TODO
		if(useTexture){
			glEnable(GL_TEXTURE_2D);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			GLuint textureID = 0; //TODO
			glBindTexture(GL_TEXTURE_2D, textureID);
		}else{
			glDisable(GL_TEXTURE_2D);
		}

		//set transformations - opengl will apply these in REVERSE order.
		glPushMatrix();
		glTranslatef(-0.5, 0, 0); //move cube2 to the left
		glRotatef(45, 1.0, 0.0, 0.0); // angle in degrees, x, y,z
		glScalef(0.8f, 1.2f, 1.0f);
		glBegin(GL_TRIANGLES);
		for (MyMesh::FaceIter it = mesh.faces_begin(); it != mesh.faces_end(); ++it) {
			//assuming triangular meshes
			MyMesh::HalfedgeHandle it2 = mesh.halfedge_handle(it.handle());
			for(int v = 0; v< 3; v++){
				MyMesh::VertexHandle v_handle = mesh.to_vertex_handle(it2);
				if(false){ // should there be a setting for using face or vertex normals?
					if(mesh.has_vertex_normals()){
						Vec3f avg =mesh.normal(v_handle);
						glNormal3f(avg[0], avg[1], avg[2]);
					}
				}else{
					if(mesh.has_face_normals()){
						Vec3f avg =mesh.normal(it.handle());
						glNormal3f(avg[0], avg[1], avg[2]);
					}
				}
				if(useTexture){
					Vec2f texCoord; //TODO
					glTexCoord2f(texCoord[0], texCoord[1]);
				}
				Vec3f p = mesh.point(v_handle);
				glVertex3f(p[0],p[1],p[2]);
				it2 = mesh.next_halfedge_handle(it2);
			}
		}
		glEnd();
		glPopMatrix(); // you need one of these for every glPushMatrix()
	}
	
	//glutSolidSphere(1.0, 20, 20); //for debugging
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