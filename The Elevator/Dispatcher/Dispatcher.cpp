// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157
#include "C:\RTExamples\rt.h"
#include <ElevatorData.h>
#include <ElevatorStatus.h>

//Function Headers
int check_max_passenger(int, int);
void increment_passenger(int, int, int);
int find_largest_age_index(int);
int find_closest_index(int, int);
void save_command(int);
void empty_command_array();

command mystruct;

elevator_status E1_status;
elevator_status E2_status;

int passenger_inside_count = 0;
int passenger_outside_count = 0;

command_struct *command_array = new command_struct[COMMAND_SIZE];

UINT __stdcall DispatcherStatusElevator1(void *args)
{
	ElevatorStatus Elevator1Status("Elevator1");
	r1.Wait();
	while (1)
	{
		E1_status = Elevator1Status.Dispatcher_Get_Elevator_Status();
	}

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
		if (E1_status.fault && E2_status.fault)
		{
			if (mystruct.x == '+' && mystruct.y == '1')
				command_array[COMMAND_SIZE - 1] = {E1_CLEAR, 2, 0};
			else if (mystruct.x == '+' && mystruct.y == '2')
				command_array[COMMAND_SIZE - 1] = {E2_CLEAR, 2, 0};
			else
			{
				//do nothing, don't save in array
			}
		}
		else if (E1_status.fault && mystruct.x == '1')
		{
			// do nothing, don't save in array
		}
		else if (E2_status.fault && mystruct.x == '2')
		{
			// do nothing, don't save in array
		}
		else if (mystruct.x == 'd' && mystruct.y == '+')
		{
		}
		// end active passengers
		else if (mystruct.x == 'd' && mystruct.y == '-')
		{
		}
		// elevator 1 fault occurred
		else if (mystruct.x == '-' && mystruct.y == '1')
		{
			command_array[COMMAND_SIZE - 1] = {E1_FAULT, 2, 0};
		}
		// elevator 1 fault cleared
		else if (mystruct.x == '+' && mystruct.y == '1')
		{
			command_array[COMMAND_SIZE - 1] = {E1_CLEAR, 2, 0};
		}
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

	return 0;
}
/* =======  End of READ PIPELINE  ======= */

int main()
{

	CProcess Elevator1("C:\\Users\\sfron\\OneDrive\\School\\UBC 4th Year\\CPEN333\\Labs\\CPEN333-The-Elevator\\The Elevator\\x64\\Debug\\Elevator 1.exe", // pathlist to child program executable
					   NORMAL_PRIORITY_CLASS,																											  // priority
					   OWN_WINDOW,																														  // process has its own window
					   ACTIVE																															  // process is active immediately
	);

	CProcess Elevator2("C:\\Users\\sfron\\OneDrive\\School\\UBC 4th Year\\CPEN333\\Labs\\CPEN333-The-Elevator\\The Elevator\\x64\\Debug\\Elevator 2.exe", // pathlist to child program executable
					   NORMAL_PRIORITY_CLASS,																											  // priority
					   OWN_WINDOW,																														  // process has its own window
					   ACTIVE																															  // process is active immediately
	);

	CProcess IO("C:\\Users\\sfron\\OneDrive\\School\\UBC 4th Year\\CPEN333\\Labs\\CPEN333-The-Elevator\\The Elevator\\x64\\Debug\\IO.exe", // pathlist to child program executable	plus some arguments
				NORMAL_PRIORITY_CLASS,																									   // priority
				OWN_WINDOW,																												   // process has its own window
				ACTIVE);

	CThread Elevator1Status(DispatcherStatusElevator1, ACTIVE, NULL);
	CThread Elevator2Status(DispatcherStatusElevator2, ACTIVE, NULL);
	CThread ReadPipeline(ReadPipeline, ACTIVE, NULL);

	r1.Wait();
	int Message = NULL;
	int command_floor = NULL;
	int command_type = NULL;
	int end_sim = 0;

	/**================================================== *
	 * ==========   Dispatcher  ========== *
	 * ================================================== */

	while (1)
	{

		/**================================================== *
		 * ==========   Command Search  ========== *
		 * ================================================== */

		// 0-9 = E1 inside, 10-19 = E2 inside, 20-29 = outside up, 30-39 = outside down
		for (int i = 0; i < COMMAND_SIZE; i++)
		{
			if (command_array[i].valid == 0)
				continue;

			command_type = command_array[i].command / 10;
			command_floor = command_array[i].command % 10;

			command_type = command_array[i].command / 10;
			command_floor = command_array[i].command % 10;

			/**================================================== *
			 * ==========  FAULTS  ========== *
			 * ================================================== */

			// TODO: what if passengers are stuck inside during fault
			// faults are stored in index 99
			if (command_array[COMMAND_SIZE - 1].command == E1_FAULT && E1_status.fault == 0)
			{
				Elevator1.Post(command_array[COMMAND_SIZE - 1].command);
				// cout << "Posting Fault to EV1" << endl;
				//clear array
				empty_command_array();
			}
			else if (command_array[COMMAND_SIZE - 1].command == E2_FAULT && E2_status.fault == 0)
			{
				// cout << "EV2 POST0" << endl;
				Elevator2.Post(command_array[COMMAND_SIZE - 1].command);
				// cout << "Posting Fault to EV2" << endl;
				//clear array
				empty_command_array();
			}
			else if (command_array[COMMAND_SIZE - 1].command == E1_CLEAR)
			{
				Elevator1.Post(command_array[COMMAND_SIZE - 1].command);
				command_array[COMMAND_SIZE - 1].command = 0;
			}
			else if (command_array[COMMAND_SIZE - 1].command == E2_CLEAR)
			{
				// cout << "EV2 POST1" << endl;
				Elevator2.Post(command_array[COMMAND_SIZE - 1].command);
				command_array[COMMAND_SIZE - 1].command = 0;
			}
			else if (command_array[COMMAND_SIZE - 1].command == END_SIM)
			{
				Elevator1.Post(command_array[COMMAND_SIZE - 1].command);
				// cout << "EV2 POST2" << endl;
				Elevator2.Post(command_array[COMMAND_SIZE - 1].command);
				end_sim = 1;
				// cout << "RECEIVIED END SIM" << endl;
				break;
			}

			/* =======  End of FAULTS  ======= */
			else if (E1_status.fault && E2_status.fault)
			{
				// do nothing
			}
			else if (E1_status.fault && command_array[i].valid == 1)
			{
				// EV1 fault, E2 going up, command on the way to E2
				if (E2_status.direction && E2_status.floor <= command_floor && E2_status.target_floor >= command_floor)
				{
					if (mode == MANUAL_MODE || check_max_passenger(command_floor, DIS_E2))
					{
						command_array[i].valid = 0;
						if (command_type == DIS_E2)
						{
							// cout << "EV2 POST3" << endl;
							Elevator2.Post(command_floor);
						}
						else
						{
							// cout << "EV2 POST4" << i << command_array[i].command << endl;
							Elevator2.Post(command_array[i].command - 10);
						}
					}
				}
				// EV1 fault, E2 going down, command on the way to E2
				else if (E2_status.direction == DOWN && E2_status.floor >= command_floor && E2_status.target_floor <= command_floor)
				{
					if (mode == MANUAL_MODE || check_max_passenger(command_floor, DIS_E2))
					{
						command_array[i].valid = 0;
						if (command_type == DIS_E2)
						{
							// cout << "EV2 POST5" << endl;
							Elevator2.Post(command_floor);
						}
						else
						{
							// cout << "EV2 POST6" << endl;
							Elevator2.Post(command_array[i].command - 10);
						}
					}
				}
				// EV1 fault, E2 idle
				else if (E2_status.floor == E2_status.target_floor)
				{
					command_array[i].valid = 0;
					if (command_type == DIS_E2)
					{
						// cout << "EV2 POST7" << endl;
						Elevator2.Post(command_floor);
					}
					else
					{
						// cout << "EV2 POST8" << endl;
						Elevator2.Post(command_array[i].command - 10);
					}
				}
				// else leave in array
			}

			/*********  Elevator 2 Fault  **********/

			else if (E2_status.fault && command_array[i].valid == 1)
			{
				// EV2 fault, E1 going up, command on the way to E1
				if (E1_status.direction && E1_status.floor <= command_floor && E1_status.target_floor >= command_floor)
				{
					if (mode == MANUAL_MODE || check_max_passenger(command_floor, DIS_E1))
					{
						command_array[i].valid = 0;
						if (command_type == DIS_E1)
						{
							// cout << "EV1 POST3" << endl;
							Elevator1.Post(command_floor);
						}
						else
						{
							// cout << "EV1 POST4" << i << command_array[i].command << endl;
							Elevator1.Post(command_array[i].command - 10);
						}
					}
				}
				// EV2 fault, E1 going down, command on the way to E1
				else if (E1_status.direction == DOWN && E1_status.floor >= command_floor && E1_status.target_floor <= command_floor)
				{
					if (mode == MANUAL_MODE || check_max_passenger(command_floor, DIS_E1))
					{
						command_array[i].valid = 0;
						if (command_type == DIS_E1)
						{
							// cout << "EV1 POST5" << endl;
							Elevator1.Post(command_floor);
						}
						else
						{
							// cout << "EV1 POST6" << endl;
							Elevator1.Post(command_array[i].command - 10);
						}
					}
				}
				// EV2 fault, E1 idle
				else if (E1_status.floor == E1_status.target_floor)
				{
					command_array[i].valid = 0;
					if (command_type == DIS_E1)
					{
						// cout << "EV1 POST7" << endl;
						Elevator1.Post(command_floor);
					}
					else
					{
						// cout << "EV1 POST8" << endl;
						Elevator1.Post(command_array[i].command - 10);
					}
				}
				// else leave in array
			}

			/**================================================== *
			 * ==========  Inside Elevator  ========== *
			 * ================================================== */

			else if ((command_type == DIS_E1 && command_array[i].valid == 1) || (command_type == DIS_E2 && command_array[i].valid == 1))
			{
				if (command_type == DIS_E1)
				{
					command_array[i].valid = 0;
					Elevator1.Post(command_floor);
				}
				else
				{
					command_array[i].valid = 0;
					// cout << "EV2 POST17" << endl;
					Elevator2.Post(command_floor);
				}
			}
			/* =======  End of Inside Elevator  ======= */

			/**================================================== *
			 * =======  Section EV Reached Target Floor  ======== *
			 * ================================================== */

			// go to passenger waiting first
			// both elevators idle
			else if (E1_status.target_floor == E1_status.floor && E2_status.target_floor == E2_status.floor && command_array[i].valid == 1)
			{
				int largest_age_index = 0;
				int largest_age = 0;
				int largest_age_command_type = 0;
				int largest_age_command_floor = 0;
				// no commands in array and no faults
				while (largest_age == 0 && command_array[COMMAND_SIZE - 1].command == 0)
				{
					largest_age_index = find_largest_age_index(4); // command / 10 != 4 -> any commands
					largest_age = command_array[largest_age_index].age;
					if (largest_age != 0)
					{
						if (command_array[largest_age_index].valid == 1)
						{
							largest_age_command_type = command_array[largest_age_index].command / 10;
							largest_age_command_floor = command_array[largest_age_index].command % 10;
							if (largest_age_command_type == DIS_E1)
							{
								Elevator1.Post(command_array[largest_age_index].command % 10);
								command_array[largest_age_index].valid = 0;
							}
							else if (largest_age_command_type == DIS_E2)
							{
								// cout << "EV2 POST9" << endl;
								Elevator2.Post(command_array[largest_age_index].command % 10);
								command_array[largest_age_index].valid = 0;
							}
							// E1 is closer, u or d
							else if (abs(largest_age_command_floor - E1_status.floor) <= abs(largest_age_command_floor - E2_status.floor))
							{
								if (mode == MANUAL_MODE || check_max_passenger(command_floor, DIS_E1))
								{
									Elevator1.Post(command_array[largest_age_index].command - 10);
									command_array[largest_age_index].valid = 0;
								}
							}
							else
							{
								// cout << "EV2 POST10" << endl;
								if (mode == MANUAL_MODE || check_max_passenger(command_floor, DIS_E2))
								{
									Elevator2.Post(command_array[largest_age_index].command - 10);
									command_array[largest_age_index].valid = 0;
								}
							}
						}
					}
				}
			}
			// only E1 idle
			else if (E1_status.target_floor == E1_status.floor && command_array[i].valid == 1)
			{
				int largest_age_index = 0;
				int largest_age = 0;
				int largest_age_command_type = 0;
				while (largest_age == 0 && command_array[COMMAND_SIZE - 1].command == 0)
				{
					largest_age_index = find_largest_age_index(1); // command / 10 != 1 -> any commands except E2
					largest_age = command_array[largest_age_index].age;
					if (largest_age != 0)
					{
						if (command_array[largest_age_index].valid == 1)
						{
							largest_age_command_type = command_array[largest_age_index].command / 10;
							// if (debug)
							// cout << "Posting to EV1" << endl;
							if (largest_age_command_type == DIS_E1)
							{
								Elevator1.Post(command_array[largest_age_index].command % 10);
								command_array[largest_age_index].valid = 0;
							}
							else
							{
								if (mode == MANUAL_MODE || check_max_passenger(command_floor, DIS_E1))
								{
									Elevator1.Post(command_array[largest_age_index].command - 10);
									command_array[largest_age_index].valid = 0;
								}
							}
						}
					}
				}
			}
			// only E2 idle
			else if (E2_status.target_floor == E2_status.floor && command_array[i].valid == 1)
			{
				int largest_age_index = 0;
				int largest_age = 0;
				int largest_age_command_type = 0;
				while (largest_age == 0 && command_array[COMMAND_SIZE - 1].command == 0)
				{
					largest_age_index = find_largest_age_index(0); // command / 10 != 0 -> any commands except E1
					largest_age = command_array[largest_age_index].age;
					if (largest_age != 0)
					{
						if (command_array[largest_age_index].valid == 1)
						{
							largest_age_command_type = command_array[largest_age_index].command / 10;
							//if (debug)
							// cout << "Posting to EV2" << endl;
							if (largest_age_command_type == DIS_E2)
							{
								// cout << "EV2 POST11" << endl;
								Elevator2.Post(command_array[largest_age_index].command % 10);
								command_array[largest_age_index].valid = 0;
							}
							else
							{
								//getchar();
								if (mode == MANUAL_MODE || check_max_passenger(command_floor, DIS_E2))
								{
									Elevator2.Post(command_array[largest_age_index].command - 10);
									command_array[largest_age_index].valid = 0;
								}
							}
						}
					}
				}
			}
			/* =======  End of EV Reached Target Floor  ======= */

			/**================================================== *
			 * ==========  Outside Elevator, Up Input  ========== *
			 * ================================================== */

			else if (command_type == DIS_OUT_UP && command_array[i].valid == 1)
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
								// cout << "EV2 POST13" << endl;
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
						// cout << "EV2 POST14" << endl;
						Elevator2.Post(Message);
					}
				}
				// not on the way, leave command in array
			}

			/* =======  End of Outside Elevator, UP Input  ======= */

			/**================================================== *
			 * ==========  Outside Elevator, Down Input  ========== *
			 * ================================================== */

			else if (command_type == DIS_OUT_DOWN && command_array[i].valid == 1)
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
								// cout << "EV2 POST15" << endl;
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
						// cout << "EV2 POST16" << endl;
						Elevator2.Post(Message);
					}
				}
				// not on the way, leave command in array
			}

			/* =======  End of Outside Elevator, Down Input  ======= */

			else
			{
				// leave in command_array
			}
		}
		/* =======  End of Command Search  ======= */
		if (end_sim)
		{
			while (E1_status.floor != 0 && E1_status.door != 0 && E2_status.floor != 0 && E2_status.door != 0)
			{
			}
			// cout << "End of Simulation" << endl;
			break;
		}
	}

	/* =======  End of Dispatcher  ======= */
	// cout << "End of Dispatcher" << endl;

	Elevator1Status.~CThread();
	Elevator2Status.~CThread();
	ReadPipeline.~CThread();
	IO.Post(END_SIM);

	Elevator1Status.WaitForThread();
	Elevator2Status.WaitForThread();
	ReadPipeline.WaitForThread();
	// cout << "Waiting for r2" << endl;
	r2.Wait();
	Elevator1.WaitForProcess();
	Elevator2.WaitForProcess();
	IO.WaitForProcess();

	delete[] command_array;
	command_array = NULL;

	return 0;
}

void save_command(int command)
{
	// increase the age of all valid command_array
	for (int i = 0; i < COMMAND_SIZE; i++)
	{
		if (command_array[i].valid == 1)
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

int find_closest_index(int floor, int type)
{
	int closest = 15;
	int closest_index = 100; // max index is 99
	// if we end with 100, nothing in array

	for (int i = 0; i < COMMAND_SIZE; i++)
	{
		int command_type = command_array[i].command / 10;
		int command_floor = command_array[i].command % 10;
		if (command_array[i].valid == 1 && command_type != type)
		{
			if (abs(floor - command_floor) < closest)
			{
				closest = abs(floor - command_floor);
				closest_index = i;
			}
		}
	}
	return closest_index;
}

int find_largest_age_index(int command_type)
{
	int largest_age = 0;
	int largest_age_index = 0;

	for (int i = 0; i < COMMAND_SIZE; i++)
	{
		if (command_array[i].valid && command_array[i].age > largest_age && (command_array[i].command / 10 != command_type))
		{
			largest_age = command_array[i].age;
			largest_age_index = i;
		}
	}
	return largest_age_index;
}

void empty_command_array()
{
	for (int i = 0; i < COMMAND_SIZE; i++)
	{
		command_array[i].valid = 0;
		command_array[i].command = 0;
		command_array[i].age = 0;
	}
}

int check_max_passenger(int req_floor, int elevator_num)
{
	if (elevator_num == DIS_E1)
	{
		if (E1_status.direction == UP)
		{
			for (int i = E1_status.floor; i < req_floor; i++)
			{
				increment_passenger(i, UP, DIS_E1);
			}
			// cout << "E1_UP " << endl
			//<< "Curr: " << E1_status.passenger_count << "; Passenger inside: " << passenger_inside_count << "; Passenger outside: " << passenger_outside_count << endl;
			if ((E1_status.passenger_count + passenger_outside_count - passenger_inside_count) < MAX_PASSENGERS)
			{
				passenger_outside_count = 0;

				passenger_outside_count = 0;
				passenger_inside_count = 0;
				passenger_inside_count = 0;
				return 1;
			}
			else
			{
				passenger_outside_count = 0;
				passenger_inside_count = 0;
				return 0;
			}
		}
		else if (E1_status.direction == DOWN)
		{
			for (int i = E1_status.floor; i > req_floor; i--)
			{
				increment_passenger(i, DOWN, DIS_E1);
			}
			// cout << "E1_DOWN " << endl
			//<< "Curr: " << E1_status.passenger_count << "; Passenger inside: " << passenger_inside_count << "; Passenger outside: " << passenger_outside_count << endl;
			if ((E1_status.passenger_count + passenger_outside_count - passenger_inside_count) < MAX_PASSENGERS)
			{
				passenger_outside_count = 0;
				passenger_inside_count = 0;
				return 1;
			}
			else
			{
				passenger_outside_count = 0;
				passenger_inside_count = 0;
				return 0;
			}
		}
	}
	else if (elevator_num == DIS_E2)
	{
		if (E2_status.direction == UP)
		{
			for (int i = E2_status.floor; i < req_floor; i++)
			{
				increment_passenger(i, UP, DIS_E2);
			}
			// cout << "E2_UP " << endl
			//<< "Curr: " << E2_status.passenger_count << "; Passenger inside: " << passenger_inside_count << "; Passenger outside: " << passenger_outside_count << endl;
			if ((E2_status.passenger_count + passenger_outside_count - passenger_inside_count) < MAX_PASSENGERS)
			{
				passenger_outside_count = 0;
				passenger_inside_count = 0;
				return 1;
			}
			else
			{
				passenger_outside_count = 0;
				passenger_inside_count = 0;
				return 0;
			}
		}
		else if (E2_status.direction == DOWN)
		{
			for (int i = E2_status.floor; i > req_floor; i--)
			{
				increment_passenger(i, DOWN, DIS_E2);
			}
			// cout << "E2_DOWN " << endl
			//<< "Curr: " << E2_status.passenger_count << "; Passenger inside: " << passenger_inside_count << "; Passenger outside: " << passenger_outside_count << endl;
			if ((E2_status.passenger_count + passenger_outside_count - passenger_inside_count) < MAX_PASSENGERS)
			{
				passenger_outside_count = 0;
				passenger_inside_count = 0;
				return 1;
			}
			else
			{
				passenger_outside_count = 0;
				passenger_inside_count = 0;
				return 0;
			}
		}
	}
}

void increment_passenger(int i, int direction, int elevator_num)
{
	if (elevator_num == DIS_E1)
	{
		if (direction == UP)
		{
			if (i == 0)
			{
				passenger_inside_count += E1_status.UP_array.s0.passenger_inside;
				passenger_outside_count += E1_status.UP_array.s0.passenger_outside;
			}
			else if (i == 1)
			{
				passenger_inside_count += E1_status.UP_array.s1.passenger_inside;
				passenger_outside_count += E1_status.UP_array.s1.passenger_outside;
			}
			else if (i == 2)
			{
				passenger_inside_count += E1_status.UP_array.s2.passenger_inside;
				passenger_outside_count += E1_status.UP_array.s2.passenger_outside;
			}
			else if (i == 3)
			{
				passenger_inside_count += E1_status.UP_array.s3.passenger_inside;
				passenger_outside_count += E1_status.UP_array.s3.passenger_outside;
			}
			else if (i == 4)
			{
				passenger_inside_count += E1_status.UP_array.s4.passenger_inside;
				passenger_outside_count += E1_status.UP_array.s4.passenger_outside;
			}
			else if (i == 5)
			{
				passenger_inside_count += E1_status.UP_array.s5.passenger_inside;
				passenger_outside_count += E1_status.UP_array.s5.passenger_outside;
			}
			else if (i == 6)
			{
				passenger_inside_count += E1_status.UP_array.s6.passenger_inside;
				passenger_outside_count += E1_status.UP_array.s6.passenger_outside;
			}
			else if (i == 7)
			{
				passenger_inside_count += E1_status.UP_array.s7.passenger_inside;
				passenger_outside_count += E1_status.UP_array.s7.passenger_outside;
			}
			else if (i == 8)
			{
				passenger_inside_count += E1_status.UP_array.s8.passenger_inside;
				passenger_outside_count += E1_status.UP_array.s8.passenger_outside;
			}
			else if (i == 9)
			{
				passenger_inside_count += E1_status.UP_array.s9.passenger_inside;
				passenger_outside_count += E1_status.UP_array.s9.passenger_outside;
			}
		}
		else if (direction == DOWN)
		{
			if (i == 0)
			{
				passenger_inside_count += E1_status.DOWN_array.s0.passenger_inside;
				passenger_outside_count += E1_status.DOWN_array.s0.passenger_outside;
			}
			else if (i == 1)
			{
				passenger_inside_count += E1_status.DOWN_array.s1.passenger_inside;
				passenger_outside_count += E1_status.DOWN_array.s1.passenger_outside;
			}
			else if (i == 2)
			{
				passenger_inside_count += E1_status.DOWN_array.s2.passenger_inside;
				passenger_outside_count += E1_status.DOWN_array.s2.passenger_outside;
			}
			else if (i == 3)
			{
				passenger_inside_count += E1_status.DOWN_array.s3.passenger_inside;
				passenger_outside_count += E1_status.DOWN_array.s3.passenger_outside;
			}
			else if (i == 4)
			{
				passenger_inside_count += E1_status.DOWN_array.s4.passenger_inside;
				passenger_outside_count += E1_status.DOWN_array.s4.passenger_outside;
			}
			else if (i == 5)
			{
				passenger_inside_count += E1_status.DOWN_array.s5.passenger_inside;
				passenger_outside_count += E1_status.DOWN_array.s5.passenger_outside;
			}
			else if (i == 6)
			{
				passenger_inside_count += E1_status.DOWN_array.s6.passenger_inside;
				passenger_outside_count += E1_status.DOWN_array.s6.passenger_outside;
			}
			else if (i == 7)
			{
				passenger_inside_count += E1_status.DOWN_array.s7.passenger_inside;
				passenger_outside_count += E1_status.DOWN_array.s7.passenger_outside;
			}
			else if (i == 8)
			{
				passenger_inside_count += E1_status.DOWN_array.s8.passenger_inside;
				passenger_outside_count += E1_status.DOWN_array.s8.passenger_outside;
			}
			else if (i == 9)
			{
				passenger_inside_count += E1_status.DOWN_array.s9.passenger_inside;
				passenger_outside_count += E1_status.DOWN_array.s9.passenger_outside;
			}
		}
	}
	else if (elevator_num == DIS_E2)
	{
		if (direction == UP)
		{
			if (i == 0)
			{
				passenger_inside_count += E2_status.UP_array.s0.passenger_inside;
				passenger_outside_count += E2_status.UP_array.s0.passenger_outside;
			}
			else if (i == 1)
			{
				passenger_inside_count += E2_status.UP_array.s1.passenger_inside;
				passenger_outside_count += E2_status.UP_array.s1.passenger_outside;
			}
			else if (i == 2)
			{
				passenger_inside_count += E2_status.UP_array.s2.passenger_inside;
				passenger_outside_count += E2_status.UP_array.s2.passenger_outside;
			}
			else if (i == 3)
			{
				passenger_inside_count += E2_status.UP_array.s3.passenger_inside;
				passenger_outside_count += E2_status.UP_array.s3.passenger_outside;
			}
			else if (i == 4)
			{
				passenger_inside_count += E2_status.UP_array.s4.passenger_inside;
				passenger_outside_count += E2_status.UP_array.s4.passenger_outside;
			}
			else if (i == 5)
			{
				passenger_inside_count += E2_status.UP_array.s5.passenger_inside;
				passenger_outside_count += E2_status.UP_array.s5.passenger_outside;
			}
			else if (i == 6)
			{
				passenger_inside_count += E2_status.UP_array.s6.passenger_inside;
				passenger_outside_count += E2_status.UP_array.s6.passenger_outside;
			}
			else if (i == 7)
			{
				passenger_inside_count += E2_status.UP_array.s7.passenger_inside;
				passenger_outside_count += E2_status.UP_array.s7.passenger_outside;
			}
			else if (i == 8)
			{
				passenger_inside_count += E2_status.UP_array.s8.passenger_inside;
				passenger_outside_count += E2_status.UP_array.s8.passenger_outside;
			}
			else if (i == 9)
			{
				passenger_inside_count += E2_status.UP_array.s9.passenger_inside;
				passenger_outside_count += E2_status.UP_array.s9.passenger_outside;
			}
		}
		else if (direction == DOWN)
		{
			if (i == 0)
			{
				passenger_inside_count += E2_status.DOWN_array.s0.passenger_inside;
				passenger_outside_count += E2_status.DOWN_array.s0.passenger_outside;
			}
			else if (i == 1)
			{
				passenger_inside_count += E2_status.DOWN_array.s1.passenger_inside;
				passenger_outside_count += E2_status.DOWN_array.s1.passenger_outside;
			}
			else if (i == 2)
			{
				passenger_inside_count += E2_status.DOWN_array.s2.passenger_inside;
				passenger_outside_count += E2_status.DOWN_array.s2.passenger_outside;
			}
			else if (i == 3)
			{
				passenger_inside_count += E2_status.DOWN_array.s3.passenger_inside;
				passenger_outside_count += E2_status.DOWN_array.s3.passenger_outside;
			}
			else if (i == 4)
			{
				passenger_inside_count += E2_status.DOWN_array.s4.passenger_inside;
				passenger_outside_count += E2_status.DOWN_array.s4.passenger_outside;
			}
			else if (i == 5)
			{
				passenger_inside_count += E2_status.DOWN_array.s5.passenger_inside;
				passenger_outside_count += E2_status.DOWN_array.s5.passenger_outside;
			}
			else if (i == 6)
			{
				passenger_inside_count += E2_status.DOWN_array.s6.passenger_inside;
				passenger_outside_count += E2_status.DOWN_array.s6.passenger_outside;
			}
			else if (i == 7)
			{
				passenger_inside_count += E2_status.DOWN_array.s7.passenger_inside;
				passenger_outside_count += E2_status.DOWN_array.s7.passenger_outside;
			}
			else if (i == 8)
			{
				passenger_inside_count += E2_status.DOWN_array.s8.passenger_inside;
				passenger_outside_count += E2_status.DOWN_array.s8.passenger_outside;
			}
			else if (i == 9)
			{
				passenger_inside_count += E2_status.DOWN_array.s9.passenger_inside;
				passenger_outside_count += E2_status.DOWN_array.s9.passenger_outside;
			}
		}
	}
}
