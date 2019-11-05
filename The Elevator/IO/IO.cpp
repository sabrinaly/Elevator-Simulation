// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157
#include "C:\RTExamples\rt.h"
#include <ElevatorData.h>
#include <ElevatorStatus.h>

//Function Headers
void print_floor_array(floor_struct);

command passengerstruct;
command pipeline1struct;

elevator_status E1_status;
elevator_status E2_status;

UINT __stdcall IOStatusElevator1(void *args)
{
	//Direction (up/down), General Status (in/out of service), Door Status (open/closed), Current Floor Number
	ElevatorStatus Elevator1Status("Elevator1");
	r1.Wait();
	while (1)
	{
		E1_status = Elevator1Status.IO_Get_Elevator_Status();
		cursor.Wait();
		MOVE_CURSOR(2, 0);
		cout << "------ ELEVATOR 1 STATUS -----" << endl
			 << "direction: " << E1_status.direction << endl
			 << "floor: " << E1_status.floor << endl
			 << "target floor: " << E1_status.target_floor << endl
			 << "passenger count: " << E1_status.passenger_count << endl
			 << "door status: " << E1_status.door << endl;
		cout << "... UP ARRAY ..." << endl;
		print_floor_array(*E1_status.UP_array);
		cout << "... DOWN ARRAY ..." << endl;
		print_floor_array(*E1_status.DOWN_array);
		cursor.Signal();
	}

	r2.Wait();
	return 0;
}

UINT __stdcall IOStatusElevator2(void *args)
{
	ElevatorStatus Elevator2Status("Elevator2");
	r1.Wait();
	while (1)
	{
		elevator_status *dispatcher_elevator2status; // pointer to the data struct
		dispatcher_elevator2status = (struct elevator_status *)(Elevator2StatusDataPool->LinkDataPool());
		E2_status = Elevator2Status.IO_Get_Elevator_Status();
		cursor.Wait();
		MOVE_CURSOR(19, 0);
		cout << "------ ELEVATOR 2 STATUS -----" << endl
			 << "direction: " << E2_status.direction << endl
			 << "floor: " << E2_status.floor << endl
			 << "target floor: " << E2_status.target_floor << endl
			 << "passenger count: " << E2_status.passenger_count << endl
			 << "door status: " << E2_status.door << endl;
		cout << "... UP ARRAY ..." << endl;
		print_floor_array(*E2_status.UP_array);
		cout << "... DOWN ARRAY ..." << endl;
		print_floor_array(*E2_status.DOWN_array);
		cursor.Signal();
	}
	r2.Wait();
	return 0;
}

UINT __stdcall ReadPassengerPipeline(void *args)
{
	r1.Wait();
	CTypedPipe<command> passengerPipe("PassengerPipeline", 1024);
	CTypedPipe<command> dispatcherPipe("DispatcherPipeline", 1024);
	while (1)
	{
		passengerPipe.Read(&passengerstruct);
		pipelineMutex.Wait();
		dispatcherPipe.Write(&passengerstruct);
		pipelineMutex.Signal();
		cursor.Wait();
		cout << "Received passenger command: " << passengerstruct.x << passengerstruct.y << endl;
		cursor.Signal();
	}
	r2.Wait();
	return 0;
}

int main()
{
	CThread Elevator1(IOStatusElevator1, ACTIVE, NULL);
	CThread Elevator2(IOStatusElevator2, ACTIVE, NULL);
	CThread Passenger(ReadPassengerPipeline, ACTIVE, NULL);
	CTypedPipe<command> dispatcherPipe("DispatcherPipeline", 1024);

	r1.Wait();

	while (1)
	{
		if (TEST_FOR_KEYBOARD())
		{
			//do we need to count how many passengers entered the elevator, to allow that many inside elevator commands?

			char input1 = _getch();
			char input2 = _getch();

			//if input not valid -- do something
			pipeline1struct = {input1, input2};
			pipelineMutex.Wait();
			dispatcherPipe.Write(&pipeline1struct);
			pipelineMutex.Signal();

			if (input1 == '-' && input2 == '1')
			{
				while (1)
				{
					if (TEST_FOR_KEYBOARD())
					{
						input1 = _getch();
						input2 = _getch();
						if (input1 == '+' && input2 == '1')
						{
							pipeline1struct = {input1, input2};
							pipelineMutex.Wait();
							dispatcherPipe.Write(&pipeline1struct);
							pipelineMutex.Signal();
							break;
						}
					}
				}
			}

			if (input1 == '-' && input2 == '2')
			{
				while (1)
				{
					if (TEST_FOR_KEYBOARD())
					{
						input1 = _getch();
						input2 = _getch();
						if (input1 == '+' && input2 == '2')
						{
							pipeline1struct = {input1, input2};
							pipelineMutex.Wait();
							dispatcherPipe.Write(&pipeline1struct);
							pipelineMutex.Signal();
							break;
						}
					}
				}
			}

			cursor.Wait();
			MOVE_CURSOR(0, 0);
			cout << "Received command: " << input1 << input2 << endl;
			cursor.Signal();
		}
	}

	r2.Wait();

	Elevator1.WaitForThread();
	Elevator2.WaitForThread();
	Passenger.WaitForThread();

	return 0;
}

void print_floor_array(floor_struct floor_array[10])
{
	// stop: [ 0 0 0 0 ]
	cout << "stop: [ ";
	for (int i = 0; i < NUM_FLOORS; i++)
	{
		cout << floor_array[i].stop << " ";
	}
	cout << "]" << endl;

	// passenger_inside: [ 0 0 0 0 ]
	cout << "inside: [ ";
	for (int i = 0; i < NUM_FLOORS; i++)
	{
		cout << floor_array[i].passenger_inside << " ";
	}
	cout << "]" << endl;

	// passenger_outside: [ 0 0 0 0 ]
	cout << "outside: [ ";
	for (int i = 0; i < NUM_FLOORS; i++)
	{
		cout << floor_array[i].passenger_outside << " ";
	}
	cout << "]" << endl;
}