// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157
#include "C:\RTExamples\rt.h"
#include <ElevatorData.h>
#include <ElevatorStatus.h>
#include <Passengers.h>
#define CURSOR_Y 35

//Function Headers
void print_UP_array(UP_struct);
void print_DOWN_array(DOWN_struct);
void print_door(int);
void print_elevator_base();
void print_elevator_move(int, int, int);

struct passenger_struct
{
	Passengers *passenger;
	int valid = 0;
};

int create_pass_flag = 0;

command passengerstruct;
command pipeline1struct;

passenger_struct passenger_array[NUM_PASSENGERS];

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
		print_elevator_move(1, E1_status.floor, E1_status.passenger_count);
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
		print_elevator_move(2, E2_status.floor, E2_status.passenger_count);
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
		MOVE_CURSOR(0, 0);
		cout << "Received passenger command: " << passengerstruct.x << passengerstruct.y << endl;
		cursor.Signal();
	}
	return 0;
}

UINT __stdcall CreatePassenger(void *args)
{
	int passenger_count = 0;
	while (1)
	{
		while (create_pass_flag == 1)
		{

			//find space in array
			for (int i = 0; i < NUM_PASSENGERS; i++)
			{
				if (passenger_array[i].valid == 0 && passenger_count <= PASSENGER_COUNT)
				{
					passenger_array[i].passenger = new Passengers;
					passenger_array[i].passenger->Resume();
					passenger_array[i].valid = 1;
					passenger_count++;
					break;
				}
			}
			random_device rd;
			mt19937 eng(rd());
			uniform_int_distribution<> distr(3100, 3500);
			Sleep(distr(eng));
		}

		if (create_pass_flag == 2)
		{

			for (int i = 0; i < NUM_PASSENGERS; i++)
			{
				if (passenger_array[i].valid == 1)
				{
					delete passenger_array[i].passenger;
					passenger_array[i].valid = 0;
				}
			}
		}
	}
	return 0;
}

int main()
{
	CThread Elevator1(IOStatusElevator1, ACTIVE, NULL);
	CThread Elevator2(IOStatusElevator2, ACTIVE, NULL);
	CThread Passenger(ReadPassengerPipeline, ACTIVE, NULL);
	CThread InitializePassenger(CreatePassenger, ACTIVE, NULL);
	CTypedPipe<command> dispatcherPipe("DispatcherPipeline", 1024);

	r1.Wait();

	cursor.Wait();
	MOVE_CURSOR(0, CURSOR_Y);
	print_elevator_base();
	cursor.Signal();

	Passengers p1, p2, p3, p4, p5;

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

			if (input1 == 'd' && input2 == '+')
			{
				create_pass_flag = 1;
			}
			else if (input1 == 'd' && input2 == '-')
			{
				create_pass_flag = 2;
			}

			cursor.Wait();
			MOVE_CURSOR(0, 0);
			cout << "Received command: " << input1 << input2 << "          \n";
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

	/* Elevator1.~CThread();
	Elevator2.~CThread();
	Passenger.~CThread();

	Elevator1.WaitForThread();
	Elevator2.WaitForThread();
	Passenger.WaitForThread();
 */
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

void print_elevator_base()
{
	cout << "   ELEVATOR 1                      ELEVATOR 2" << endl;
	cout << "  _____________                   _____________" << endl;
	cout << "9|             |                9|             |" << endl;
	cout << "  _____________                   _____________" << endl;
	cout << "8|             |                8|             |" << endl;
	cout << "  _____________                   _____________" << endl;
	cout << "7|             |                7|             |" << endl;
	cout << "  _____________                   _____________" << endl;
	cout << "6|             |                6|             |" << endl;
	cout << "  _____________                   _____________" << endl;
	cout << "5|             |                5|             |" << endl;
	cout << "  _____________                   _____________" << endl;
	cout << "4|             |                4|             |" << endl;
	cout << "  _____________                   _____________" << endl;
	cout << "3|             |                3|             |" << endl;
	cout << "  _____________                   _____________" << endl;
	cout << "2|             |                2|             |" << endl;
	cout << "  _____________                   _____________" << endl;
	cout << "1|             |                1|             |" << endl;
	cout << "  _____________                   _____________" << endl;
	cout << "0|             |                0|             |" << endl;
	cout << "  _____________                   _____________" << endl;
}

void print_elevator_move(int elevator, int floor, int num)
{
	// clear the floors above/below, print num on new floor
	// num = E1_status.passenger_count or E2_status.passenger_count
	int cursor_spot = 0;
	if (elevator == 1)
		cursor_spot = 8;
	else
		cursor_spot = 40;
	cursor.Wait();
	if (floor == 9)
	{
		MOVE_CURSOR(cursor_spot, 4 + CURSOR_Y);
		cout << "  ";
		MOVE_CURSOR(cursor_spot, 2 + CURSOR_Y);
	}
	else if (floor == 8)
	{
		MOVE_CURSOR(cursor_spot, 2 + CURSOR_Y);
		cout << "  ";
		MOVE_CURSOR(cursor_spot, 6 + CURSOR_Y);
		cout << "  ";
		MOVE_CURSOR(cursor_spot, 4 + CURSOR_Y);
	}
	else if (floor == 7)
	{
		MOVE_CURSOR(cursor_spot, 4 + CURSOR_Y);
		cout << "  ";
		MOVE_CURSOR(cursor_spot, 8 + CURSOR_Y);
		cout << "  ";
		MOVE_CURSOR(cursor_spot, 6 + CURSOR_Y);
	}
	else if (floor == 6)
	{
		MOVE_CURSOR(cursor_spot, 6 + CURSOR_Y);
		cout << "  ";
		MOVE_CURSOR(cursor_spot, 10 + CURSOR_Y);
		cout << "  ";
		MOVE_CURSOR(cursor_spot, 8 + CURSOR_Y);
	}
	else if (floor == 5)
	{
		MOVE_CURSOR(cursor_spot, 8 + CURSOR_Y);
		cout << "  ";
		MOVE_CURSOR(cursor_spot, 12 + CURSOR_Y);
		cout << "  ";
		MOVE_CURSOR(cursor_spot, 10 + CURSOR_Y);
	}
	else if (floor == 4)
	{
		MOVE_CURSOR(cursor_spot, 10 + CURSOR_Y);
		cout << "  ";
		MOVE_CURSOR(cursor_spot, 14 + CURSOR_Y);
		cout << "  ";
		MOVE_CURSOR(cursor_spot, 12 + CURSOR_Y);
	}
	else if (floor == 3)
	{
		MOVE_CURSOR(cursor_spot, 12 + CURSOR_Y);
		cout << "  ";
		MOVE_CURSOR(cursor_spot, 16 + CURSOR_Y);
		cout << "  ";
		MOVE_CURSOR(cursor_spot, 14 + CURSOR_Y);
	}
	else if (floor == 2)
	{
		MOVE_CURSOR(cursor_spot, 14 + CURSOR_Y);
		cout << "  ";
		MOVE_CURSOR(cursor_spot, 18 + CURSOR_Y);
		cout << "  ";
		MOVE_CURSOR(cursor_spot, 16 + CURSOR_Y);
	}
	else if (floor == 1)
	{
		MOVE_CURSOR(cursor_spot, 16 + CURSOR_Y);
		cout << "  ";
		MOVE_CURSOR(cursor_spot, 20 + CURSOR_Y);
		cout << "  ";
		MOVE_CURSOR(cursor_spot, 18 + CURSOR_Y);
	}
	else if (floor == 0)
	{
		MOVE_CURSOR(cursor_spot, 18 + CURSOR_Y);
		cout << "  ";
		MOVE_CURSOR(cursor_spot, 20 + CURSOR_Y);
	}
	cout << num;
	cursor.Signal();
}