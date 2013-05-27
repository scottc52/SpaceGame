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

GameState *PCInputManager::gameState = NULL;
const PCInputManager *PCInputManager::activeCommandSet = NULL;
queue<PCInputManager::UIEvent *> PCInputManager::eventQueue;
pthread_mutex_t PCInputManager::eventQueue_mutex;


//public functions

void PCInputManager::EnableUI(GameState *gameState) {
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(Keyboard);
	glutSpecialFunc(SpecialKey);
	glutSpecialUpFunc(SpecialKeyUp);
	//glutMouseFunc(MouseClick);
	//glutPassiveMotionFunc(MousePassiveMotion);
	//glutMotionFunc(MouseMotion);
	//glutMouseWheelFunc(MouseWheel);

	PCInputManager::gameState = gameState;
	pthread_mutex_init(&eventQueue_mutex, NULL);
}

void PCInputManager::DisableUI() {
	glutKeyboardFunc(NULL);
	glutKeyboardUpFunc(NULL);
	glutSpecialFunc(NULL);
	glutSpecialUpFunc(NULL);
	//glutMouseFunc(NULL);
	//glutPassiveMotionFunc(NULL);
	//glutMotionFunc(NULL);
	//glutMouseWheelFunc(NULL);

	gameState = NULL;
	activeCommandSet = NULL;
	pthread_mutex_destroy(&eventQueue_mutex);
}

bool PCInputManager::QueueEmpty() {
	pthread_mutex_lock(&eventQueue_mutex);
	bool isEmtpy = eventQueue.empty();
	pthread_mutex_unlock(&eventQueue_mutex);
	return isEmtpy;
}

void PCInputManager::ExecuteQueuedEvent() {
	pthread_mutex_lock(&eventQueue_mutex);
	UIEvent *nextEvent = eventQueue.front();
	eventQueue.pop();
	pthread_mutex_unlock(&eventQueue_mutex);
	(*nextEvent)();
	delete nextEvent;
}

void PCInputManager::ExecutePendingEvents() {
	list<UIEvent *> events;
	pthread_mutex_lock(&eventQueue_mutex);
	while(!eventQueue.empty()){
		events.push_back(eventQueue.front());
		eventQueue.pop(); 
	}
	pthread_mutex_unlock(&eventQueue_mutex);
	list<UIEvent *>::iterator it;
	for (it = events.begin(); it != events.end(); it++){
		UIEvent *ev = *it;
		(*ev)();
		delete ev;  
	}
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
	stateStack.push_back(UIState());
}

void PCInputManager::setKeyCallback(int modifier, unsigned char key, bool pressDown,
						void (*callback)(GameState *)) {
	stateStack.back().keyboardCallbacks[
		KeyPress::Command(modifier, key, pressDown)
	] = callback;
}

void PCInputManager::setSpecialKeyCallback(int modifier, int key, bool pressDown,
										   void (*callback)(GameState *)) {
	stateStack.back().specialKeyboardCallbacks[
		SpecialKeyPress::Command(modifier, key, pressDown)
	] = callback;
}

void PCInputManager::setActiveCommandSet() const {
	activeCommandSet = this;
}


//glut callbacks

void PCInputManager::Keyboard(unsigned char key, int x, int y) {
	//TODO also, when calling the function, get the right one from the stack
	Enqueue(new KeyPress(KeyPress::Command(glutGetModifiers(), key, true)));
}

void PCInputManager::KeyboardUp(unsigned char key, int x, int y) {
	Enqueue(new KeyPress(KeyPress::Command(glutGetModifiers(), key, false)));
}

void PCInputManager::SpecialKey(int key, int x, int y) {
	Enqueue(new SpecialKeyPress(SpecialKeyPress::Command(glutGetModifiers(), key, true)));
}

void PCInputManager::SpecialKeyUp(int key, int x, int y) {
	Enqueue(new SpecialKeyPress(SpecialKeyPress::Command(glutGetModifiers(), key, false)));
}


//private queueing functions

void PCInputManager::Enqueue(UIEvent *uiEvent) {
	pthread_mutex_lock(&eventQueue_mutex);
	eventQueue.push(uiEvent);
	pthread_mutex_unlock(&eventQueue_mutex);
}


//Command and Event structs

bool PCInputManager::KeyPress::Command::operator<(const Command& other) const {
	if(key == other.key) {
		if(pressDown && other.pressDown) {
			return modifier < other.modifier;
		} else
			return pressDown;
	} else
		return key < other.key;
}

void PCInputManager::KeyPress::operator()() const {
	map<Command, Callback>::const_iterator callbackMapping = 
		activeCommandSet->stateStack.back().keyboardCallbacks.find(cmd);
	if(callbackMapping != activeCommandSet->stateStack.back().keyboardCallbacks.end()
	   && callbackMapping->second != NULL)
		callbackMapping->second(gameState);
}

bool PCInputManager::SpecialKeyPress::Command::operator<(const Command& other) const {
	if(key == other.key) {
		if(pressDown && other.pressDown) {
			return modifier < other.modifier;
		} else
			return pressDown;
	} else
		return key < other.key;
}

void PCInputManager::SpecialKeyPress::operator()() const {
	map<Command, Callback>::const_iterator callbackMapping = 
		activeCommandSet->stateStack.back().specialKeyboardCallbacks.find(cmd);
	if(callbackMapping != activeCommandSet->stateStack.back().specialKeyboardCallbacks.end()
	   && callbackMapping->second != NULL)
		callbackMapping->second(gameState);
}
