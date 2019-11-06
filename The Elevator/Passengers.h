// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157
#ifndef __Passengers__
#define __Passengers__

#define TIMEOUT 100
#include "C:\RTExamples\rt.h"
#include <ElevatorData.h>
#include <random>

class Passengers : public ActiveClass
{
	int current_floor;
	char current_floor_char;
	char request_direction;
	int request_floor;
	char request_floor_char;
	char elevator_num;

	int main(void)
	{
		CTypedPipe<command> passengerPipe("PassengerPipeline", 1024);

		if (current_floor == 0)
			request_direction = 'u';
		else if (current_floor == 9)
			request_direction = 'd';
		else
		{
			random_device rd;
			mt19937 eng(rd());
			uniform_int_distribution<> distr(0, 1);
			if (distr(eng))
				request_direction = 'u';
			else
				request_direction = 'd';
		}

		command mystruct = {request_direction, current_floor_char};
		passengerPipelineMutex.Wait();
		passengerPipe.Write(&mystruct);
		passengerPipelineMutex.Signal();

		if (request_direction == 'u')
		{
			// poll EV1 and EV2 UP[floor]
			while (1)
			{
				if (EV1_UP_WAIT(current_floor))
				{
					elevator_num = '1';
					break;
				}
				if (EV2_UP_WAIT(current_floor))
				{
					elevator_num = '2';
					break;
				}
				if (EV1_DW_WAIT(current_floor))
				{
					elevator_num = '1';
					break;
				}
				if (EV2_DW_WAIT(current_floor))
				{
					elevator_num = '2';
					break;
				}
			}
			//randomize which floor passenger wants to go to
			random_device rd;
			mt19937 eng(rd());
			uniform_int_distribution<> distr(current_floor + 1, 9);
			request_floor = distr(eng);
		}
		else
		{
			// poll EV1 and EV2 DW[floor]
			while (1)
			{
				if (EV1_DW_WAIT(current_floor))
				{
					elevator_num = '1';
					break;
				}
				if (EV2_DW_WAIT(current_floor))
				{
					elevator_num = '2';
					break;
				}
				if (EV1_UP_WAIT(current_floor))
				{
					elevator_num = '1';
					break;
				}
				if (EV2_UP_WAIT(current_floor))
				{
					elevator_num = '2';
					break;
				}
			}
			//randomize which floor passenger wants to go to
			random_device rd;
			mt19937 eng(rd());
			uniform_int_distribution<> distr(0, current_floor - 1);
			request_floor = distr(eng);
		}

		request_floor_char = '0' + request_floor;
		mystruct = {elevator_num, request_floor_char};
		passengerPipelineMutex.Wait();
		passengerPipe.Write(&mystruct);
		passengerPipelineMutex.Signal();

		if (request_direction == 'u' && elevator_num == '1')
		{
			while (1)
			{
				if (EV1_UP_WAIT(request_floor))
					break;
			}
		}
		else if (request_direction == 'd' && elevator_num == '1')
		{
			while (1)
			{
				if (EV1_DW_WAIT(request_floor))
					break;
			}
		}
		else if (request_direction == 'u' && elevator_num == '2')
		{
			while (1)
			{
				if (EV2_UP_WAIT(request_floor))
					break;
			}
		}
		else if (request_direction == 'd' && elevator_num == '2')
		{
			while (1)
			{
				if (EV2_DW_WAIT(request_floor))
					break;
			}
		}

		// passenger leaves - destroy passenger
		return 0;
	}

public:
	Passengers()
	{
		random_device rd;
		mt19937 eng(rd());
		uniform_int_distribution<> distr(0, 9);
		current_floor = distr(eng);
		current_floor_char = '0' + current_floor;
		request_direction = 'u';
		request_floor = 0;
		request_floor_char = '0';
		elevator_num = '1';
	}

	~Passengers() {}

	//wait for elevator 1 up door to open
	int EV1_UP_WAIT(int floor)
	{
		if (floor == 0)
		{
			if (EV1_UP0.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 1)
		{
			if (EV1_UP1.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 2)
		{
			if (EV1_UP2.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 3)
		{
			if (EV1_UP3.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 4)
		{
			if (EV1_UP4.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 5)
		{
			if (EV1_UP5.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 6)
		{
			if (EV1_UP6.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 7)
		{
			if (EV1_UP7.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 8)
		{
			if (EV1_UP8.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 9)
		{
			if (EV1_UP9.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
	}

	int EV1_DW_WAIT(int floor)
	{
		if (floor == 0)
		{
			if (EV1_DW0.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 1)
		{
			if (EV1_DW1.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 2)
		{
			if (EV1_DW2.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 3)
		{
			if (EV1_DW3.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 4)
		{
			if (EV1_DW4.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 5)
		{
			if (EV1_DW5.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 6)
		{
			if (EV1_DW6.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 7)
		{
			if (EV1_DW7.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 8)
		{
			if (EV1_DW8.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 9)
		{
			if (EV1_DW9.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
	}

	int EV2_UP_WAIT(int floor)
	{
		if (floor == 0)
		{
			if (EV2_UP0.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 1)
		{
			if (EV2_UP1.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 2)
		{
			if (EV2_UP2.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 3)
		{
			if (EV2_UP3.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 4)
		{
			if (EV2_UP4.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 5)
		{
			if (EV2_UP5.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 6)
		{
			if (EV2_UP6.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 7)
		{
			if (EV2_UP7.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 8)
		{
			if (EV2_UP8.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 9)
		{
			if (EV2_UP9.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
	}

	int EV2_DW_WAIT(int floor)
	{
		if (floor == 0)
		{
			if (EV2_DW0.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 1)
		{
			if (EV2_DW1.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 2)
		{
			if (EV2_DW2.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 3)
		{
			if (EV2_DW3.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 4)
		{
			if (EV2_DW4.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 5)
		{
			if (EV2_DW5.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 6)
		{
			if (EV2_DW6.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 7)
		{
			if (EV2_DW7.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 8)
		{
			if (EV2_DW8.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
		else if (floor == 9)
		{
			if (EV2_DW9.Wait(TIMEOUT) == WAIT_TIMEOUT)
				return 0;
			else
				return 1;
		}
	}
};

#endif
