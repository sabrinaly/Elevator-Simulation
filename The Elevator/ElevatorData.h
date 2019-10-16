// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157
struct 	    mydatapooldata {		// start of structure template
	int floor;				// floor corresponding to lifts current position
	int direction;			// direction of travel of lift
	int floors[10];			// an array representing the floors and whether requests are set 
};

struct mypipelinedata {
	int x;
	int y;
};

CRendezvous r1("StartRendezvous", 4);
CRendezvous r2("EndRendezvous", 4);
