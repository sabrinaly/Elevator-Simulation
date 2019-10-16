#ifndef __Elevator2Status__
#define __Elevator2Status__

#include "C:\RTExamples\rt.h"
#include <ElevatorData.h>

class Elevator2Status
{

	elevator_status *elevator2status; // pointer to the data struct

public:
	Elevator2Status(string Name)
	{
		Elevator2StatusDataPool = new CDataPool(string("__DataPool__") + string(Name), sizeof(struct elevator_status));
		elevator2status = (struct elevator_status *)(Elevator2StatusDataPool->LinkDataPool());
	}

	~Elevator2Status() {}

	void Update_Status(int value)
	{
		cs3.Wait(); // wait to make sure IO has read the last update
		cs3.Wait(); // wait to make sure Dispatcher has read the last update
		elevator2status->temp = value;
		ps4.Signal(); // signal to IO that new data is available
		ps4.Signal(); // signal to Dispatcher that new data is available
	}
};

#endif