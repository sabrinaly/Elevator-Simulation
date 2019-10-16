#ifndef __Elevator2Status__
#define __Elevator2Status__

#include "C:\RTExamples\rt.h"

class Elevator2Status {
	struct theData {
		int x;	// the data to be protected
	};

	CDataPool* theDataPool;  // a datapool containing the data to be protected double balance; 
	theData* D_ptr;			// pointer to the data

public:
	Elevator2Status(string Name) {
		// Semaphores to communicate with IO
		CSemaphore ps3("PS3", 0, 1); // semaphore with initial value 0 and max value 1
		CSemaphore cs3("CS3", 1, 1); // semaphore with initial value 1 and max value 1
		// Semaphores to communicate with Dispatcher
		CSemaphore ps4("PS4", 0, 1); // semaphore with initial value 0 and max value 1
		CSemaphore cs4("CS4", 1, 1); // semaphore with initial value 1 and max value 1
		theDataPool = new CDataPool(string("__DataPool__") + string(Name), sizeof(struct theData));
		D_ptr = (struct theData*)(theDataPool->LinkDataPool());
	}

	~Elevator2Status() {}

	//int Get_Elevator_Status() {
	//	return D_ptr->x;
	//}

	void Update_Status(int value) {
		cs3.Wait(); // wait to make sure IO has read the last update
		cs3.Wait(); // wait to make sure Dispatcher has read the last update
		D_ptr->x = value;
		ps4.Signal(); // signal to IO that new data is available
		ps4.Signal(); // signal to Dispatcher that new data is available
	}
};

#endif