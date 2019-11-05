// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157
#include "C:\RTExamples\rt.h"
#include <ElevatorData.h>
#include <ElevatorStatus.h>

command mystruct;

elevator_status E1_status;
elevator_status E2_status;

command_struct *command_array = new command_struct[COMMAND_SIZE];

UINT __stdcall DispatcherStatusElevator1(void *args)
{
	ElevatorStatus Elevator1Status("Elevator1");
	r1.Wait();
	while (1)
	{
		E1_status = Elevator1Status.Dispatcher_Get_Elevator_Status();
	}

	r2.Wait();
	return 0;
}

UINT __stdcall DispatcherStatusElevator2(void *args)
{
	ElevatorStatus Elevator2Status("Elevator2");
	r1.Wait();
	while (1)
	{
		E2_status = Elevator2Status.Dispatcher_Get_Elevator_Status();
	}

	r2.Wait();
	return 0;
}

/**================================================== *
 * ==========  READ PIPELINE  ========== *
 * ================================================== */

UINT __stdcall ReadPipeline(void *args)
{
	r1.Wait();
	CTypedPipe<command> dispatcherPipe("DispatcherPipeline", 1024); // pipeline from IO with keyboard command_array
	while (1)
	{
		dispatcherPipe.Read(&mystruct);

		int command_floor = mystruct.y - '0';
		// start active passengers
		if (mystruct.x == 'd' && mystruct.y == '+')
		{
		}
		// end active passengers
		else if (mystruct.x == 'd' && mystruct.y == '-')
		{
		}
		// elevator 1 fault occurred
		else if (mystruct.x == '-' && mystruct.y == '1')
			command_array[COMMAND_SIZE - 1] = {E1_FAULT, 2, 0};
		// elevator 1 fault cleared
		else if (mystruct.x == '+' && mystruct.y == '1')
			command_array[COMMAND_SIZE - 1] = {E1_CLEAR, 2, 0};
		// elevator 2 fault occurred
		else if (mystruct.x == '-' && mystruct.y == '2')
			command_array[COMMAND_SIZE - 1] = {E2_FAULT, 2, 0};
		// elevator 2 fault cleared
		else if (mystruct.x == '+' && mystruct.y == '2')
			command_array[COMMAND_SIZE - 1] = {E2_CLEAR, 2, 0};
		// elevators return to floor 0, open doors, end simulation
		else if (mystruct.x == 'e' && mystruct.y == 'e')
			command_array[COMMAND_SIZE - 1] = {END_SIM, 2, 0};

		// 0-9 = E1 inside, 10-19 = E2 inside, 20-29 = outside up, 30-39 = outside down
		else if (mystruct.x == '1')
		{
			if (command_floor >= 0 && command_floor <= 9)
				save_command(command_floor);
		}
		else if (mystruct.x == '2')
		{
			if (command_floor >= 0 && command_floor <= 9)
				save_command(10 + command_floor);
		}
		else if (mystruct.x == 'u')
		{
			if (command_floor >= 0 && command_floor <= 9)
				save_command(20 + command_floor);
		}
		else if (mystruct.x == 'd')
		{
			if (command_floor >= 0 && command_floor <= 9)
				save_command(30 + command_floor);
		}
	}

	r2.Wait();
	return 0;
}
/* =======  End of READ PIPELINE  ======= */

int main()
{

	CProcess Elevator1("C:\\Users\\Sabrina Ly\\Documents\\Year4\\CPEN 333\\CPEN333-The-Elevator\\The Elevator\\Debug\\Elevator 1.exe", // pathlist to child program executable
					   NORMAL_PRIORITY_CLASS,																						   // priority
					   OWN_WINDOW,																									   // process has its own window
					   ACTIVE																										   // process is active immediately
	);

	CProcess Elevator2("C:\\Users\\Sabrina Ly\\Documents\\Year4\\CPEN 333\\CPEN333-The-Elevator\\The Elevator\\Debug\\Elevator 2.exe", // pathlist to child program executable
					   NORMAL_PRIORITY_CLASS,																						   // priority
					   OWN_WINDOW,																									   // process has its own window
					   ACTIVE																										   // process is active immediately
	);

	CProcess IO("C:\\Users\\Sabrina Ly\\Documents\\Year4\\CPEN 333\\CPEN333-The-Elevator\\The Elevator\\Debug\\IO.exe", // pathlist to child program executable	plus some arguments
				NORMAL_PRIORITY_CLASS,																					// priority
				OWN_WINDOW,																								// process has its own window
				ACTIVE);

	CThread Elevator1Status(DispatcherStatusElevator1, ACTIVE, NULL);
	CThread Elevator2Status(DispatcherStatusElevator2, ACTIVE, NULL);
	CThread ReadPipeline(ReadPipeline, ACTIVE, NULL);

	r1.Wait();
	int Message = NULL;
	int command_floor = NULL;
	int command_type = NULL;

	while (1)
	{

		/**================================================== *
		 * ==========   Dispatcher  ========== *
		 * ================================================== */

		// 0-9 = E1 inside, 10-19 = E2 inside, 20-29 = outside up, 30-39 = outside down
		for (int i = 0; i < COMMAND_SIZE; i++)
		{

			/**================================================== *
			 * ==========  FAULTS  ========== *
			 * ================================================== */

			// TODO: what if passengers are stuck inside during fault
			if (command_array[COMMAND_SIZE - 1].command == E1_FAULT)
			{
				Elevator1.Post(command_array[COMMAND_SIZE - 1].command);
				//clear array
				delete[] command_array;
				command_struct *command_array = new command_struct[COMMAND_SIZE];
				while (command_array[COMMAND_SIZE - 1].command != E1_CLEAR)
				{
				}
				//Elevator1.Post(command_array[COMMAND_SIZE - 1].command);
				command_array[COMMAND_SIZE - 1].command = 0;
			}
			else if (command_array[COMMAND_SIZE - 1].command == E2_FAULT)
			{
				Elevator2.Post(command_array[COMMAND_SIZE - 1].command);
				//clear array
				delete[] command_array;
				command_struct *command_array = new command_struct[COMMAND_SIZE];
				while (command_array[COMMAND_SIZE - 1].command != E2_CLEAR)
				{
				}
				//Elevator2.Post(command_array[COMMAND_SIZE - 1].command);
				command_array[COMMAND_SIZE - 1].command = 0;
			}
			else if (command_array[COMMAND_SIZE - 1].command == END_SIM)
			{
				Elevator1.Post(command_array[COMMAND_SIZE - 1].command);
				Elevator2.Post(command_array[COMMAND_SIZE - 1].command);
			}

			/* =======  End of FAULTS  ======= */

			command_type = command_array[i].command / 10;
			command_floor = command_array[i].command % 10;

			/**================================================== *
			 * ==========  Outside Elevator, Up Input  ========== *
			 * ================================================== */

			if (command_type == DIS_OUT_UP)
			{
				Message = 10 + command_floor; // 10-19 for up
				// if passenger waiting outside is on the way, send command to elevator
				// for start of simulation, it will only post directly if command_floor == 0, else it will save in array

				if (E1_status.direction && E1_status.floor <= command_floor && E1_status.target_floor >= command_floor)
				{
					//if both elevator on the way then check which one is closer
					if (E2_status.direction && E2_status.floor <= command_floor && E2_status.target_floor >= command_floor)
					{
						//elevator 1 closer - post to E1
						if ((command_floor - E1_status.floor) < (command_floor - E2_status.floor))
						{
							if (mode == MANUAL_MODE || check_max_passenger(command_floor, DIS_E1))
							{
								command_array[i].valid = 0;
								Elevator1.Post(Message);
							}
						}
						//elevator 2 closer
						else
						{
							if (mode == MANUAL_MODE || check_max_passenger(command_floor, DIS_E2))
							{
								command_array[i].valid = 0;
								Elevator2.Post(Message);
							}
						}
					}
					// only elevator 1 on the way - post to elevator 1
					else
					{
						if (mode == MANUAL_MODE || check_max_passenger(command_floor, DIS_E1))
						{
							command_array[i].valid = 0;
							Elevator1.Post(Message);
						}
					}
				}
				// only elevator 2 on the way - post to E2
				else if (E2_status.direction && E2_status.floor <= command_floor && E2_status.target_floor >= command_floor)
				{
					if (mode == MANUAL_MODE || check_max_passenger(command_floor, DIS_E2))
					{
						command_array[i].valid = 0;
						Elevator2.Post(Message);
					}
				}
				// not on the way, leave command in array
			}

			/* =======  End of Outside Elevator, UP Input  ======= */

			/**================================================== *
			 * ==========  Outside Elevator, Down Input  ========== *
			 * ================================================== */

			else if (command_type == DIS_OUT_DOWN)
			{
				Message = 20 + command_floor; // 20-29 for down
				if (E1_status.direction == DOWN && E1_status.floor >= command_floor && E1_status.target_floor <= command_floor)
				{
					//if both elevator on the way then check which one is closer
					if (E2_status.direction == DOWN && E2_status.floor >= command_floor && E2_status.target_floor <= command_floor)
					{
						//elevator 1 closer - post to E1
						if ((E1_status.floor - command_floor) < (E2_status.floor - command_floor))
						{
							if (mode == MANUAL_MODE || check_max_passenger(command_floor, DIS_E1))
							{
								command_array[i].valid = 0;
								Elevator1.Post(Message);
							}
						}
						//elevator 2 closer
						else
						{
							if (mode == MANUAL_MODE || check_max_passenger(command_floor, DIS_E2))
							{
								command_array[i].valid = 0;
								Elevator2.Post(Message);
							}
						}
					}
					// only elevator 1 on the way - post to elevator 1
					else
					{
						if (mode == MANUAL_MODE || check_max_passenger(command_floor, DIS_E1))
						{
							command_array[i].valid = 0;
							Elevator1.Post(Message);
						}
					}
				}
				// only elevator 2 on the way - post to E2
				else if (E2_status.direction == DOWN && E2_status.floor >= command_floor && E2_status.target_floor <= command_floor)
				{
					if (mode == MANUAL_MODE || check_max_passenger(command_floor, DIS_E2))
					{
						command_array[i].valid = 0;
						Elevator2.Post(Message);
					}
				}
				// not on the way, leave command in array
			}

			/* =======  End of Outside Elevator, Down Input  ======= */

			/**================================================== *
			 * ==========  Inside Elevator  ========== *
			 * ================================================== */

			else if ((command_type == DIS_E1) || (command_type == DIS_E2))
			{
				if (command_type == DIS_E1)
				{
					command_array[i].valid = 0;
					Elevator1.Post(command_floor);
				}
				else
				{
					command_array[i].valid = 0;
					Elevator2.Post(command_floor);
				}
			}
			else
			{
				// wrong command, print not valid
				cursor.Wait();
				cout << "INVALID COMMAND" << endl;
				cursor.Signal();
			}
		}

		/* =======  End of Inside Elevator  ======= */

		/**================================================== *
		 * ==========  Section EV Reached Target Floor  ========== *
		 * ================================================== */

		// if elevator reached target floor, issue new command from array
		// TODO: ?? will there be passengers waiting on this floor
		int largest_age_index = 0;
		int largest_age = 0;
		if (E1_status.target_floor == E1_status.floor)
		{
			while (largest_age == 0)
			{
				largest_age_index = find_largest_age_index();
				largest_age = command_array[largest_age_index].age;
				if (largest_age != 0)
				{
					command_array[largest_age_index].valid = 0;
					Elevator1.Post(command_array[largest_age_index].command);
				}
			}
		}
		else if (E2_status.target_floor == E2_status.floor)
		{
			while (largest_age == 0)
			{
				largest_age_index = find_largest_age_index();
				largest_age = command_array[largest_age_index].age;
				if (largest_age != 0)
				{
					command_array[largest_age_index].valid = 0;
					Elevator2.Post(command_array[largest_age_index].command);
				}
			}
		}
		/* =======  End of EV Reached Target Floor  ======= */
	}

	/* =======  End of Dispatcher  ======= */

	r2.Wait();

	Elevator1Status.WaitForThread();
	Elevator2Status.WaitForThread();
	ReadPipeline.WaitForThread();
	Elevator1.WaitForProcess();
	Elevator2.WaitForProcess();
	IO.WaitForProcess();

	return 0;
}

void save_command(int command)
{
	// increase the age of all valid command_array
	for (int i = 0; i < COMMAND_SIZE; i++)
	{
		if (command_array[i].valid)
			command_array[i].age++;
	}
	c = {command, 1, 1};
	// save the command in the next nonvalid spot
	for (int i = 0; i < COMMAND_SIZE; i++)
	{
		if (command_array[i].valid == 0)
		{
			command_array[i] = c;
			break;
		}
	}
}

int find_largest_age_index()
{
	int largest_age = 0;
	int largest_age_index = 0;

	for (int i = 0; i < COMMAND_SIZE; i++)
	{
		if (command_array[i].valid && command_array[i].age > largest_age)
		{
			largest_age = command_array[i].age;
			largest_age_index = i;
		}
	}
	return largest_age_index;
}

int check_max_passenger(int req_floor, int elevator_num)
{
	int passenger_inside_count = 0;
	int passenger_outside_count = 0;
	if (elevator_num = DIS_E1)
	{
		if (E1_status.direction == UP)
		{
			for (int i = E1_status.floor; i < req_floor; i++)
			{
				passenger_inside_count += E1_status.UP_array[i].passenger_inside;
				passenger_outside_count += E1_status.UP_array[i].passenger_outside;
			}
			if ((E1_status.passenger_count + passenger_outside_count - passenger_inside_count) < MAX_PASSENGERS)
				return 1;
			else
				return 0;
		}
		else if (E1_status.direction == DOWN)
		{
			for (int i = E1_status.floor; i > req_floor; i--)
			{
				passenger_inside_count += E1_status.DOWN_array[i].passenger_inside;
				passenger_outside_count += E1_status.DOWN_array[i].passenger_outside;
			}
			if ((E1_status.passenger_count + passenger_outside_count - passenger_inside_count) < MAX_PASSENGERS)
				return 1;
			else
				return 0;
		}
	}
	else if (elevator_num = DIS_E2)
	{
		if (E2_status.direction == UP)
		{
			for (int i = E2_status.floor; i < req_floor; i++)
			{
				passenger_inside_count += E2_status.UP_array[i].passenger_inside;
				passenger_outside_count += E2_status.UP_array[i].passenger_outside;
			}
			if ((E2_status.passenger_count + passenger_outside_count - passenger_inside_count) < MAX_PASSENGERS)
				return 1;
			else
				return 0;
		}
		else if (E2_status.direction == DOWN)
		{
			for (int i = E2_status.floor; i > req_floor; i--)
			{
				passenger_inside_count += E2_status.DOWN_array[i].passenger_inside;
				passenger_outside_count += E2_status.DOWN_array[i].passenger_outside;
			}
			if ((E2_status.passenger_count + passenger_outside_count - passenger_inside_count) < MAX_PASSENGERS)
				return 1;
			else
				return 0;
		}
	}
}

// 0-9 = E1 inside, 10-19 = E2 inside, 20-29 = outside up, 30-39 = outside down

// E1, going up
/*
		if (E1_status.floor < E1_status.target_floor)
		{
			for (int i = 0; i < COMMAND_SIZE; i++)
			{
				if (command_array[i].valid && (command_array[i].command % 10 > E1_status.floor) && (command_array[i].command % 10 <= E1_status.target_floor) && (command_type == DIS_E1))
				{
					command_array[i].valid = 0;
					Elevator1.Post(command_array[i].command % 10);
				}
				else if (command_array[i].valid && (command_array[i].command % 10 > E1_status.floor) && (command_array[i].command % 10 <= E1_status.target_floor) && (command_type == DIS_OUT_UP))
				{
					command_array[i].valid = 0;
					Elevator1.Post(10 + (command_array[i].command % 10));
				}
			}
		}

		// E1, going down
		if (E1_status.floor > E1_status.target_floor)
		{
			for (int i = 0; i < COMMAND_SIZE; i++)
			{
				if (command_array[i].valid && (command_array[i].command % 10 < E1_status.floor) && (command_array[i].command % 10 >= E1_status.target_floor) && (command_type == DIS_E1))
				{
					command_array[i].valid = 0;
					Elevator1.Post(command_array[i].command % 10);
				}
				else if (command_array[i].valid && (command_array[i].command % 10 < E1_status.floor) && (command_array[i].command % 10 >= E1_status.target_floor) && (command_type == DIS_OUT_DOWN))
				{
					command_array[i].valid = 0;
					Elevator1.Post(20 + (command_array[i].command % 10));
				}
			}
		}

		// E2, going up
		if (E2_status.floor < E2_status.target_floor)
		{
			for (int i = 0; i < COMMAND_SIZE; i++)
			{
				if (command_array[i].valid && (command_array[i].command % 10 > E2_status.floor) && (command_array[i].command % 10 <= E2_status.target_floor) && (command_type == DIS_E2))
				{
					command_array[i].valid = 0;
					Elevator2.Post(command_array[i].command % 10);
				}
				else if (command_array[i].valid && (command_array[i].command % 10 > E2_status.floor) && (command_array[i].command % 10 <= E2_status.target_floor) && (command_type == DIS_OUT_UP))
				{
					command_array[i].valid = 0;
					Elevator2.Post(10 + (command_array[i].command % 10));
				}
			}
		}

		// E2, going down
		if (E2_status.floor > E2_status.target_floor)
		{
			for (int i = 0; i < COMMAND_SIZE; i++)
			{
				if (command_array[i].valid && (command_array[i].command % 10 < E2_status.floor) && (command_array[i].command % 10 >= E2_status.target_floor) && (command_type == DIS_E2))
				{
					command_array[i].valid = 0;
					Elevator2.Post(command_array[i].command % 10);
				}
				else if (command_array[i].valid && (command_array[i].command % 10 < E2_status.floor) && (command_array[i].command % 10 >= E2_status.target_floor) && (command_type == DIS_OUT_DOWN))
				{
					command_array[i].valid = 0;
					Elevator2.Post(20 + (command_array[i].command % 10));
				}
			}
		}

		int largest_age_index = 0;
		int largest_age = 0;
		if (E1_status.target_floor == E1_status.floor)
		{
			while (largest_age == 0)
			{
				largest_age_index = find_largest_age_index();
				largest_age = command_array[largest_age_index].age;
				if (largest_age != 0)
				{
					command_array[largest_age_index].valid = 0;
					Elevator1.Post(command_array[largest_age_index].command);
				}
			}
		}
		else if (E2_status.target_floor == E2_status.floor)
		{
			while (largest_age == 0)
			{
				largest_age_index = find_largest_age_index();
				largest_age = command_array[largest_age_index].age;
				if (largest_age != 0)
				{
					command_array[largest_age_index].valid = 0;
					Elevator2.Post(command_array[largest_age_index].command);
				}
			}
		}
		*/