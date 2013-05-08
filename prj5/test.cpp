/**
*
*/

#include "MMath.h"
#include <iostream>
#include <vector>
#include "debug.h"
#include "CMSpline.h"
#include "TaskQueue.h"

using namespace std; 

Vec3f verts[12]; 
Quaternian qs[3];
GLfloat mat [][4] = 
{{1.0f, 0, 0 , 4.0f},
{0, 1.0f, 0 , 1.0f},
{0.0f, 0, 1.0f , 20.0f},
{0, 0, 0 , 1.0f}};
Matrix4 T(&mat[0][0]); 
vector<Vec3f> pts (12); 
TaskQueue *tq;
void debug(){}

void init(){

	verts[0]=Vec3f(-1.0f, 	0, 0);
	verts[1]=Vec3f(1.0f, 	0, 0);
	verts[2]=Vec3f(0.0f, 	0, 1.732f);
	verts[3]=Vec3f(-1.0f, 	0, 0);
	verts[4]=Vec3f(1.0f, 	0, 0);
	verts[5]=Vec3f(0.0f, 	1.75f, 0.866f);
	verts[6]=Vec3f(-1.0f, 	0, 0);
	verts[7]=Vec3f(0, 	1.75f, 0.866f);
	verts[8]=Vec3f(0, 	0, 1.732f);
	verts[9]=Vec3f(1.0f, 	0, 0);
	verts[10]=Vec3f(0, 	0, 1.732f);
	verts[11]=Vec3f(0.0f, 	1.75f, 0.866f);
	for (int i =0; i<12; i++){
		pts[i] = verts[i];
	}
	qs[0] = Quaternian::generateRotation(30, Vec3f(1, 0, 0));
	qs[1] = Quaternian::generateRotation(180, Vec3f(0,0,1));
 	qs[2] = Quaternian::generateRotation(90, Vec3f(0,0,1)); 
	tq = new TaskQueue(8); 
}


void test(){
	for (int i = 0; i < 400000; i ++){ 
	Quaternian q = qs[2];
	Vec3f v = verts[1]; 
	//print_Quaternian(Quaternian (v));
	//print_Quaternian(q);
	//print_Quaternian (q.conj());
	//print_Quaternian (q.neg());
	//print_Quaternian (q.rot(v)); 
	Vec3f v1 = q.rot(v).real();
	//print_Vec3f(v1); 
	Vec3f v2 = q.rot(v1).real();
	//print_Vec3f(v2);
	Vec4f v3(v1);
	//print_Matrix4(T); 
	//print_Vec4f(v3); 
	Vec4f v4 = T * v3; 	
	//print_Vec4f(v4);
	//cout << "rotation test"<< endl;
	Matrix4 T1 = q.generateRotationMatrix();
	v4 = T1*v3;
	//print_Matrix4(T1); 
	//print_Vec4f(v4);
	vector <VCMSpline> *splines = VCMSpline::generateSplineCurve(pts);
 	for (int i = 0; i < 11; i++){
		VCMSpline *s = &(*splines)[i];
		//cout << i << "=>" << i +1 << endl;
		for (GLfloat j = 0; j < 1.0f; j += 10.0f/60.0f){
			(s->interpolate(j));
		} 
	}
}

} 

class Test : public Task{
	public:
	Test(){}
	void run(){
		cout << "tid: " << getTid() << "-- started" << endl; 
		test();
		cout <<"tid: " << getTid() << "-- done" << endl;
	}
	~Test(){};
	void done(){}
};

int main(int argc, char **argv){
	init();
	int n = 1;
	Test data[n]; 
	for (int i = 0; i < n; i ++){
		tq->enqueue(&data[i]);
	}

	for (int i = 0; i < n; i ++){
		data[i].join();
	} 	
}
