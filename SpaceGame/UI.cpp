#include "UI.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <gl/freeglut.h>
#endif

#ifdef _WIN32
#pragma comment(lib, "pthreadVCE2.lib")
#endif

using namespace std;

//initialization of static variables

PCInputManager *PCInputManager::activeCommandSet = NULL;
queue<UIEvent *> PCInputManager::eventQueue;
pthread_mutex_t PCInputManager::eventQueue_mutex;

void PCInputManager::pushState(){
	ActionMap m(stateStack.back());
	stateStack.push_back(m); 
}

void PCInputManager::popState(){
	stateStack.pop_back(); 
	if(stateStack.empty())
		stateStack.push_back(ActionMap());
}


int PCInputManager::oldX = -1; 
int PCInputManager::oldY = -1; 
//public functions

void PCInputManager::EnableUI() {
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutSpecialFunc(SpecialKey);
	glutSpecialUpFunc(SpecialKeyUp);
	glutMouseFunc(MouseClick);
	glutPassiveMotionFunc(MousePassiveMotion);
	glutMotionFunc(MouseMotion);
	//glutMouseWheelFunc(MouseWheel);

	pthread_mutex_init(&eventQueue_mutex, NULL);
}

void PCInputManager::DisableUI() {
	glutKeyboardFunc(NULL);
	glutKeyboardUpFunc(NULL);
	glutSpecialFunc(NULL);
	glutSpecialUpFunc(NULL);
	glutMouseFunc(NULL);
	glutPassiveMotionFunc(NULL);
	glutMotionFunc(NULL);
	glutMouseWheelFunc(NULL);

	pthread_mutex_destroy(&eventQueue_mutex);
}

bool PCInputManager::QueueEmpty() {
	pthread_mutex_lock(&eventQueue_mutex);
	bool isEmtpy = eventQueue.empty();
	pthread_mutex_unlock(&eventQueue_mutex);
	return isEmtpy;
}

UIEvent *PCInputManager::PopEvent() {
	pthread_mutex_lock(&eventQueue_mutex);
	UIEvent *nextEvent = eventQueue.front();
	eventQueue.pop();
	pthread_mutex_unlock(&eventQueue_mutex);
	return nextEvent; 
}

void PCInputManager::AllPending(list<UIEvent *> &buf) {
	pthread_mutex_lock(&eventQueue_mutex);
	while(!eventQueue.empty()){
		buf.push_back(eventQueue.front());
		eventQueue.pop();
	}
	pthread_mutex_unlock(&eventQueue_mutex);
} 

void PCInputManager::FlushQueue() {
	pthread_mutex_lock(&eventQueue_mutex);
	while(!eventQueue.empty()) {
		delete eventQueue.front();
		eventQueue.pop();
	}
	pthread_mutex_unlock(&eventQueue_mutex);
}

PCInputManager::PCInputManager() {
	stateStack.push_back(ActionMap());
}

void PCInputManager::setKeyCallback(int modifier, unsigned char key, bool pressDown, int val) {
	stateStack.back()[
		UIEvent::Specifier(UIEvent::KEY, (int) key, modifier, pressDown)
	] = val;
}

void PCInputManager::setSpecialKeyCallback(int modifier, int key, bool pressDown, int val) {
	stateStack.back()[
		UIEvent::Specifier(UIEvent::SPECIAL_KEY, key, modifier, pressDown)
	] = val;
}

void PCInputManager::setMouseButtonCallback(int modifier, int button, bool pressDown, int val){
	stateStack.back()[
		UIEvent::Specifier(UIEvent::CLICK, button, modifier, pressDown)
	] = val; 
}
void PCInputManager::setMouseMoveCallback(int modifier, bool pressDown /*vs button up*/,int val){
	stateStack.back()[
		UIEvent::Specifier(UIEvent::MOVE, 0, modifier, pressDown)
	] = val; 
}


void PCInputManager::setActiveCommandSet(){
	activeCommandSet = this;
}


UIEvent *PCInputManager::ProcessEvent(UIEvent::Specifier &s, int x, int y){
	ActionMap *m = &(activeCommandSet->stateStack.back());
	ActionMap::iterator it = m->find(s);
	if(it == m->end())
		return NULL;  
	UIEvent * ev = new UIEvent(s);
	ev->value = it->second; 
	ev->pos = Vector2f(x, y);
	return ev; 
}

void PCInputManager::Keyboard(unsigned char key, int x, int y) {
	UIEvent::Specifier s = UIEvent::Specifier(UIEvent::KEY, (int) key, glutGetModifiers(), true); 
	UIEvent *ev = ProcessEvent(s,x,y); if (ev) Enqueue(ev);	  
}

void PCInputManager::KeyboardUp(unsigned char key, int x, int y) {
	UIEvent::Specifier s = UIEvent::Specifier(UIEvent::KEY, (int) key, glutGetModifiers(), false);
	UIEvent *ev = ProcessEvent(s,x,y); if (ev) Enqueue(ev);	
}

void PCInputManager::SpecialKey(int key, int x, int y) {
	UIEvent::Specifier s = UIEvent::Specifier(UIEvent::SPECIAL_KEY, (int) key, glutGetModifiers(), true);
	UIEvent *ev = ProcessEvent(s,x,y); if (ev) Enqueue(ev);	
}

void PCInputManager::SpecialKeyUp(int key, int x, int y) {
	UIEvent::Specifier s = UIEvent::Specifier(UIEvent::SPECIAL_KEY, (int) key, glutGetModifiers(), false);
	UIEvent *ev = ProcessEvent(s,x,y); if (ev) Enqueue(ev);	
}

void PCInputManager::MouseClick(int button, int state, int x, int y){
	UIEvent::Specifier s = UIEvent::Specifier(UIEvent::CLICK, button, glutGetModifiers(), (state == GLUT_DOWN));
	UIEvent *ev = ProcessEvent(s,x,y); if (ev) Enqueue(ev);	
}

void PCInputManager::MouseMotion(int x, int y){
	UIEvent::Specifier s = UIEvent::Specifier(UIEvent::MOVE, 0, glutGetModifiers(), true);
	UIEvent *ev = ProcessEvent(s, x, y);
	if (!ev)
		return;
	if (oldX >= 0 && oldY>=0){
		ev->delta = Vector2f(x-oldX, y-oldY);
	} else {
		ev->delta = Vector2f(0, 0);
	}
	oldX = x; 
	oldY = y;
	Enqueue(ev); 	
}

void PCInputManager::MousePassiveMotion(int x, int y){
	UIEvent::Specifier s = UIEvent::Specifier(UIEvent::MOVE, 0, glutGetModifiers(), false);
	UIEvent *ev = ProcessEvent(s, x, y);
	if(!ev)
		return;
	if (oldX >= 0 && oldY>=0){
		ev->delta = Vector2f(x-oldX, y-oldY);
	} else {
		ev->delta = Vector2f(0, 0);
	}
	oldX = x; 
	oldY = y; 
	Enqueue(ev);
}


//private queueing functions

void PCInputManager::Enqueue(UIEvent *uiEvent) {
	pthread_mutex_lock(&eventQueue_mutex);
	eventQueue.push(uiEvent);
	pthread_mutex_unlock(&eventQueue_mutex);
}


//Command and Event struct

bool UIEvent::Specifier::operator<(const Specifier& other) {
	if (mType != other.mType)
		mType < other.mType;
	if (aux != other.aux)
		return aux < other.aux;
	if (down != other.down)
		return down;
	if (modifier != other.modifier)
		return modifier < other.modifier;
	return false;  //equal
}
