#ifndef __Elevator1Status__
#define __Elevator1Status__

#include "C:\RTExamples\rt.h"

class Elevator1Status {
	struct theData {
		int x;	// the data to be protected
	};

	CDataPool* theDataPool;  // a datapool containing the data to be protected double balance; 
	theData* D_ptr;			// pointer to the data

public:
	Elevator1Status(string Name) {
		// Semaphores to communicate with IO
		CSemaphore ps1("PS1", 0, 1); // semaphore with initial value 0 and max value 1
		CSemaphore cs1("CS1", 1, 1); // semaphore with initial value 1 and max value 1
		// Semaphores to communicate with Dispatcher
		CSemaphore ps2("PS2", 0, 1); // semaphore with initial value 0 and max value 1
		CSemaphore cs2("CS2", 1, 1); // semaphore with initial value 1 and max value 1
		theDataPool = new CDataPool(string("__DataPool__") + string(Name), sizeof(struct theData));
		D_ptr = (struct theData*)(theDataPool->LinkDataPool());
	}

	~Elevator1Status() {}

	//int Get_Elevator_Status() {
	//	return D_ptr->x;
	//}

	void Update_Status(int value) {
		cs1.Wait(); // wait to make sure IO has read the last update
		cs2.Wait(); // wait to make sure Dispatcher has read the last update
		D_ptr->x = value;
		ps1.Signal(); // signal to IO that new data is available
		ps2.Signal(); // signal to Dispatcher that new data is available
	}
};

#endif