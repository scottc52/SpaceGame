/*GameRoom.h*/

class GameRoom{
	private:
	/*Private variables*/ 

	/*Private functions*/
	public:
	/*public variables*/
	
	/*public functions*/
	
	/* 	Loads a room object from a file in the room format 
		described in documentation. */ 
	static void load_room(char *fname, GameRoom &room);

	/* 	Writes a file in the specified room format 
		see documentation for further details*/ 
	static void write_room(char *fname, GameRoom &room);
};

