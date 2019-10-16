// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157

#ifndef __ElevatorStatus__
#define __ElevatorStatus__

#include "C:\RTExamples\rt.h"
#include <ElevatorData.h>

class ElevatorStatus
{
	elevator_status *elevator1status; // pointer to the data struct
	elevator_status *elevator2status; // pointer to the data struct
	string instance;

public:
	ElevatorStatus(string Name)
	{
		instance = Name;
		if (Name == "Elevator1")
		{
			Elevator1StatusDataPool = new CDataPool(string("__DataPool__") + string(Name), sizeof(struct elevator_status));
			elevator1status = (struct elevator_status *)(Elevator1StatusDataPool->LinkDataPool());
		}
		else if (Name == "Elevator2")
		{
			Elevator2StatusDataPool = new CDataPool(string("__DataPool__") + string(Name), sizeof(struct elevator_status));
			elevator2status = (struct elevator_status *)(Elevator2StatusDataPool->LinkDataPool());
		}
	}

	~ElevatorStatus() {}

	void Update_Status(elevator_status status)
	{
		if (instance == "Elevator1")
		{
			cs1.Wait(); // wait to make sure IO has read the last update
			cs2.Wait(); // wait to make sure Dispatcher has read the last update
			*elevator1status = status;
			ps1.Signal(); // signal to IO that new data is available
			ps2.Signal(); // signal to Dispatcher that new data is available
		}
		else if (instance == "Elevator2")
		{
			cs3.Wait(); // wait to make sure IO has read the last update
			cs4.Wait(); // wait to make sure Dispatcher has read the last update
			*elevator2status = status;
			ps3.Signal(); // signal to IO that new data is available
			ps4.Signal(); // signal to Dispatcher that new data is available
		}
	}

	elevator_status IO_Get_Elevator_Status()
	{
		if (instance == "Elevator1")
		{
			elevator_status temp_elevator_status;
			ps1.Wait();
			temp_elevator_status = *elevator1status;
			cs1.Signal();
			return temp_elevator_status;
		}
		else if (instance == "Elevator2")
		{
			elevator_status temp_elevator_status;
			ps3.Wait();
			temp_elevator_status = *elevator2status;
			cs3.Signal();
			return temp_elevator_status;
		}
	}

	elevator_status Dispatcher_Get_Elevator_Status()
	{
		if (instance == "Elevator1")
		{
			elevator_status temp_elevator_status;
			ps2.Wait();
			temp_elevator_status = *elevator1status;
			cs2.Signal();
			return temp_elevator_status;
		}
		else if (instance == "Elevator2")
		{
			elevator_status temp_elevator_status;
			ps4.Wait();
			temp_elevator_status = *elevator2status;
			cs4.Signal();
			return temp_elevator_status;
		}
	}
};

#endif