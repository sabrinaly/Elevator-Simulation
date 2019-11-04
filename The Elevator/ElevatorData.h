// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157

/**

	TODO:
	- max 4 passengers
	- dispatcher algorithm
	- print status and other debug prints - RAYMOND - WIP
	- elevator faults
	- activate active objects
 */

#ifndef __ElevatorData__
#define __ElevatorData__

#include "C:\RTExamples\rt.h"
#include <random>

#define COMMAND_SIZE 100
#define EV_UP 1
#define EV_DOWN 0
#define REQ_DOWN 1
#define REQ_UP 2
#define NOONE 0
#define SAME_DIR 1
#define OPP_DIR 2

struct mydatapooldata
{					// start of structure template
	int floor;		// floor corresponding to lifts current position
	int direction;  // direction of travel of lift
	int floors[10]; // an array representing the floors and whether requests are set
};

struct pipeline_data
{
	char x;
	char y;
};

struct elevator_status
{
	int floor;	 // floor corresponding to lifts current position
	int direction; // direction of travel of lift
	int target_floor;
};

int floors = 10;
int door1 = 0; // 1 = open, 0 = close
int door2 = 0;
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

CMailbox Elevator1Mailbox;
CMailbox Elevator2Mailbox;

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

CCondition EV1_UP0("EV1_UP0");
CCondition EV1_UP1("EV1_UP1");
CCondition EV1_UP2("EV1_UP2");
CCondition EV1_UP3("EV1_UP3");
CCondition EV1_UP4("EV1_UP4");
CCondition EV1_UP5("EV1_UP5");
CCondition EV1_UP6("EV1_UP6");
CCondition EV1_UP7("EV1_UP7");
CCondition EV1_UP8("EV1_UP8");
CCondition EV1_UP9("EV1_UP9");

CCondition EV1_DW0("EV1_DW0");
CCondition EV1_DW1("EV1_DW1");
CCondition EV1_DW2("EV1_DW2");
CCondition EV1_DW3("EV1_DW3");
CCondition EV1_DW4("EV1_DW4");
CCondition EV1_DW5("EV1_DW5");
CCondition EV1_DW6("EV1_DW6");
CCondition EV1_DW7("EV1_DW7");
CCondition EV1_DW8("EV1_DW8");
CCondition EV1_DW9("EV1_DW9");

CCondition EV2_UP0("EV2_UP0");
CCondition EV2_UP1("EV2_UP1");
CCondition EV2_UP2("EV2_UP2");
CCondition EV2_UP3("EV2_UP3");
CCondition EV2_UP4("EV2_UP4");
CCondition EV2_UP5("EV2_UP5");
CCondition EV2_UP6("EV2_UP6");
CCondition EV2_UP7("EV2_UP7");
CCondition EV2_UP8("EV2_UP8");
CCondition EV2_UP9("EV2_UP9");

CCondition EV2_DW0("EV2_DW0");
CCondition EV2_DW1("EV2_DW1");
CCondition EV2_DW2("EV2_DW2");
CCondition EV2_DW3("EV2_DW3");
CCondition EV2_DW4("EV2_DW4");
CCondition EV2_DW5("EV2_DW5");
CCondition EV2_DW6("EV2_DW6");
CCondition EV2_DW7("EV2_DW7");
CCondition EV2_DW8("EV2_DW8");
CCondition EV2_DW9("EV2_DW9");

CRendezvous r1("StartRendezvous", 8);
CRendezvous r2("EndRendezvous", 8);

CMutex cursor("Cursor");
CMutex passengerPipelineMutex("PassengerMutex");
CMutex pipelineMutex("PipelineMutex");
#endif
