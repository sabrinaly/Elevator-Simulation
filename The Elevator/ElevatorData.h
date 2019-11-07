// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157

/**

	TODO:
	- max 4 passengers - RAYMOND - IMPLEMENTED
		- not working
	- print status and other debug prints - RAYMOND - WIP
		- only printing elevator status and command rn
	- elevator faults - SABRINA - IMPLEMENTED
		- missing 'ee'
	- activate active objects/mode changes
	- mailbox to end simulation - WIP
		- make doors open when simulation ends
		- post back to IO to indicate end of simulation
	- move mode into status so IO can print the mode

	- Each elevator should freeze its actions immediately upon receipt of a simulated fault condition, and should ignore subsequent commands except a command to end the simulation, or clear the fault.

	- restarting passenger mode
	Debugging:
		- stop not being printed - RESOLVED - cannot put arrays in datapool
 */

#ifndef __ElevatorData__
#define __ElevatorData__

#include "C:\RTExamples\rt.h"
#include <random>

#define COMMAND_SIZE 100

//Elevator Defines
#define DOWN 0
#define UP 1
#define IDLE 2
#define INSIDE 0
#define OUT_UP 1
#define OUT_DOWN 2

//Dispatcher Defines
#define DIS_E1 0
#define DIS_E2 1
#define DIS_OUT_UP 2
#define DIS_OUT_DOWN 3

#define NUM_PASSENGERS 30
#define PASSENGER_COUNT 8

//Faults Defines
#define START_PASSENGERS 93
#define END_PASSENGERS 94
#define END_SIM 95
#define E1_FAULT 96
#define E1_CLEAR 97
#define E2_FAULT 98
#define E2_CLEAR 99

#define MAX_PASSENGERS 4
#define MANUAL_MODE 0
#define ACTIVE_MODE 1

#define NUM_FLOORS 10
#define DOOR_DELAY 2000
#define MOVE_DELAY 1000

struct command
{
	char x;
	char y;
};

struct floor_struct
{
	int stop = 0;
	int passenger_inside = 0;
	int passenger_outside = 0;
};

struct UP_struct
{
	floor_struct s0;
	floor_struct s1;
	floor_struct s2;
	floor_struct s3;
	floor_struct s4;
	floor_struct s5;
	floor_struct s6;
	floor_struct s7;
	floor_struct s8;
	floor_struct s9;
};

struct DOWN_struct
{
	floor_struct s0;
	floor_struct s1;
	floor_struct s2;
	floor_struct s3;
	floor_struct s4;
	floor_struct s5;
	floor_struct s6;
	floor_struct s7;
	floor_struct s8;
	floor_struct s9;
};

struct elevator_status
{
	int floor = 0;		// floor corresponding to lifts current position
	int direction = UP; // direction of travel of lift
	int target_floor = 0;
	int passenger_count = 0;
	int door = 0;
	int fault = 0;
	int changed_floor = 0;
	int req_direction = 0;
	UP_struct UP_array;
	DOWN_struct DOWN_array;
};

struct command_struct
{
	int command = 0;
	int valid = 0;
	int age = 0;
} c;

floor_struct EV1UP_array[10];
floor_struct EV1DOWN_array[10];
floor_struct EV2UP_array[10];
floor_struct EV2DOWN_array[10];

int EV_passenger_count;
int outside_issued_count;

//int mode = ACTIVE_MODE;

int door1 = 0; // 1 = open, 0 = close
int door2 = 0;

int debug = 0;

CMailbox Elevator1Mailbox;
CMailbox Elevator2Mailbox;
CMailbox EndSimMailbox;

CDataPool *Elevator1StatusDataPool;
CDataPool *Elevator2StatusDataPool;

CSemaphore EV1SimFinished("EV1SimFinished", 0, 2); // semaphore with initial value 0 and max value 1
CSemaphore EV2SimFinished("EV2SimFinished", 0, 2); // semaphore with initial value 0 and max value 1

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

CRendezvous r1("StartRendezvous", 12);
CRendezvous r2("EndRendezvous", 4);

CMutex cursor("Cursor");
CMutex passengerPipelineMutex("PassengerMutex");
CMutex pipelineMutex("PipelineMutex");
#endif
