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
int last_requested_floor = NULL;
struct command_struct
{
	int command = 0;
	int valid = 0;
	int age = 0;
} c;
command_struct *commands = new command_struct[100];

UINT __stdcall ReadMailbox(void *args)
{
	r1.Wait();
	int i = 0;
	// initialize status: floor 0, going up, target floor 0
	status = {0, 1, 0};
	Elevator1Status.Update_Status(status);
	// suspends until you get the first message. doesnt matter if up or down
	Message = Elevator1Mailbox.GetMessage();
	last_requested_floor = Message % 10; // what if this starts as 0?
	status = {0, 1, last_requested_floor};
	Elevator1Status.Update_Status(status);

	while (1)
	{
		if (Elevator1Mailbox.TestForMessage())
		{
			Message = Elevator1Mailbox.GetMessage();
			// increase the age of all valid commands
			for (i = 0; i < COMMAND_SIZE; i++)
			{
				if (commands[i].valid)
				{
					commands[i].age++;
				}
			}
			// if message is from inside elevator (0-9) and the request exceeds the current targeted floor, set as new targeted floor
			if ((Message < last_requested_floor && elevator_direction == 0) || (Message <= 9 && Message > last_requested_floor && elevator_direction))
			{
				// smallest valid age is 1
				c = {last_requested_floor, 1, 1};
				last_requested_floor = Message;
				for (i = 0; i < COMMAND_SIZE; i++)
				{
					if (commands[i].valid == 0)
					{
						commands[i] = c;
						break;
					}
				}
			}
			// if message is from outside elevator or command not greater than last_requested_floor, save the request
			else
			{
				c = {Message, 1, 1};
				for (i = 0; i < COMMAND_SIZE; i++)
				{
					if (commands[i].valid == 0)
					{
						commands[i] = c;
						break;
					}
				}
			}
		}
	}
	r2.Wait();
}

int main()
{
	CThread t1(ReadMailbox, ACTIVE, NULL);
	r1.Wait();
	int i = 0;
	int stopped_flag = 0;

	while (1)
	{
		/*********  ELEVATOR GOING UP  **********/

		while (elevator_floor < last_requested_floor) //do we need to check for elevator_direction too??
		{
			for (i = 0; i < COMMAND_SIZE; i++)
			{
				// if passenger getting off (command/10 = 0) or getting on to go down (command/10 = 1)
				if (commands[i].valid && (elevator_floor == commands[i].command % 10) && (commands[i].command / 10 != 2))
					stopped_flag = 1;
			}
			if (stopped_flag)
			{
				stop_elevator(REQ_DOWN, EV_UP);
			}
			stopped_flag = 0;
			Sleep(1000);
			elevator_floor++;
			elevator_direction = 1;
			update_status();
		}

		/*********  ELEVATOR GOING DOWN  **********/

		while (elevator_floor > last_requested_floor)
		{
			for (i = 0; i < COMMAND_SIZE; i++)
			{
				// if passenger getting off (command/10 = 0) or getting on to go down (command/10 = 2)
				if (commands[i].valid && (elevator_floor == commands[i].command % 10) && (commands[i].command / 10 != 1))
					stopped_flag = 1;
			}
			if (stopped_flag)
			{
				stop_elevator(REQ_DOWN, EV_DOWN);
			}
			stopped_flag = 0;
			Sleep(1000);
			elevator_floor--;
			elevator_direction = 0;
			update_status();
		}

		/**================================================== *
		 * ==========  Elevator at target floor  ============ *
		 * ================================================== */

		int passenger_waiting = NOONE; // 0 = no one waiting, 1 = waiting and going same direction, 2 = waiting and going opposite direction
		if (elevator_floor == last_requested_floor)
		{
			for (i = 0; i < COMMAND_SIZE; i++)
			{
				if (commands[i].valid && (elevator_floor == commands[i].command % 10))
				{
					// passenger(s) waiting on this floor, going same direction
					// when they enter, last requested floor will be greater than the current one
					// new last_request_floor will be set in the other thread
					if ((elevator_direction && commands[i].command / 10) || (elevator_direction == 0 && commands[i].command / 10 == 2))
					{
						passenger_waiting = SAME_DIR;
						break;
					}
					// passenger waiting on this floor, but requested the opposite direction
					else if ((elevator_direction && commands[i].command / 10 == 2) || (elevator_direction == 0 && commands[i].command / 10))
					{
						//still need passengers to get off, bottom part still needs to work
						// after we need to signal the opposite direction
						passenger_waiting = OPP_DIR;
					}
				}
			}

			/*********  NO PASSENGERS  **********/

			// if no passengers are waiting at this floor, then we set a new last_requested_floor
			int largest_age = 0;
			int largest_age_index = 0;
			if (passenger_waiting == NOONE)
			{
				// opens door to let passengers out first 
				if (elevator_direction)
					stop_elevator(NOONE, EV_UP);
				else
					stop_elevator(NOONE, EV_DOWN);
				//poll for new command
				while (largest_age == 0)
				{
					for (i = 0; i < COMMAND_SIZE; i++)
					{
						if (commands[i].valid && commands[i].age > largest_age)
						{
							largest_age = commands[i].age;
							largest_age_index = i;
						}
					}
					// if largest age is 0, that means there are no valid commands in the list and elevator should be idle
					// should it go back to middle floor?
					if (largest_age != 0)
					{
						last_requested_floor = commands[largest_age_index].command % 10;
						commands[largest_age_index].valid = 0;
						if (last_requested_floor > elevator_floor)
						{
							elevator_direction = 1;
							Sleep(1000);
							elevator_floor++;
							update_status();
						}
						else if (last_requested_floor < elevator_floor)
						{
							elevator_direction = 0;
							Sleep(1000);
							elevator_floor--;
							update_status();
						}
						// what if last_requested_floor == elevator_floor 
						// after a while, passenger arrives on the same floor
						// open the door for them, then go to the direction they want 
						else 
						{
							// passenger arrives on elevator's floor, presses up
							if (commands[largest_age_index].command / 10) 
							{
								elevator_direction = 1;
								stop_elevator(REQ_UP, EV_UP);
								Sleep(1000);
								elevator_floor++;
								update_status();
							}
							else // command/10 == 2
							{
								elevator_direction = 0;
								stop_elevator(REQ_DOWN, EV_DOWN);
								Sleep(1000);
								elevator_floor--;
								update_status();
							}
						}
					}
				}
			}

			/*********  PASSENGER GOING SAME DIR  **********/

			else if (passenger_waiting == SAME_DIR)
			{
				if (elevator_direction)
				{ // elevator went up, passenger requesting up
					stop_elevator(REQ_UP, EV_UP);
					Sleep(1000);
					elevator_floor++;
					update_status();
				}
				else
				{ // elevator went down, passenger requesting down
					stop_elevator(REQ_DOWN, EV_DOWN);
					Sleep(1000);
					elevator_floor--;
					update_status();
				}
			}

			/*********  PASSENGER GOING OPP DIR  **********/

			else if (passenger_waiting == OPP_DIR)
			{ // passenger_waiting == 2
				if (elevator_direction)
				{ // elevator went up, passenger requesting down
					elevator_direction = 0;
					EV1_UP_SIGNAL(); // for passengers getting off
					EV1_UP_RESET();
					//elevator going down now and passengers getting on to go down
					stop_elevator(REQ_DOWN, EV_DOWN);
					Sleep(1000);
					elevator_floor--;
					update_status();
				}
				else
				{ // elevator went down, passenger requesting up
					elevator_direction = 1;
					EV1_DW_SIGNAL(); // for passengers getting off
					EV1_DW_RESET();
					//elevator going up now and passengers getting on to go up
					stop_elevator(REQ_UP, EV_UP);
					Sleep(1000);
					elevator_floor++;
					update_status();
				}
			}
		}
	}

	//delete[] commands;  // When done, free memory
	//commands = NULL;

	r2.Wait();
	t1.WaitForThread();

	return 0;
}

//clears command that have been consumed (e.g passenger got off, passenger got on)
void clear_command(int requested_direction)
{
	if (requested_direction == 0) {
		for (int i = 0; i < COMMAND_SIZE; i++)
		{
			if (commands[i].valid && (elevator_floor == commands[i].command % 10) && (commands[i].command / 10 == requested_direction))
			{
				commands[i].valid = 0;
			}
		}
	}
	else {
		for (int i = 0; i < COMMAND_SIZE; i++)
		{
			if (commands[i].valid && (elevator_floor == commands[i].command % 10) && (commands[i].command / 10 != requested_direction))
			{
				commands[i].valid = 0;
			}
		}
	}
}

//called whenever elevator stops to let passengers off or pick up passengers
void stop_elevator(int requested_direction, int elevator_direction)
{
	if (elevator_direction == EV_UP)
	{
		open_door();
		EV1_UP_SIGNAL();
		Sleep(2000);
		clear_command(requested_direction);
		EV1_UP_RESET();
		close_door();
	}
	else if (elevator_direction == EV_DOWN)
	{
		open_door();
		EV1_DW_SIGNAL();
		Sleep(2000);
		clear_command(requested_direction);
		EV1_DW_RESET();
		close_door();
	}
}

void open_door()
{
	door1 = 1;
	cursor.Wait();
	cout << "ElEVATOR 1 DOOR OPENED" << endl;
	cursor.Signal();
}

void close_door()
{
	door1 = 0;
	cursor.Wait();
	cout << "ElEVATOR 1 DOOR CLOSED" << endl;
	cursor.Signal();
}

void update_status()
{
	status = {elevator_floor, elevator_direction, last_requested_floor};
	Elevator1Status.Update_Status(status);
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