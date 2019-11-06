// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157
#include "C:\RTExamples\rt.h"
#include <ElevatorData.h>
#include <ElevatorStatus.h>

//Function Headers
void print_UP_array(UP_struct);
void print_DOWN_array(DOWN_struct);
void print_door(int);

command passengerstruct;
command pipeline1struct;

elevator_status E1_status;
elevator_status E2_status;

UINT __stdcall IOStatusElevator1(void *args)
{
	//Direction (up/down), General Status (in/out of service), Door Status (open/closed), Current Floor Number
	ElevatorStatus Elevator1Status("Elevator1");

	Sleep(2000);

	cursor.Wait();
	CURSOR_OFF();
	cursor.Signal();

	r1.Wait();
	if (debug)
	{
		cursor.Wait();
		MOVE_CURSOR(0, 30);
		cout << "Elevator1 Status THREAD created" << endl;
		cursor.Signal();
	}
	while (1)
	{
		E1_status = Elevator1Status.IO_Get_Elevator_Status();
		cursor.Wait();
		MOVE_CURSOR(0, 2);
		cout << "------ ELEVATOR 1 STATUS -----" << endl
			 << "direction: " << E1_status.direction << endl
			 << "floor: " << E1_status.floor << endl
			 << "target floor: " << E1_status.target_floor << endl
			 << "passenger count: " << E1_status.passenger_count << endl
			 << "fault: " << E1_status.fault << endl;
		print_door(E1_status.door);
		cout << "... UP ARRAY ..." << endl;
		print_UP_array(E1_status.UP_array);
		cout << "... DOWN ARRAY ..." << endl;
		print_DOWN_array(E1_status.DOWN_array);
		cout.flush();
		cursor.Signal();
	}

	return 0;
}

UINT __stdcall IOStatusElevator2(void *args)
{
	ElevatorStatus Elevator2Status("Elevator2");
	r1.Wait();
	if (debug)
	{
		cursor.Wait();
		MOVE_CURSOR(0, 31);
		cout << "Elevator2 Status THREAD created" << endl;
		cursor.Signal();
	}
	while (1)
	{
		E2_status = Elevator2Status.IO_Get_Elevator_Status();
		cursor.Wait();
		MOVE_CURSOR(0, 19);
		cout << "------ ELEVATOR 2 STATUS -----" << endl
			 << "direction: " << E2_status.direction << endl
			 << "floor: " << E2_status.floor << endl
			 << "target floor: " << E2_status.target_floor << endl
			 << "passenger count: " << E2_status.passenger_count << endl
			 << "fault: " << E2_status.fault << endl;
		print_door(E2_status.door);
		cout << "... UP ARRAY ..." << endl;
		print_UP_array(E2_status.UP_array);
		cout << "... DOWN ARRAY ..." << endl;
		print_DOWN_array(E2_status.DOWN_array);
		cout.flush();
		cursor.Signal();
	}
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

			// if (input1 == '-' && input2 == '1')
			// {
			// 	cursor.Wait();
			// 	MOVE_CURSOR(0, 0);
			// 	cout << "Received command: " << input1 << input2 << endl;
			// 	cursor.Signal();
			// 	/* while (1)
			// 	{
			// 		if (TEST_FOR_KEYBOARD())
			// 		{
			// 			input1 = _getch();
			// 			input2 = _getch();
			// 			if (input1 == '+' && input2 == '1')
			// 			{
			// 				cursor.Wait();
			// 				MOVE_CURSOR(0, 0);
			// 				cout << "Received command: " << input1 << input2 << endl;
			// 				cursor.Signal();
			// 				pipeline1struct = {input1, input2};
			// 				pipelineMutex.Wait();
			// 				dispatcherPipe.Write(&pipeline1struct);
			// 				pipelineMutex.Signal();
			// 				break;
			// 			}
			// 		}
			// 	} */
			// }

			// if (input1 == '-' && input2 == '2')
			// {
			// 	cursor.Wait();
			// 	MOVE_CURSOR(0, 0);
			// 	cout << "Received command: " << input1 << input2 << endl;
			// 	cursor.Signal();
			// 	/* while (1)
			// 	{
			// 		if (TEST_FOR_KEYBOARD())
			// 		{
			// 			input1 = _getch();
			// 			input2 = _getch();
			// 			if (input1 == '+' && input2 == '2')
			// 			{
			// 				pipeline1struct = {input1, input2};
			// 				pipelineMutex.Wait();
			// 				dispatcherPipe.Write(&pipeline1struct);
			// 				pipelineMutex.Signal();
			// 				break;
			// 			}
			// 		}
			// 	} */
			// }

			cursor.Wait();
			MOVE_CURSOR(0, 0);
			cout << "Received command: " << input1 << input2 << endl;
			cursor.Signal();
		}

		if (EndSimMailbox.TestForMessage())
		{
			UINT Message = EndSimMailbox.GetMessage();
			if (Message == END_SIM)
			{
				cursor.Wait();
				MOVE_CURSOR(0, 0);
				cout << "END OF SIMULATION" << endl;
				cursor.Signal();
				break;
			}
		}
	}

	Elevator1.~CThread();
	Elevator2.~CThread();
	Passenger.~CThread();

	Elevator1.WaitForThread();
	Elevator2.WaitForThread();
	Passenger.WaitForThread();

	cursor.Wait();
	CLEAR_SCREEN();
	MOVE_CURSOR(0, 0);
	cout << "threads ended" << endl;
	cout << "END OF SIMULATION2" << endl;
	cout << "Press enter to close simulation ..." << endl;
	cursor.Signal();

	getchar();

	cout << "Waiting for r2" << endl;
	r2.Wait();

	return 0;
}

void print_UP_array(UP_struct floor_array)
{
	// stop: [ 0 0 0 0 ]
	cout << "stop:    [ ";
	cout << floor_array.s0.stop << " ";
	cout << floor_array.s1.stop << " ";
	cout << floor_array.s2.stop << " ";
	cout << floor_array.s3.stop << " ";
	cout << floor_array.s4.stop << " ";
	cout << floor_array.s5.stop << " ";
	cout << floor_array.s6.stop << " ";
	cout << floor_array.s7.stop << " ";
	cout << floor_array.s8.stop << " ";
	cout << floor_array.s9.stop << " ";
	cout << "]" << endl;

	// passenger_inside: [ 0 0 0 0 ]
	cout << "inside:  [ ";
	cout << floor_array.s0.passenger_inside << " ";
	cout << floor_array.s1.passenger_inside << " ";
	cout << floor_array.s2.passenger_inside << " ";
	cout << floor_array.s3.passenger_inside << " ";
	cout << floor_array.s4.passenger_inside << " ";
	cout << floor_array.s5.passenger_inside << " ";
	cout << floor_array.s6.passenger_inside << " ";
	cout << floor_array.s7.passenger_inside << " ";
	cout << floor_array.s8.passenger_inside << " ";
	cout << floor_array.s9.passenger_inside << " ";
	cout << "]" << endl;

	// passenger_outside: [ 0 0 0 0 ]
	cout << "outside: [ ";
	cout << floor_array.s0.passenger_outside << " ";
	cout << floor_array.s1.passenger_outside << " ";
	cout << floor_array.s2.passenger_outside << " ";
	cout << floor_array.s3.passenger_outside << " ";
	cout << floor_array.s4.passenger_outside << " ";
	cout << floor_array.s5.passenger_outside << " ";
	cout << floor_array.s6.passenger_outside << " ";
	cout << floor_array.s7.passenger_outside << " ";
	cout << floor_array.s8.passenger_outside << " ";
	cout << floor_array.s9.passenger_outside << " ";
	cout << "]" << endl;
}

void print_DOWN_array(DOWN_struct floor_array)
{
	// stop: [ 0 0 0 0 ]
	cout << "stop:    [ ";
	cout << floor_array.s0.stop << " ";
	cout << floor_array.s1.stop << " ";
	cout << floor_array.s2.stop << " ";
	cout << floor_array.s3.stop << " ";
	cout << floor_array.s4.stop << " ";
	cout << floor_array.s5.stop << " ";
	cout << floor_array.s6.stop << " ";
	cout << floor_array.s7.stop << " ";
	cout << floor_array.s8.stop << " ";
	cout << floor_array.s9.stop << " ";
	cout << "]" << endl;

	// passenger_inside: [ 0 0 0 0 ]
	cout << "inside:  [ ";
	cout << floor_array.s0.passenger_inside << " ";
	cout << floor_array.s1.passenger_inside << " ";
	cout << floor_array.s2.passenger_inside << " ";
	cout << floor_array.s3.passenger_inside << " ";
	cout << floor_array.s4.passenger_inside << " ";
	cout << floor_array.s5.passenger_inside << " ";
	cout << floor_array.s6.passenger_inside << " ";
	cout << floor_array.s7.passenger_inside << " ";
	cout << floor_array.s8.passenger_inside << " ";
	cout << floor_array.s9.passenger_inside << " ";
	cout << "]" << endl;

	// passenger_outside: [ 0 0 0 0 ]
	cout << "outside: [ ";
	cout << floor_array.s0.passenger_outside << " ";
	cout << floor_array.s1.passenger_outside << " ";
	cout << floor_array.s2.passenger_outside << " ";
	cout << floor_array.s3.passenger_outside << " ";
	cout << floor_array.s4.passenger_outside << " ";
	cout << floor_array.s5.passenger_outside << " ";
	cout << floor_array.s6.passenger_outside << " ";
	cout << floor_array.s7.passenger_outside << " ";
	cout << floor_array.s8.passenger_outside << " ";
	cout << floor_array.s9.passenger_outside << " ";
	cout << "]" << endl;
}

void print_door(int door)
{
	if (door)
		cout << "door status: OPEN" << endl;
	else
		cout << "door status: CLOSED" << endl;
}