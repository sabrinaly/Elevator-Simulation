// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157
struct mydatapooldata
{					// start of structure template
	int floor;		// floor corresponding to lifts current position
	int direction;  // direction of travel of lift
	int floors[10]; // an array representing the floors and whether requests are set
};

struct mypipelinedata
{
	int x;
	int y;
};

struct elevator_status
{
	int temp;
}

/* struct dispatcher_status
{
	int temp; // the data to be protected
}; */

/* struct IO_status
{
	int temp; // the data to be protected
}; */

/* //Elevator 1 Dispatcher Data
CDataPool *DispatcherElevator1DataPool;
dispatcher_status *dispatcher_E1_status;

//Elevator 2 Dispatcher Data
CDataPool *DispatcherElevator2DataPool;
dispatcher_status *dispatcher_E2_status;

//Elevator 1 IO Data
CDataPool *IOElevator1DataPool;
IO_status *IO_E1_status;

//Elevator 2 IO Data
CDataPool *IOElevator2DataPool;
IO_status *IO_E2_status; */

CDataPool *Elevator1StatusDataPool;
CDataPool *Elevator2StatusDataPool;

// ---- Semaphores ---- //
// Elevator 1
// Semaphores to communicate with IO
CSemaphore ps1("PS1", 0, 1); // semaphore with initial value 0 and max value 1
CSemaphore cs1("CS1", 1, 1); // semaphore with initial value 1 and max value 1
// Semaphores to communicate with Dispatcher
CSemaphore ps2("PS2", 0, 1); // semaphore with initial value 0 and max value 1
CSemaphore cs2("CS2", 1, 1); // semaphore with initial value 1 and max value 1

//Elevator 2
// Semaphores to communicate with IO
CSemaphore ps3("PS3", 0, 1); // semaphore with initial value 0 and max value 1
CSemaphore cs3("CS3", 1, 1); // semaphore with initial value 1 and max value 1
// Semaphores to communicate with Dispatcher
CSemaphore ps4("PS4", 0, 1); // semaphore with initial value 0 and max value 1
CSemaphore cs4("CS4", 1, 1); // semaphore with initial value 1 and max value 1

CRendezvous r1("StartRendezvous", 8);
CRendezvous r2("EndRendezvous", 8);
