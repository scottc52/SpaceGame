/*
*
* Actual functionality.
*
*/
#include "Render.h"
#include "shader_utils.h"
#include "bloom_utils.h"
#include "projectile_particles.h"
#include "GameObjectHeaderList.h"

//Allocate Statics
GameState *Render::gameState = NULL;

pthread_mutex_t Render::lock; 
bool Render::drawing = false;
bool Render::frameRequested = false;  
bool Render::pauseToggle = false;
bool Render::paused = false; 
int Render::h = 600;
int Render::w = 800;  
//****************************************************
// function prototypes (so they can be called before they are defined)
//****************************************************
void setSimpleFog(float offset, float scale, GLuint texture, GLuint depth);
void setBloom(GLuint base, Surface *passes);

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
// global variables
//****************************************************

int prevT; //time of previous myIdle call

int lastHit;
Vector3f cameraPos;
float near_p, far_p;
//SmokyBullet *bullet = new SmokyBullet();

int anti_alias = 2; // should be at least 1
//TODO: if time, do proper anti-aliasing by multi_sampling and down sampling using a kernel (borrow shader from blur).
//	only the unblurred frames should be rendered in high-rez. Probably should make a new fbo.
//right now, only values of 1 and 2 are supported

//Shaders
Surface fbo0, fbo1;
Surface pass0[4];
Surface pass1[4];
GLuint vbo_fbo_vertices;
GLuint program_postproc, attribute_v_coord_postproc, uniform_source_postproc, uniform_hit_time_postproc, uniform_damage_postproc;
GLuint program_fog, uniform_source_fog, uniform_depth_fog, uniform_z_offset_fog, uniform_z_scale_fog, uniform_z_pow_fog, uniform_min_fog, uniform_max_fog, uniform_color_fog, uniform_z_near_fog, uniform_z_far_fog;
//5x5 gaussian blur filter (using 3 lookups per pass)
GLuint program_blur, uniform_source_blur, uniform_offsetx_blur, uniform_offsety_blur, uniform_coefficients_blur;
//9x9 gaussian blur filter (using 5 lookups per pass)
GLuint program_blur9, uniform_source_blur9, uniform_offsetx1_blur9, uniform_offsety1_blur9, uniform_offsetx2_blur9, uniform_offsety2_blur9, uniform_coefficients_blur9;
GLuint program_bloom, uniform_sourceBase_bloom, uniform_source0_bloom, uniform_source1_bloom, uniform_source2_bloom, uniform_source3_bloom;

//****************************************************
// reshape viewport if the window is resized
//****************************************************
void Render::myReshape(int w, int h) {
	//glViewport(viewport.w/2,viewport.h/2,viewport.w,viewport.h);// sets the rect angle that will be the window
	Render::w= w;
	Render::h= h;
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glViewport(0, 0, w, h);
	

	//glDepthFunc(GL_LEQUAL);
	//glLineWidth(1);
	//----------- setting the projection -------------------------
	// glOrtho sets left, right, bottom, top, zNear, zFar of the chord system


	// glOrtho(-1, 1 + (w-400)/200.0 , -1 -(h-400)/200.0, 1, 1, -1); // resize type = add
	// glOrtho(-w/400.0, w/400.0, -h/400.0, h/400.0, 1, -1); // resize type = center

	//glOrtho(-1, 1, -1, 1, 1, -1);	// resize type = stretch

	//------------------------------------------------------------

	//post-processing
	//resize framebuffer and render buffer
	int width = w;
	int height = h;
	resizeSurface(&fbo0, width*anti_alias, height*anti_alias);
	resizeSurface(&fbo1, width, height);
	for(int p = 0; p<4; p++){
		resizeSurface(pass0+p,width, height);
		resizeSurface(pass1+p,width, height);
		width = width/2;
		height = height/2;
	}
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

//functions that actually does the drawing
void setupCamera(){
	Camera *cam  = Render::gameState->GetCamera();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();				// loading the identity matrix for the screen

	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	float aspect_ratio = (float) w / (float) h;
	float fieldOfView = cam->getVerticalFieldOfView(); //TODO
	near_p = cam->getNearViewPlane();
	far_p = cam->getFarViewPlane();
	gluPerspective(fieldOfView, aspect_ratio, near_p, far_p);

	
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_ALWAYS);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	


	Vector3f pos = cam->getPivotPoint(); //TODO
	cameraPos = pos;
	Vector3f up = cam->getUpVector(); //TODO
	Vector3f direction = cam->getDirection(); //TODO
	Vector3f look = pos+direction;
	gluLookAt(
	 	pos[0], pos[1], pos[2],
	 	look[0], look[1], look[2], 
	 	up[0], up[1], up[2]);
	glDisable(GL_LIGHTING);
}

void Render::setHitTime(int t){
	lastHit = t;
}

void Render::hitEffect(){
	lastHit = glutGet(GLUT_ELAPSED_TIME);
}

void setupLighting(){
	//enabling lighting/ shading
	glEnable(GL_LIGHTING);
	static float lmodel_twoside[] = { GL_TRUE };
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE , lmodel_twoside);
	//The following two lines can make specular lighting more accurate, but is usually not necessary.
	//glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,GL_SEPARATE_SPECULAR_COLOR);
	//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	unsigned int gl_lights[] = {GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4,
		GL_LIGHT5, GL_LIGHT6, GL_LIGHT7};
	int numLights = 1; //TODO
	for(int i = 0; i<numLights; i++){
		//TODO: all these variables must depend on game room lights 
		glEnable(gl_lights[0]);
		GLfloat ambientlight[] = {0.3,0.3,0.3,1.0};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, &ambientlight[0]);

		//light colors
		GLfloat ambientcolor[] = {0.2, 0.2, 0.2, 1.0};
		glLightfv(gl_lights[0], GL_AMBIENT, ambientcolor);
		GLfloat diffusecolor[] = {0.8, 0.8, 0.4, 1.0};
		glLightfv(gl_lights[0], GL_DIFFUSE, diffusecolor);
		GLfloat specularcolor[] = {0.2, 0.2, 0.2, 1.0};
		glLightfv(gl_lights[0], GL_SPECULAR, specularcolor);

		GLfloat lightposition[] = {-1.0, 0.0, -30.0, 1.0};
		if(true){ //positional
			lightposition[3] = 1.0;
		}else{//directional
			lightposition[3] = 0.0;
		}
		glLightfv(gl_lights[0], GL_POSITION, &lightposition[0]);
		glEnable(gl_lights[0]);
		glEnable(GL_DEPTH_TEST);
		if(false){ // should have a setting for this
			GLfloat lightdirection[] = {1.0,1.0,1.0};
			glLightfv(gl_lights[0], GL_SPOT_DIRECTION, lightdirection);
			glLightf(gl_lights[0], GL_SPOT_CUTOFF, 30.0);
		}
	}
}

void bindMaterial(Material &material){
	GLfloat ambientmat[] = { material.Ka[0], material.Ka[1], material.Ka[2], 1.0-material.Tr };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientmat);
	GLfloat diffusemat[] = { material.Kd[0], material.Kd[1], material.Kd[2], 1.0-material.Tr };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffusemat);
	GLfloat specularmat[] = { material.Ks[0], material.Ks[1], material.Ks[2], 1.0-material.Tr };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularmat);
	GLfloat shinimat[] = {material.Ns};
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shinimat);
	GLfloat emmisionmat[] = { material.Ke[0], material.Ke[1], material.Ke[2], 1.0-material.Tr };
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emmisionmat);
}

void drawTestPrism(){
	glBegin(GL_TRIANGLES);
		glColor4f(0.20f, 0.20f,  1.0f, 1.0f);

		glVertex3f(-10.0f, 0, 0);
		glVertex3f(0.0f, 10.0f, 0);
		glVertex3f(10.0f, 0, 0);
		glVertex3f(10.0f, 0, 0);
		glVertex3f(0.0f, 10.0f, 0);
		glVertex3f(-10.0f, 0, 0);
		glVertex3f(10.0f, 0, 0);
		glVertex3f(0.0f, 0, 10.0f);
		glVertex3f(-10.0f, 0, 0);
		glVertex3f(-10.0f, 0, 0);
		glVertex3f(0.0f, 0, 10.0f);
		glVertex3f(10.0f, 0, 0);

		glVertex3f(-10.0f, 0, 0);
		glVertex3f(0.0f, 10.0f, 0);
		glVertex3f(0, 0, 10.0f);
		glVertex3f(0.0f, 0, 10.0);
		glVertex3f(0.0f, 10.0f, 0);
		glVertex3f(-10.0f, 0, 0);
		glVertex3f(10.0f, 0, 0);
		glVertex3f(0.0f, 0, 10.0f);
		glVertex3f(0.0f, 10.0, 0);
		glVertex3f(0.0f, 10.0, 0);
		glVertex3f(0.0f, 0, 10.0f);
		glVertex3f(10.0f, 0, 0);
	glEnd();
	glBegin(GL_LINES);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f); 
		glVertex3f(10.1f, 0.1, 0.1);
		glVertex3f(0.1f, 0.0, 10.1f);

		glVertex3f(10.0f, 0.1, 0.1);
		glVertex3f(0.1, 10.1f, 0.1);
		
		glVertex3f(10.0f, 0.1, 0.1);
		glVertex3f(-10.0f, 0.1, 0.1);

		glVertex3f(0.1, 10.1f, 0.1);
		glVertex3f(0.1, 0.1, 10.1f);

		glVertex3f(0.1, 10.1f, 0.1);
		glVertex3f(-10.1f, 0.1, 0.1);

		glVertex3f(0.1, 0.1, 10.1f);
		glVertex3f(-10.1f, 0.1, 0.1);
	glEnd();



}

void drawFrame(){
	//Now that we have fbo, we can easily do anti-aliasing through mutil-sampling. 
	//If necessary, do that instead using polygon_smooth which doesn't work well with depth
	//testing.
	GameRoom *gr = Render::gameState->GetRoom(); 	
	//map<string, GameWorldObject>::iterator iter = gr->GetRoomWorldObjectsIterator(), end = gr->GetRoomWorldObjectsEnd(); 
	vector<GameWorldObject*> wobs = gr->GetWorldObjects();
	//for(int i = 0; i<numObjects;i++){
	GameTime::GameTimer ref = GameTime::GetTime(); 
	for(unsigned int w = 0; w <wobs.size(); w++){
		GameWorldObject *gwo = wobs[w]; 
		MyMesh *mesh = gwo->GetMesh();
		if (!mesh)
			continue;  
		//cout << "rendering " << gwo->GetName() << endl;
		bool useCustomShader = false; //TODO
		if(useCustomShader){ //check if you should use custom shader
			GLuint programID = 1; //TODO
			glUseProgram(programID); //TODO: this gives errors. Not sure why. Something to do with linking with glew.
		}else{
			glUseProgram(0); //standard openGL shader
		}
		
		//set materials and textures
		//TODO all material properties should be read in from object
		Material material = exampleMaterial();
		//bindMaterial(material); 

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
		Vector3f Position = gwo->GetPosition();
		glTranslatef(Position[0], Position[1], Position[2]); //move cube2 to the left
		Vec4f Rotation = gwo->GetRotation();
		glRotatef(Rotation[0], Rotation[1], Rotation[2], Rotation[3]); // angle in degrees, x, y,z
		bool non_uniform_scaling = false;
		if(non_uniform_scaling){
			//This is here so scaling doesn't screw up normal vectors.
			//It should only be used if there's non-uniform scaling, since it's less efficient.
			glEnable(GL_NORMALIZE);
		}else{
			glEnable(GL_RESCALE_NORMAL);
		}
		glScalef(1.0f, 1.0, 1.0f);
		//drawTestPrism();
		glBegin(GL_QUADS);
		for (MyMesh::FaceIter it = mesh->faces_begin(); it != mesh->faces_end(); ++it) {
			//assuming triangular meshes
			MyMesh::HalfedgeHandle it2 = mesh->halfedge_handle(it.handle());
			for(int v = 0; v< 4; v++){
				MyMesh::VertexHandle v_handle = mesh->to_vertex_handle(it2);
				if(false){ // should there be a setting for using face or vertex normals?
					if(mesh->has_vertex_normals()){glVertex3f(1.0f, 0, 0);
						Vec3f avg =mesh->normal(v_handle);
						glNormal3f(avg[0], avg[1], avg[2]);
					}
				}else{
					if(mesh->has_face_normals()){
						Vec3f avg =mesh->normal(it.handle());
						glNormal3f(avg[0], avg[1], avg[2]);
					}
				}
				if(useTexture){
					Vec2f texCoord; //TODO
					glTexCoord2f(texCoord[0], texCoord[1]);
				}
				Vec3f p = mesh->point(v_handle);
				glVertex3f(p[0],p[1],p[2]);
				it2 = mesh->next_halfedge_handle(it2);
			}
		}
		glEnd();
		glPopMatrix(); // you need one of these for every glPushMatrix()
		glDisable(GL_NORMALIZE);
		glDisable(GL_RESCALE_NORMAL);
		//render Actors AKA metaball Warriors!

	}
	cerr << "rendering objects took: "<< GameTime::DiffTimeMS(ref) <<  endl ;
	list<AI *>::iterator it = Render::gameState->GetActors()->begin();
	list<AI *>::iterator end = Render::gameState->GetActors()->end();
		
	ref = GameTime::GetTime();	
	while (it != end){
		AI *ai = *it; 
		ai->render();
		it++;
	}
	cerr << "rendering actors took: "<< GameTime::DiffTimeMS(ref) <<  endl ;

}

void drawGlow(){
	GameRoom *gr = Render::gameState->GetRoom(); 	
	//map<string, GameWorldObject>::iterator iter = gr->GetRoomWorldObjectsIterator(), end = gr->GetRoomWorldObjectsEnd(); 
	vector<GameWorldObject*> wobs = gr->GetWorldObjects();
	//for(int i = 0; i<numObjects;i++){
	for(unsigned int w = 0; w < wobs.size(); w++){
		GameWorldObject *gwo = wobs[w]; 
		MyMesh *mesh = gwo->GetMesh();
		if (!mesh)
			continue;  
		//cout << "glowing " << gwo->GetName() << endl;
		bool useCustomShader = false; //TODO
		if(useCustomShader){ //check if you should use custom shader
			GLuint programID = 1; //TODO
			glUseProgram(programID); //TODO: this gives errors. Not sure why. Something to do with linking with glew.
		}else{
			glUseProgram(0); //standard openGL shader
		}
		
		//set materials and textures
		//TODO all material properties should be read in from object
		bool useTexture = false; //TODO
		if(useTexture){
			glEnable(GL_TEXTURE_2D);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			GLuint textureID = 0; //TODO
			glBindTexture(GL_TEXTURE_2D, textureID);
		}else{
			glDisable(GL_TEXTURE_2D);
		}

		//set transformations - opengl will apply these in REVERSE order.
		glPushMatrix();
		Vector3f Position = gwo->GetPosition();
		glTranslatef(Position[0], Position[1], Position[2]); //move cube2 to the left
		Vec4f Rotation = gwo->GetRotation();
		glRotatef(Rotation[0], Rotation[1], Rotation[2], Rotation[3]); // angle in degrees, x, y,z
		//normal scaling code shouldn't be necessary
		glScalef(1.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);
		if(false){
			glColor4f(0.1f, 1.0f, 1.0f, 1.0f); //TODO obviously
		}else{
			glColor4f(0.0f, 0.0f, 0.0f, 1.0f); //render with black if not glowing
		}
		//glColor4f(0.0f, 0.0f, 0.0f, 1.0f); //for debugging
		for (MyMesh::FaceIter it = mesh->faces_begin(); it != mesh->faces_end(); ++it) {
			//assuming triangular meshes
			MyMesh::HalfedgeHandle it2 = mesh->halfedge_handle(it.handle());
			for(int v = 0; v< 4; v++){
				MyMesh::VertexHandle v_handle = mesh->to_vertex_handle(it2);
				if(useTexture){
					Vec2f texCoord; //TODO
					glTexCoord2f(texCoord[0], texCoord[1]);
				}
				Vec3f p = mesh->point(v_handle);
				glVertex3f(p[0],p[1],p[2]);
				it2 = mesh->next_halfedge_handle(it2);
			}
		}
		glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
		glEnd();
		glPopMatrix(); // you need one of these for every glPushMatrix()
		glDisable(GL_NORMALIZE);
		glDisable(GL_RESCALE_NORMAL);
	}
}

void drawBullets(bool glow){
	glDepthMask(GL_FALSE);
	glEnable( GL_BLEND );
	glEnable( GL_PROGRAM_POINT_SIZE_EXT );
	static GLfloat attenuate[3] = { 1.0, 0.01, 0.005 };  //Const, linear, quadratic 
	glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, attenuate); 
	list<Projectile *> *bullets = Render::gameState->GetParticleSystems()->GetBullets();
	Render::gameState->GetParticleSystems()->monitor.Enter('r');   	
	list<Projectile *>::iterator it = bullets->begin(); 
	while(it != bullets->end()){  
		Projectile *curBullet = *it;
		if(!curBullet->isDead() /*&& (glow || !curBullet->glow)*/){
			//if(glow && !curBullet->glow){
				//mask glow
				//glBlendFunc( GL_ZERO, GL_ONE_MINUS_SRC_ALPHA );
				//glBlendFunc( GL_SRC_ALPHA, GL_SRC_ALPHA_SATURATE );
			//}else{
				glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
				//glBlendFunc( GL_SRC_ALPHA, GL_ONE );
			//}
			curBullet->display(cameraPos, glow);
		}
		it++;
	}
	Render::gameState->GetParticleSystems()->monitor.Exit('r');
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

void performBlur(Surface *original, Surface *temporary, int numBuffers){
	//blur into temporary, then blur back into original (ping pong)
	//assuming and originla and temporary are matching arrays of surfaces
	glUseProgram(program_blur);
	float kernel[] = {5.0f/16.0f, 6.0f/16.0f, 5.0f/16.0f};
	glUniform1fv(uniform_coefficients_blur, 3, kernel);

	glUniform1i(uniform_source_blur, 0);
	glActiveTexture(GL_TEXTURE0);

	Surface *source;
	Surface *dest;
	GLuint offsetDim;

	source = original;
	dest = temporary;
	offsetDim = uniform_offsetx_blur;
	glUniform1f(uniform_offsetx_blur, 0.0);
	glUniform1f(uniform_offsety_blur, 0.0);
	for (int i = 0; i < numBuffers; i++)
    {
        float offset = 1.2f / source[i].width;
        glUniform1f(offsetDim, offset);
        bindSurface(dest + i);
		clearSurfaceColor(0.0, 0.0, 0.0, 1.0);
        glBindTexture(GL_TEXTURE_2D, source[i].texture);
        glBegin(GL_QUADS);
			glTexCoord2i(0, 0); glVertex2i(-1, -1);
			glTexCoord2i(1, 0); glVertex2i(1, -1);
			glTexCoord2i(1, 1); glVertex2i(1, 1);
			glTexCoord2i(0, 1); glVertex2i(-1, 1);
        glEnd();
    }


	source = temporary;
	dest = original;
	offsetDim = uniform_offsety_blur;
	glUniform1f(uniform_offsetx_blur, 0.0);
	glUniform1f(uniform_offsety_blur, 0.0);
	for (int i = 0; i < numBuffers; i++)
    {
        float offset = 1.2f / source[i].height;
        glUniform1f(offsetDim, offset);
        bindSurface(dest + i);
		clearSurfaceColor(0.0, 0.0, 0.0, 1.0);
        glBindTexture(GL_TEXTURE_2D, source[i].texture);
        glBegin(GL_QUADS);
			glTexCoord2i(0, 0); glVertex2i(-1, -1);
			glTexCoord2i(1, 0); glVertex2i(1, -1);
			glTexCoord2i(1, 1); glVertex2i(1, 1);
			glTexCoord2i(0, 1); glVertex2i(-1, 1);
        glEnd();
    }
}

void performBlur9(Surface *original, Surface *temporary, int numBuffers){
	//blur into temporary, then blur back into original (ping pong)
	//assuming and originla and temporary are matching arrays of surfaces
	glUseProgram(program_blur9);
	float kernel[] = {9.0f/256.0f, 84.0f/256.0f, 70.0f/256.0f, 84.0f/256.0f, 9.0f/256.0f};
	glUniform1fv(uniform_coefficients_blur9, 5, kernel);

	glUniform1i(uniform_source_blur9, 0);
	glActiveTexture(GL_TEXTURE0);

	Surface *source;
	Surface *dest;
	GLuint offsetDim1, offsetDim2;

	source = original;
	dest = temporary;
	offsetDim1 = uniform_offsetx1_blur9;
	offsetDim2 = uniform_offsetx2_blur9;
	glUniform1f(uniform_offsetx1_blur9, 0.0);
	glUniform1f(uniform_offsety1_blur9, 0.0);
	glUniform1f(uniform_offsetx2_blur9, 0.0);
	glUniform1f(uniform_offsety2_blur9, 0.0);
	for (int i = 0; i < numBuffers; i++)
    {
        float offset1 = (4.0f/3.0f) / source[i].width;
		float offset2 = (28.0f/9.0f) / source[i].width;
        glUniform1f(offsetDim1, offset1);
		glUniform1f(offsetDim2, offset2);
        bindSurface(dest + i);
		clearSurfaceColor(0.0, 0.0, 0.0, 1.0);
        glBindTexture(GL_TEXTURE_2D, source[i].texture);
        glBegin(GL_QUADS);
			glTexCoord2i(0, 0); glVertex2i(-1, -1);
			glTexCoord2i(1, 0); glVertex2i(1, -1);
			glTexCoord2i(1, 1); glVertex2i(1, 1);
			glTexCoord2i(0, 1); glVertex2i(-1, 1);
        glEnd();
    }


	source = temporary;
	dest = original;
	offsetDim1 = uniform_offsety1_blur9;
	offsetDim2 = uniform_offsety2_blur9;
	glUniform1f(uniform_offsetx1_blur9, 0.0);
	glUniform1f(uniform_offsety1_blur9, 0.0);
	glUniform1f(uniform_offsetx2_blur9, 0.0);
	glUniform1f(uniform_offsety2_blur9, 0.0);
	for (int i = 0; i < numBuffers; i++)
    {
        float offset1 = (4.0f/3.0f) / source[i].height;
		float offset2 = (28.0f/9.0f) / source[i].height;
        glUniform1f(offsetDim1, offset1);
		glUniform1f(offsetDim2, offset2);
        bindSurface(dest + i);
		clearSurfaceColor(0.0, 0.0, 0.0, 1.0);
        glBindTexture(GL_TEXTURE_2D, source[i].texture);
        glBegin(GL_QUADS);
			glTexCoord2i(0, 0); glVertex2i(-1, -1);
			glTexCoord2i(1, 0); glVertex2i(1, -1);
			glTexCoord2i(1, 1); glVertex2i(1, 1);
			glTexCoord2i(0, 1); glVertex2i(-1, 1);
        glEnd();
    }
}

void drawCrossHair(float w, float h){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w/2.0, w/2.0, -h/2.0, h/2.0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_TEXTURE_2D);
	glUseProgram(0);
	glEnable( GL_LINE_SMOOTH );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	
	//draw circle
	glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
	glLineWidth(2.0);
	glBegin(GL_LINE_LOOP);
	float radius = 20;
	for (int i=0; i < 360; i = i+10)
	{
		float degInRad = i*PI/180.0;
		glVertex2f(cos(degInRad)*radius,sin(degInRad)*radius);
	}
	glEnd();
	
	//draw cross hair
	glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
	glLineWidth(2.0);
	float inRadius = 12;
	float outRadius = 32;
	glBegin(GL_LINES);
		glVertex2f(inRadius, 0.0); glVertex2f(outRadius, 0.0);
		glVertex2f(-inRadius, 0.0); glVertex2f(-outRadius, 0.0);
		glVertex2f(0.0, inRadius); glVertex2f(0.0, outRadius);
		glVertex2f(0.0, -inRadius); glVertex2f(0.0, -outRadius);
	glEnd();

	glDisable(GL_BLEND);
}

void Render::defaultDisplay(){
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	int t = glutGet(GLUT_ELAPSED_TIME);
	GLfloat dt = (float)(t - lastHit);  
	

	bindSurface(&fbo0);
	glDisable(GL_LIGHTING);
	clearSurfaceColor(0.0f, 0.0f, 0.0f, 1.0f); // Clear to black
	glUseProgram(0);
	setupCamera();	
	drawGlow();
	drawBullets(true);

	
	bindSurface(&fbo1);
	clearSurfaceColor(0.0f, 0.0f, 0.0f, 1.0f); // Clear to black
	//glUseProgram(program_fog); //done in setSimpleFog.
	setSimpleFog(1.0, 0.1, fbo0.texture, fbo0.depth);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
    glEnd();
	
	glUseProgram(0);
	glEnable(GL_TEXTURE_2D); glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, fbo1.texture);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();				// loading the identity matrix for the screen
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    for (int p = 0; p < 4; p++)
    {
        bindSurface(pass0 + p);
		clearSurfaceColor(0.0, 0.0, 0.0, 1.0);
        glBegin(GL_QUADS);
			glTexCoord2i(0, 0); glVertex2i(-1, -1);
			glTexCoord2i(1, 0); glVertex2i(1, -1);
			glTexCoord2i(1, 1); glVertex2i(1, 1);
			glTexCoord2i(0, 1); glVertex2i(-1, 1);
        glEnd();
    }
	
	//can do multiple passes
	//performBlur(pass0, pass1, 4);
	//performBlur9(pass0, pass1, 4);
	performBlur9(pass0, pass1, 4);

	//draw meshes
	bindSurface(&fbo0);
	clearSurfaceColor(0.0f, 0.0f, 0.0f, 1.0f); // Clear to black
	glUseProgram(0);
	setupCamera();
	setupLighting();	
	drawFrame();
	glDisable(GL_LIGHTING);
	drawBullets(false);
	
	bindSurface(&fbo1);
	clearSurfaceColor(0.0, 0.0, 0.0, 1.0);
	setBloom(fbo0.texture, pass0); // contaings glUseProgram(program_bloom);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
    glEnd();
	drawCrossHair(w, h);


	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glViewport(0, 0, w, h);
	
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
 
	glUseProgram(program_postproc);
	glUniform1f(uniform_hit_time_postproc, (dt>0)? dt : 100000);
	float damage = 0;
	glUniform1f(uniform_damage_postproc, min(damage/100.0, 1.0));
	glEnable(GL_TEXTURE_2D); 
	glBindTexture(GL_TEXTURE_2D, fbo1.texture);
	glUniform1i(uniform_source_postproc, /*GL_TEXTURE*/0);
	glEnableVertexAttribArray(attribute_v_coord_postproc);
 
	glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
	glVertexAttribPointer(attribute_v_coord_postproc, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableVertexAttribArray(attribute_v_coord_postproc);
	glFlush();
	glutSwapBuffers();
}

void Render::myDisplay() {
	if (gameState->paused){
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW); 
	} else{
		glutSetCursor(GLUT_CURSOR_NONE); 
		defaultDisplay(); 
	}					// swap buffers (we earlier set double buffer)
	drawing = false; 
}

//****************************************************
// called by glut when there are no messages to handle
//****************************************************
void Render::myIdle() {
	pthread_mutex_lock(&lock);
	if(!drawing && frameRequested){ 
		glutPostRedisplay(); // forces glut to call the display function (myDisplay())
		frameRequested = false; 
		drawing = true; 
	}
	pthread_mutex_unlock(&lock); 

}

/**
Event notifications... 
**/

bool Render::requestFrame(){
	bool value = true; 
	pthread_mutex_lock(&lock);
	if (frameRequested){
		cerr<< "warning: frame dropped" << endl;
		value = false;
	}
	frameRequested = true; 
	pthread_mutex_unlock(&lock); 
	return value;
}

void Render::pause(bool t){
	pthread_mutex_lock(&lock);
	if (t != paused){
		pauseToggle = true; 
	}
	pthread_mutex_unlock(&lock);
}

//****************************************************
// UI callback functions
//****************************************************
void Render::myKeyboard(unsigned char key, int x, int y){
	if (key == 'p' || key == 'P') {
		//hit effect
		lastHit = glutGet(GLUT_ELAPSED_TIME);
	}
	else if (key == 'o' || key == 'O') {
		//fire bullet!
		Vector3f loc = Vector3f(0, -1, 4); //below camera
		Vector3f vel = Vector3f(0.0, 0.0, -4.8);
		Vector4f col = Vector4f(0.95, 0.0, 0.0, 0.5);
		//bullet = new SmokyBullet(loc, vel, col[0], col[1], col[2], col[3]);
	}
	else if (key == 'q' || key == 'Q') exit(0);
	//glutPostRedisplay();
}

//****************************************************
// Loading Programs
//****************************************************
int loadPostProcessingProgram(){
	string vertexShader = SHADERS_POSTHIT_VERTEX_FILE;
	string fragmentShader = SHADERS_POSTHIT_FRAGMENT_FILE;
	GLuint vs, fs;
	if ((vs = create_shader(vertexShader.c_str(), GL_VERTEX_SHADER))   == 0) return 0;
	if ((fs = create_shader(fragmentShader.c_str(), GL_FRAGMENT_SHADER)) == 0) return 0;
 
	GLint link_ok, validate_ok;
	program_postproc = glCreateProgram();
	glAttachShader(program_postproc, vs);
	glAttachShader(program_postproc, fs);
	glLinkProgram(program_postproc);
	glGetProgramiv(program_postproc, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		fprintf(stderr, "glLinkProgram:");
		//print_shader_log(program_postproc);
		return 0;
	}
	glValidateProgram(program_postproc);
	glGetProgramiv(program_postproc, GL_VALIDATE_STATUS, &validate_ok); 
	if (!validate_ok) {
		fprintf(stderr, "glValidateProgram:");
		//print_shader_log(program_postproc);
		cout << "post processing program not valid!";
	}
 
	//attribute_name = "v_coord";
	attribute_v_coord_postproc = glGetAttribLocation(program_postproc, "v_coord");
	if (attribute_v_coord_postproc == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", "v_coord");
		return 0;
	}
 
	//uniform_name = "fbo_texture";
	uniform_source_postproc = glGetUniformLocation(program_postproc, "fbo_texture");
	if (uniform_source_postproc == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", "fbo_texture");
		return 0;
	}

	//uniform_name = "hit_time";
	uniform_hit_time_postproc = glGetUniformLocation(program_postproc, "hit_time");
	if (uniform_hit_time_postproc == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", "fbo_texture");
		return 0;
	}

	//uniform_name = "damage";
	uniform_damage_postproc = glGetUniformLocation(program_postproc, "damage");
	if (uniform_damage_postproc == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", "damage");
		return 0;
	}

}

int loadFogProgram(){
	string vertexShader = SHADERS_PASS_VERTEX_FILE;
	string fragmentShader = SHADERS_FOG_FRAGMENT_FILE;
	GLuint vs, fs;
	if ((vs = create_shader(vertexShader.c_str(), GL_VERTEX_SHADER))   == 0) return 0;
	if ((fs = create_shader(fragmentShader.c_str(), GL_FRAGMENT_SHADER)) == 0) return 0;
 
	GLint link_ok, validate_ok;

	program_fog = glCreateProgram();
	GLuint programID = program_fog;
	glAttachShader(programID, vs);
	glAttachShader(programID, fs);
	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		fprintf(stderr, "glLinkProgram:");
		//print_shader_log(program_postproc);
		return 0;
	}
	glValidateProgram(programID);
	glGetProgramiv(programID, GL_VALIDATE_STATUS, &validate_ok); 
	if (!validate_ok) {
		fprintf(stderr, "glValidateProgram:");
		//print_shader_log(program_postproc);
		cout << "post processing program not valid!";
	}
 
	string attr;


	//attribute
	attr = "texturemap";
	uniform_source_fog = glGetUniformLocation(programID, attr.c_str());
	if (uniform_source_fog == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		return 0;
	}

	//attribute
	attr = "depth";
	uniform_depth_fog = glGetUniformLocation(programID, attr.c_str());
	if (uniform_depth_fog == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		return 0;
	}
 
	//attribute
	attr = "offsetf";
	uniform_z_offset_fog = glGetUniformLocation(programID, attr.c_str());
	if (uniform_z_offset_fog == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		return 0;
	}

	//attribute
	attr = "scalef";
	uniform_z_scale_fog = glGetUniformLocation(programID, attr.c_str());
	if (uniform_z_scale_fog == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		return 0;
	}

	//attribute
	attr = "powf";
	uniform_z_pow_fog = glGetUniformLocation(programID, attr.c_str());
	if (uniform_z_pow_fog == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		return 0;
	}

	//attribute
	attr = "minf";
	uniform_min_fog = glGetUniformLocation(programID, attr.c_str());
	if (uniform_min_fog == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		return 0;
	}

	//attribute
	attr = "maxf";
	uniform_max_fog = glGetUniformLocation(programID, attr.c_str());
	if (uniform_max_fog == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		return 0;
	}

	//attribute
	attr = "color";
	uniform_color_fog = glGetUniformLocation(programID, attr.c_str());
	if (uniform_color_fog == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		return 0;
	}

	//attribute
	attr = "far_planef";
	uniform_z_far_fog = glGetUniformLocation(programID, attr.c_str());
	if (uniform_color_fog == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		return 0;
	}

	//attribute
	attr = "near_planef";
	uniform_z_near_fog = glGetUniformLocation(programID, attr.c_str());
	if (uniform_color_fog == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		return 0;
	}
}

void setSimpleFog(float offset, float scale, GLuint texture, GLuint depth){
	 
	glUseProgram(program_fog);
	
	glUniform1f(uniform_z_offset_fog, offset);

	glUniform1f(uniform_z_scale_fog, scale);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(uniform_source_fog, 0);
	
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depth);
	glUniform1i(uniform_depth_fog, 1);

	glActiveTexture(GL_TEXTURE0); //good to set this so other functions don't screw up.

	//set defaults for rest of settings
	glUniform1f(uniform_z_near_fog, near_p);
	glUniform1f(uniform_z_far_fog, far_p);
	glUniform1f(uniform_z_pow_fog, 1.0);
	glUniform1f(uniform_min_fog, 0.0);
	glUniform1f(uniform_max_fog, 1.0);
	glUniform4f(uniform_color_fog, 0.0, 0.0, 0.0, 1.0);
}

int loadBlurProgram(){
	string vertexShader = SHADERS_PASS_VERTEX_FILE;
	string fragmentShader = SHADERS_SUM3_FRAGMENT_FILE;
	GLuint vs, fs;
	if ((vs = create_shader(vertexShader.c_str(), GL_VERTEX_SHADER))   == 0){ 
		fprintf(stderr, "failed to create vertex shader");
		return 0;
	}
	if ((fs = create_shader(fragmentShader.c_str(), GL_FRAGMENT_SHADER)) == 0){ 
		fprintf(stderr, "failed to create fragment shader");
		return 0;
	}
 
	GLint link_ok, validate_ok;

	program_blur = glCreateProgram();
	GLuint programID = program_blur;
	glAttachShader(programID, vs);
	glAttachShader(programID, fs);
	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		fprintf(stderr, "glLinkProgram:");
		//print_shader_log(program_postproc);
		return 0;
	}
	glValidateProgram(programID);
	glGetProgramiv(programID, GL_VALIDATE_STATUS, &validate_ok); 
	if (!validate_ok) {
		fprintf(stderr, "glValidateProgram:");
		//print_shader_log(program_postproc);
		cout << "post processing program not valid!";
	}
 
	string attr;
	//attribute
	attr = "source";
	uniform_source_blur = glGetUniformLocation(programID, attr.c_str());
	if (uniform_source_blur == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		return 0;
	}

	//attribute
	attr = "offsetx";
	uniform_offsetx_blur = glGetUniformLocation(programID, attr.c_str());
	if (uniform_offsetx_blur == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		return 0;
	}

	//attribute
	attr = "offsety";
	uniform_offsety_blur = glGetUniformLocation(programID, attr.c_str());
	if (uniform_offsety_blur == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		return 0;
	}

	//attribute
	attr = "coefficients";
	uniform_coefficients_blur = glGetUniformLocation(programID, attr.c_str());
	if (uniform_coefficients_blur == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		return 0;
	}

	glUseProgram(program_blur);
	float kernel[] = {5.0f/16.0f, 6.0f/16.0f, 5.0f/16.0f};
	glUniform1fv(uniform_coefficients_blur, 3, kernel);

	return 1;
}

int loadBlur9Program(){
	string vertexShader = SHADERS_PASS_VERTEX_FILE;
	string fragmentShader = SHADERS_SUM5_FRAGMENT_FILE;
	GLuint vs, fs;
	if ((vs = create_shader(vertexShader.c_str(), GL_VERTEX_SHADER))   == 0){ 
		fprintf(stderr, "failed to create vertex shader");
		return 0;
	}
	if ((fs = create_shader(fragmentShader.c_str(), GL_FRAGMENT_SHADER)) == 0){ 
		fprintf(stderr, "failed to create fragment shader");
		return 0;
	}
 
	GLint link_ok, validate_ok;

	program_blur9 = glCreateProgram();
	GLuint programID = program_blur9;
	glAttachShader(programID, vs);
	glAttachShader(programID, fs);
	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		fprintf(stderr, "glLinkProgram:");
		//print_shader_log(program_postproc);
		return 0;
	}
	glValidateProgram(programID);
	glGetProgramiv(programID, GL_VALIDATE_STATUS, &validate_ok); 
	if (!validate_ok) {
		fprintf(stderr, "glValidateProgram:");
		//print_shader_log(program_postproc);
		cout << "post processing program not valid!";
	}
 
	string attr;
	//attribute
	attr = "source";
	uniform_source_blur9 = glGetUniformLocation(programID, attr.c_str());
	if (uniform_source_blur9 == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		return 0;
	}

	//attribute
	attr = "offsetx1";
	uniform_offsetx1_blur9 = glGetUniformLocation(programID, attr.c_str());
	if (uniform_offsetx1_blur9 == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		return 0;
	}

	//attribute
	attr = "offsety1";
	uniform_offsety1_blur9 = glGetUniformLocation(programID, attr.c_str());
	if (uniform_offsety1_blur9 == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		return 0;
	}

	//attribute
	attr = "offsetx2";
	uniform_offsetx2_blur9 = glGetUniformLocation(programID, attr.c_str());
	if (uniform_offsetx2_blur9 == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		return 0;
	}

	//attribute
	attr = "offsety2";
	uniform_offsety2_blur9 = glGetUniformLocation(programID, attr.c_str());
	if (uniform_offsety2_blur9 == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		return 0;
	}

	//attribute
	attr = "coefficients";
	uniform_coefficients_blur9 = glGetUniformLocation(programID, attr.c_str());
	if (uniform_coefficients_blur9 == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		return 0;
	}

	glUseProgram(program_blur);
	float kernel[] = {9.0f/256.0f, 84.0f/256.0f, 70.0f/256.0f, 84.0f/256.0f, 9.0f/256.0f};
	glUniform1fv(uniform_coefficients_blur9, 5, kernel);

	return 1;
}

int loadBloomProgram(){
	string vertexShader = SHADERS_PASS_VERTEX_FILE;
	string fragmentShader = SHADERS_SUMTEXTURES4_FRAGMENT_FILE;
	GLuint vs, fs;
	if ((vs = create_shader(vertexShader.c_str(), GL_VERTEX_SHADER))   == 0){ 
		fprintf(stderr, "failed to create vertex shader");
		return 0;
	}
	if ((fs = create_shader(fragmentShader.c_str(), GL_FRAGMENT_SHADER)) == 0){ 
		fprintf(stderr, "failed to create fragment shader");
		return 0;
	}
 
	GLint link_ok, validate_ok;

	program_bloom = glCreateProgram();
	GLuint programID = program_bloom;
	glAttachShader(programID, vs);
	glAttachShader(programID, fs);
	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		fprintf(stderr, "glLinkProgram:");
		//print_shader_log(program_postproc);
		return 0;
	}
	glValidateProgram(programID);
	glGetProgramiv(programID, GL_VALIDATE_STATUS, &validate_ok); 
	if (!validate_ok) {
		fprintf(stderr, "glValidateProgram:");
		//print_shader_log(program_postproc);
		cout << "post processing program not valid!";
	}
 
	string attr;
	//attribute
	attr = "source0";
	uniform_source0_bloom = glGetUniformLocation(programID, attr.c_str());
	if (uniform_source0_bloom == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		//return 0;
	}

	//attribute
	attr = "source1";
	uniform_source1_bloom = glGetUniformLocation(programID, attr.c_str());
	if (uniform_source1_bloom == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		//return 0;
	}

	//attribute
	attr = "source2";
	uniform_source2_bloom = glGetUniformLocation(programID, attr.c_str());
	if (uniform_source2_bloom == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		//return 0;
	}

	//attribute
	attr = "source3";
	uniform_source3_bloom = glGetUniformLocation(programID, attr.c_str());
	if (uniform_source3_bloom == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		//return 0;
	}

	//attribute
	attr = "sourceBase";
	uniform_sourceBase_bloom = glGetUniformLocation(programID, attr.c_str());
	if (uniform_sourceBase_bloom == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attr.c_str());
		//return 0;
	}

	return 1;
}


void setBloom(GLuint base, Surface *passes){
	glUseProgram(program_bloom);
	glEnable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, passes[0].texture);
	glUniform1i(uniform_source0_bloom, 0);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, passes[1].texture);
	glUniform1i(uniform_source1_bloom, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, passes[2].texture);
	glUniform1i(uniform_source2_bloom, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, passes[3].texture);
	glUniform1i(uniform_source3_bloom, 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, base);
	glUniform1i(uniform_sourceBase_bloom, 4);

	glActiveTexture(GL_TEXTURE0); //good to set this so other functions don't screw up.
}

//****************************************************
// Initialize GLUT and resources
//****************************************************
void effectsResourcesInitialize(){
	int windowW = glutGet(GLUT_WINDOW_WIDTH);
	int windowH = glutGet(GLUT_WINDOW_HEIGHT);

	// Vertices for mapping texture to output screen
	GLfloat fbo_vertices[] = {
		-1, -1,
		1, -1,
		-1,  1,
		1,  1,
	};
	glGenBuffers(1, &vbo_fbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//load post-processing program
	loadPostProcessingProgram();
	loadFogProgram();
	loadBlurProgram();
	loadBlur9Program();
	loadBloomProgram();

	//create frame buffer objects with depth buffers
	fbo0.width = windowW;
    fbo0.height = windowH;
    fbo0.viewport.x = 0;
    fbo0.viewport.y = 0;
    fbo0.viewport.width = windowW;
    fbo0.viewport.height = windowH;
    createSurface(&fbo0, GL_TRUE, GL_FALSE, GL_TRUE, GL_TRUE);

	fbo1.width = windowW;
    fbo1.height = windowH;
    fbo1.viewport.x = 0;
    fbo1.viewport.y = 0;
    fbo1.viewport.width = windowW;
    fbo1.viewport.height = windowH;
    createSurface(&fbo1, GL_TRUE, GL_FALSE, GL_TRUE, GL_TRUE);

	//
	//
	//
	//Addditional resources for bloom effect
	//Assume dimensions divisible by 8. Safe for common resolutions (480p, 720p, 1080p,etc.)
	uint w = windowW;
	uint h = windowH;
	for (int p = 0; p < 4; p++)
    {
        pass0[p].width = w;
        pass0[p].height = h;
        pass0[p].viewport.x = 0;
        pass0[p].viewport.y = 0;
        pass0[p].viewport.width = w;
        pass0[p].viewport.height = h;
        createSurface(pass0 + p, GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE);

		pass1[p].width = w;
        pass1[p].height = h;
        pass1[p].viewport.x = 0;
        pass1[p].viewport.y = 0;
        pass1[p].viewport.width = w;
        pass1[p].viewport.height = h;
        createSurface(pass1 + p, GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE);

        w = w >> 1;
        h = h >> 1;
    }
}


void Render::GlutInitialize(){
	cout<<"Start SpaceGame!\n";
	//This initializes glut
	glutGet(GLUT_ELAPSED_TIME); //certain implementations start time from when this is called.
	lastHit = INT_MIN;

	pthread_mutex_init(&lock, NULL);

	//This tells glut to use a double-buffered window with red, green, and blue channels 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	//The size and position of the window
	glutInitWindowSize(w, h);
	glutInitWindowPosition(10, 10);
	glutCreateWindow("SpaceGame!");

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear to black, fully transparent

	glewExperimental=GL_TRUE; 
	GLenum err=glewInit();
	  if(err!=GLEW_OK)
	  {
		//Problem: glewInit failed, something is seriously wrong.
		cout<<"glewInit failed, aborting."<<endl;
	  }

	//setup resources for post-processing
	effectsResourcesInitialize();
	prevT = glutGet(GLUT_ELAPSED_TIME);

	//calling reshape before binding callbacks makes inconsistencies from 
	//reshape operations apparent from the beginning
	myReshape(w,h);
	//setup glut callback funtions
	glutDisplayFunc(myDisplay);				// function to run when its time to draw something
	glutReshapeFunc(myReshape);				// function to run when the window gets resized
	glutIdleFunc(myIdle);				// function to run when not handling any other task
	//glutMotionFunc(mouseMoved);
	//glutMouseFunc(mouse);
	//glutKeyboardFunc(myKeyboard);
	//glutKeyboardUpFunc(keyboardUp);
	glutSetCursor(GLUT_CURSOR_NONE);
}
