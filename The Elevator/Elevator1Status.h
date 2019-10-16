#ifndef __Elevator1Status__
#define __Elevator1Status__

#include "C:\RTExamples\rt.h"
#include <ElevatorData.h>

class Elevator1Status
{
	elevator_status *elevator1status; // pointer to the data struct
public:
	Elevator1Status(string Name)
	{
		Elevator1StatusDataPool = new CDataPool(string("__DataPool__") + string(Name), sizeof(struct elevator_status));
		elevator1status = (struct elevator_status *)(Elevator1StatusDataPool->LinkDataPool());
	}

	~Elevator1Status() {}

	void Update_Status(int value)
	{
		cs1.Wait(); // wait to make sure IO has read the last update
		cs2.Wait(); // wait to make sure Dispatcher has read the last update
		elevator1status->temp = value;
		ps1.Signal(); // signal to IO that new data is available
		ps2.Signal(); // signal to Dispatcher that new data is available
	}

	elevator_status Get_Elevator_Status()
	{
		ps1.Wait();
		cs1.Signal();
	}
};

#endif