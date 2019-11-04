// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157
#include "C:\RTExamples\rt.h"
#include <ElevatorData.h>
#include <ElevatorStatus.h>

pipeline_data passengerstruct;
pipeline_data pipeline1struct;

elevator_status E1_status;
elevator_status E2_status;

UINT __stdcall IOStatusElevator1(void *args)
{
	//Direction (up/down), General Status (in/out of service), Door Status (open/closed), Current Floor Number
	ElevatorStatus Elevator1Status("Elevator1");
	r1.Wait();
	for (int i = 0; i < floors; i++)
	{
		E1_status = Elevator1Status.IO_Get_Elevator_Status();
		print.Wait();
		cout << "IO >>> elevator 1 status" << endl
			 << "direction: " << E1_status.direction << endl
			 << "floor: " << E1_status.floor << endl;
		print.Signal();
	}
	r2.Wait();
	return 0;
}

UINT __stdcall IOStatusElevator2(void *args)
{
	ElevatorStatus Elevator2Status("Elevator2");
	r1.Wait();
	for (int i = 0; i < floors; i++)
	{
		elevator_status *dispatcher_elevator2status; // pointer to the data struct
		dispatcher_elevator2status = (struct elevator_status *)(Elevator2StatusDataPool->LinkDataPool());
		E2_status = Elevator2Status.IO_Get_Elevator_Status();
		print.Wait();
		cout << "IO >>> elevator 2 status" << endl
			 << "direction: " << E2_status.direction << endl
			 << "floor: " << E2_status.floor << endl;
		print.Signal();
	}
	r2.Wait();
	return 0;
}

UINT __stdcall ReadPassengerPipeline(void* args)
{
	r1.Wait();
	CPipe passengerPipe("PassengerPipeline", 1024);
	CPipe pipe1("MyPipe", 1024);
	while (1) {
		passengerPipe.Read(&passengerstruct, sizeof(passengerstruct));
		pipelineMutex.Wait();
		pipe1.Write(&passengerstruct, sizeof(passengerstruct));
		pipelineMutex.Signal();
	}
	r2.Wait();
	return 0;
}

int main()
{
	CThread Elevator1(IOStatusElevator1, ACTIVE, NULL);
	CThread Elevator2(IOStatusElevator2, ACTIVE, NULL);
	CThread Passenger(ReadPassengerPipeline, ACTIVE, NULL);
	CPipe pipe1("MyPipe", 1024);

	r1.Wait();

	while (1) {
		if (TEST_FOR_KEYBOARD()) {
			//do we need to count how many passengers entered the elevator, to allow that many inside elevator commands?

			char input1 = _getch();
			char input2 = _getch();
			if (input1 == 'd' && input2 == '+') {}
				// start active passengers
			else if (input1 == 'd' && input2 == '-') {}
				// end active passengers
			else if (input1 == '+' && input2 == '1') {}
				// elevator 1 fault cleared
			else if (input1 == '-' && input2 == '1') {}
				// elevator 1 fault occurred
			else if (input1 == '+' && input2 == '2') {}
				// elevator 2 fault cleared
			else if (input1 == '-' && input2 == '2') {}
				// elevator 2 fault occurred
			else if (input1 == 'e' && input2 == 'e') {}
				// elevators return to floor 0, open doors, end simulation 
			else {
				//if input not valid -- do something
				pipeline1struct = { input1, input2 };
				pipelineMutex.Wait();
				pipe1.Write(&pipeline1struct, sizeof(pipeline1struct));
				pipelineMutex.Signal();
			}


		}
	}

	r2.Wait();

	Elevator1.WaitForThread();
	Elevator2.WaitForThread();
	Passenger.WaitForThread();

	return 0;
}