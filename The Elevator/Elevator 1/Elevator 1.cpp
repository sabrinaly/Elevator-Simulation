// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157
#include "C:\RTExamples\rt.h"
#include <ElevatorData.h>
#include <ElevatorStatus.h>

ElevatorStatus Elevator1Status("Elevator1");
elevator_status status;
UINT Message;
int elevator_floor = 0;
int elevator_direction = 1; // 1 = up, 0 = down
int target_floor = 0;

command_struct *commands = new command_struct[100];

UINT __stdcall Elevator1Move(void *args)
{
	r1.Wait();
	while (1)
	{

		/*********  ELEVATOR GOING UP  **********/
		while (elevator_floor < target_floor)
		{
			if (EV1UP_array[elevator_floor].stop)
			{
				stop_elevator(UP);
			}
			Sleep(MOVE_DELAY);
			elevator_floor++;
			elevator_direction = UP;
			update_status();
		}
		/*********  ELEVATOR GOING DOWN  **********/
		while (elevator_floor > target_floor)
		{
			if (EV1DOWN_array[elevator_floor].stop)
			{
				stop_elevator(DOWN);
			}
			Sleep(MOVE_DELAY);
			elevator_floor--;
			elevator_direction = DOWN;
			update_status();
		}
		/*********  ELEVATOR REACHED TARGET FLOOR  **********/
		if (elevator_floor == target_floor)
		{
			while (check_empty_array() == 0)
			{
				// do nothing
			}
			if (elevator_direction == UP)
			{
				//Update counters
				EV_passenger_count = EV_passenger_count - EV1UP_array[target_floor].passenger_inside + EV1UP_array[target_floor].passenger_outside;
				outside_issued_count -= EV1UP_array[target_floor].passenger_outside;
				//stop elevator
				stop_elevator(UP);
			}
			else if (elevator_direction == DOWN)
			{
				//Update counters
				EV_passenger_count = EV_passenger_count - EV1DOWN_array[target_floor].passenger_inside + EV1DOWN_array[target_floor].passenger_outside;
				outside_issued_count -= EV1DOWN_array[target_floor].passenger_outside;
				//stop elevator
				stop_elevator(DOWN);
			}
			update_status();
		}
	}

	r2.Wait();
}

int main()
{
	CThread t1(Elevator1Move, ACTIVE, NULL);
	r1.Wait();
	int i = 0;
	int stopped_flag = 0;

	int i = 0;
	// initialize status: floor 0, going up, target floor 0
	update_status();

	/**================================================== *
	 * ==========  Listen for Commands  ========== *
	 * ================================================== */

	while (1)
	{

		Message = Elevator1Mailbox.GetMessage();
		int command_type = Message / 10;
		int req_floor = Message % 10;

		/*********  Populate Elevator Array  **********/
		if (Message == E1_FAULT)
		{
			clear_floor_array();
			target_floor = elevator_floor; // doing nothing in other thread
										   // next message has to be clearing fault, dealt with in IO
		}
		if (Message == END_SIM)
		{
			clear_floor_array();
			target_floor = 0;
		}
		if (elevator_floor == target_floor)
		{
			if (command_type == INSIDE && req_floor > elevator_floor)
			{
				EV1UP_array[req_floor].stop = 1;
				EV1UP_array[req_floor].passenger_inside++;
			}
			else if (command_type == INSIDE && req_floor < elevator_floor)
			{
				EV1DOWN_array[req_floor].stop = 1;
				EV1DOWN_array[req_floor].passenger_inside++;
			}
			else if (req_floor > elevator_floor)
			{
				EV1UP_array[req_floor].stop = 1;
				EV1UP_array[req_floor].passenger_outside++;
			}
			else if (req_floor < elevator_floor)
			{
				EV1DOWN_array[req_floor].stop = 1;
				EV1DOWN_array[req_floor].passenger_outside++;
			}
			else if (req_floor == elevator_floor)
			{
				// does it matter which array we populate? look @ other thread
				elevator_direction = UP;
				EV1UP_array[req_floor].stop = 1;
				EV1UP_array[req_floor].passenger_outside++;
			}
			target_floor = req_floor;
			update_status();
		}
		//if passenger is inside
		else if (command_type == INSIDE)
		{
			if (elevator_direction == UP)
			{
				EV1UP_array[req_floor].stop = 1;
				EV1UP_array[req_floor].passenger_inside++;
			}
			else if (elevator_direction == DOWN)
			{
				EV1DOWN_array[req_floor].stop = 1;
				EV1DOWN_array[req_floor].passenger_inside++;
			}
		}
		//if passenger is outside and requesting to go up
		else if (command_type == OUT_UP)
		{
			EV1UP_array[req_floor].stop = 1;
			EV1UP_array[req_floor].passenger_outside++;
		}
		//if passenger is outside and requesting to go down
		else if (command_type == OUT_DOWN)
		{
			EV1DOWN_array[req_floor].stop = 1;
			EV1DOWN_array[req_floor].passenger_outside++;
		}
		// if message is from inside elevator (0-9) and the request exceeds the current targeted floor, set as new targeted floor
		if ((Message < target_floor && elevator_direction == DOWN) || (Message <= 9 && Message > target_floor && elevator_direction == UP))
		{
			target_floor = Message;
			update_status();
		}
	}
	/* =======  End of Listen for Commands  ======= */

	r2.Wait();
	t1.WaitForThread();

	return 0;
}

void stop_elevator(int elevator_direction)
{
	if (elevator_direction == UP)
	{
		open_door();
		EV1_UP_SIGNAL();
		Sleep(DOOR_DELAY);
		EV1_UP_RESET();
		close_door();
		//clear floor struct
		EV1UP_array[target_floor].stop = 0;
		EV1UP_array[target_floor].passenger_inside = 0;
		EV1UP_array[target_floor].passenger_outside = 0;
	}
	else if (elevator_direction == DOWN)
	{
		open_door();
		EV1_DW_SIGNAL();
		Sleep(DOOR_DELAY);
		EV1_DW_RESET();
		close_door();
		//clear floor struct
		EV1DOWN_array[target_floor].stop = 0;
		EV1DOWN_array[target_floor].passenger_inside = 0;
		EV1DOWN_array[target_floor].passenger_outside = 0;
	}
}

void open_door()
{
	door1 = 1;
	update_status();
	cursor.Wait();
	//cout << "ElEVATOR 1 DOOR OPENED" << endl;
	cursor.Signal();
}

void close_door()
{
	door1 = 0;
	update_status();
	cursor.Wait();
	//cout << "ElEVATOR 1 DOOR CLOSED" << endl;
	cursor.Signal();
}

void update_status()
{
	status = {elevator_floor, elevator_direction, target_floor, EV_passenger_count, door1, EV1UP_array, EV1DOWN_array};
	Elevator1Status.Update_Status(status);
}

int check_empty_array()
{
	int stop = 0;
	for (int i = 0; i < NUM_FLOORS; i++)
	{
		if (EV1UP_array[i].stop || EV1DOWN_array[i].stop)
		{
			stop = 1;
			break;
		}
	}
	return stop;
}

void clear_floor_array()
{
	for (int i = 0; i < NUM_FLOORS, i++)
	{
		EV1UP_array[i].stop = 0;
		EV1UP_array[i].passenger_inside = 0;
		EV1UP_array[i].passenger_outside = 0;
		EV1DOWN_array[i].stop = 0;
		EV1DOWN_array[i].passenger_inside = 0;
		EV1DOWN_array[i].passenger_outside = 0;
	}
}

void EV1_UP_SIGNAL()
{
	if (elevator_floor == 0)
		EV1_UP0.Signal();
	else if (elevator_floor == 1)
		EV1_UP1.Signal();
	else if (elevator_floor == 2)
		EV1_UP2.Signal();
	else if (elevator_floor == 3)
		EV1_UP3.Signal();
	else if (elevator_floor == 4)
		EV1_UP4.Signal();
	else if (elevator_floor == 5)
		EV1_UP5.Signal();
	else if (elevator_floor == 6)
		EV1_UP6.Signal();
	else if (elevator_floor == 7)
		EV1_UP7.Signal();
	else if (elevator_floor == 8)
		EV1_UP8.Signal();
	else if (elevator_floor == 9)
		EV1_UP9.Signal();
}

void EV1_DW_SIGNAL()
{
	if (elevator_floor == 0)
		EV1_DW0.Signal();
	else if (elevator_floor == 1)
		EV1_DW1.Signal();
	else if (elevator_floor == 2)
		EV1_DW2.Signal();
	else if (elevator_floor == 3)
		EV1_DW3.Signal();
	else if (elevator_floor == 4)
		EV1_DW4.Signal();
	else if (elevator_floor == 5)
		EV1_DW5.Signal();
	else if (elevator_floor == 6)
		EV1_DW6.Signal();
	else if (elevator_floor == 7)
		EV1_DW7.Signal();
	else if (elevator_floor == 8)
		EV1_DW8.Signal();
	else if (elevator_floor == 9)
		EV1_DW9.Signal();
}

void EV1_UP_RESET()
{
	if (elevator_floor == 0)
		EV1_UP0.Reset();
	else if (elevator_floor == 1)
		EV1_UP1.Reset();
	else if (elevator_floor == 2)
		EV1_UP2.Reset();
	else if (elevator_floor == 3)
		EV1_UP3.Reset();
	else if (elevator_floor == 4)
		EV1_UP4.Reset();
	else if (elevator_floor == 5)
		EV1_UP5.Reset();
	else if (elevator_floor == 6)
		EV1_UP6.Reset();
	else if (elevator_floor == 7)
		EV1_UP7.Reset();
	else if (elevator_floor == 8)
		EV1_UP8.Reset();
	else if (elevator_floor == 9)
		EV1_UP9.Reset();
}

void EV1_DW_RESET()
{
	if (elevator_floor == 0)
		EV1_DW0.Reset();
	else if (elevator_floor == 1)
		EV1_DW1.Reset();
	else if (elevator_floor == 2)
		EV1_DW2.Reset();
	else if (elevator_floor == 3)
		EV1_DW3.Reset();
	else if (elevator_floor == 4)
		EV1_DW4.Reset();
	else if (elevator_floor == 5)
		EV1_DW5.Reset();
	else if (elevator_floor == 6)
		EV1_DW6.Reset();
	else if (elevator_floor == 7)
		EV1_DW7.Reset();
	else if (elevator_floor == 8)
		EV1_DW8.Reset();
	else if (elevator_floor == 9)
		EV1_DW9.Reset();
}