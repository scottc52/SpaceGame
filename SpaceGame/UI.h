/*******************************************************************************
* FILE: UI.h
* =======================
* This file defines classes to handle input for different UI devices.  Right
* now, I defined a class for PC (mouse & keyboard) input and plan to do the 
* same for XBox.
* -----------------------
* AUTHOR: Stephen Hudson
* LAST UPDATED: 5/13/13
* TODO: 
* >> Implement mouse input
* >> Add options for single-threaded execution
* >> Implement stack functionality to save sets of commands
* >> Fix bug with modifier keys
* >> Add support for XBox controllers on Windows
*******************************************************************************/

#ifndef _UI_H
#define _UI_H

#include "GameState.h"

#ifdef _WIN32
#include <Windows.h>
//#include <XInput.h>
#endif //_WIN32

#include <map>
#include <pthread.h>
#include <queue>
#include <vector>

/*
* CLASS: PCInputManager
* =============================
* DESCRIPTION: Intercepts all glut input notifications, parses them into commands
* and then stores them in an event queue.  The client can then fetch these
* events from the queue (in an arbitrary thread) and execute them.
* The functionality to handle and execute input is static. An instance of this
* class defines a mapping between any command (ex. ALT+F4 or "press left mouse 
* button") and a callback specified by the client. An instance can then be 
* specified as the active one for resolving callbacks for events on the queue, and thus 
* different sets of commands represented by different instances of the class can be 
* swapped out for convenience.
* -------------------------
* USAGE:
* >> call EnableUI()
* >> create an instance of the class
* >> set the desired mappings with the set____Callback() functions
* >> set that instance as the source for callbacks with setActiveCommandSet()
* >> run a loop while !QueueEmpty() and call ExecuteQueuedEvent() on each iteration 
* -------------------------
* FUNCTIONALITY:
* >> Thread-safe event queueing and dequeueing
* >> Stack feature for pushing the current mapping of commands in a particular
*    instance, overwriting those commands as necessary, and popping the stack
*	  to it's original state
* >> Can disable the class to stop collecting input from glut. This can be useful
*    when 
* -------------------------
* NOTES: The only thread-safe feature of this class is the event dequeueing 
* function. If more is necessary, let me know.
*/

#define UI_MIDDLE_BUTTON (GLUT_MIDDLE_BUTTON)
#define UI_LEFT_BUTTON (GLUT_LEFT_BUTTON)
#define UI_RIGHT_BUTTON (GLUT_RIGHT_BUTTON)

#define UI_NO_MODIFIER (0)
#define UI_SHIFT (GLUT_ACTIVE_SHIFT)
#define UI_CTRL (GLUT_ACTIVE_CTRL)   
#define UI_ALT (GLUT_ACTIVE_ALT) 

class UIEvent { 
public:
	enum EventType {KEY, SPECIAL_KEY, MOVE, CLICK}; 
	struct Specifier{ 	
		EventType mType;
		int aux; // specific to type
		int modifier;
		bool down;
		bool operator<(const Specifier &o) const;
		Specifier(EventType t, int a, int m, bool d):
			mType(t), aux(a), modifier(m), down(d) 
		{}
	} specifier;
	UIEvent(Specifier &spc):specifier(spc){}   
	~UIEvent() {}
	int value; 
	Vector2f pos; 
	Vector2f delta;
};

class PCInputManager {

public:
	static void EnableUI();
	static void DisableUI();
	static bool QueueEmpty();
	static UIEvent *PopEvent();
	static void AllPending(list<UIEvent *> &buf);
	static void FlushQueue();

	//Constructor
	PCInputManager();

	void setKeyCallback(int modifier, unsigned char key, bool pressDown, int value);
	void setSpecialKeyCallback(int modifier, int key, bool pressDown /*vs release*/, int value);
	void setMouseButtonCallback(int modifier, int button, bool pressDown, int value);
	void setMouseMoveCallback(int modifier, bool buttonDown /*vs button up*/,int value);
	//void setMouseScrollCallback(int modifier,
	//							void (*callback)(GameState&, bool, int, int));

	void pushState();
	void popState();
	void setActiveCommandSet();

private:
	static void Keyboard(unsigned char key, int x, int y);
	static void KeyboardUp(unsigned char key, int x, int y);
	static void SpecialKey(int key, int x, int y);
	static void SpecialKeyUp(int key, int x, int y);
	static void MouseClick(int button, int state, int x, int y);
	static void MousePassiveMotion(int x, int y);
	static void MouseMotion(int x, int y);
	//static void MouseWheel(int wheel, int direction, int x, int y);
	//enum MouseState {HOVER, DOWN, DOWN_OUT, UN_CAPTURED};

	//static MouseState mouseState; 
	static int oldX, oldY; 
	static PCInputManager *activeCommandSet;

	static std::queue<UIEvent *> eventQueue;
	static pthread_mutex_t eventQueue_mutex;

	static void Enqueue(UIEvent *uiEvent);
	static UIEvent *ProcessEvent(UIEvent::Specifier &sp, int x, int y);
	typedef std::map<UIEvent::Specifier, int> ActionMap; 
	std::vector<ActionMap> stateStack;
};

#ifdef _WIN32
class XBoxInputManager {

};
#endif //_WIN32

#endif //_UI_H
