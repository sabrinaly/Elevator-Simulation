// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157
#include "C:\RTExamples\rt.h"
#include <ElevatorData.h>
#include <ElevatorStatus.h>

pipeline_data mystruct;

elevator_status E1_status;
elevator_status E2_status;

UINT __stdcall DispatcherStatusElevator1(void *args) // thread function
{
	ElevatorStatus Elevator1Status("Elevator1");
	r1.Wait();
	for (int i = 0; i < floors; i++)
	{
		E1_status = Elevator1Status.Dispatcher_Get_Elevator_Status();
		print.Wait();
		cout << "Dispatcher >>> elevator 1 status" << endl
			 << "direction: " << E1_status.direction << endl
			 << "floor: " << E1_status.floor << endl
			 << "target floor: " << E1_status.target_floor << endl;
		print.Signal();
	}

	r2.Wait();
	return 0;
}

UINT __stdcall DispatcherStatusElevator2(void *args) // thread function
{
	ElevatorStatus Elevator2Status("Elevator2");
	r1.Wait();
	for (int i = 0; i < floors; i++)
	{
		E2_status = Elevator2Status.Dispatcher_Get_Elevator_Status();
		print.Wait();
		cout << "Dispatcher >>> elevator 2 status" << endl
			 << "direction: " << E2_status.direction << endl
			 << "floor: " << E2_status.floor << endl
			 << "target floor: " << E2_status.target_floor << endl;
		print.Signal();
	}

	r2.Wait();
	return 0;
}

int main()
{
	int i;

	CProcess p1("C:\\Users\\Sabrina Ly\\Documents\\Year4\\CPEN 333\\CPEN333-The-Elevator\\The Elevator\\Debug\\Elevator 1.exe", // pathlist to child program executable
				NORMAL_PRIORITY_CLASS,																											   // priority
				OWN_WINDOW,																														   // process has its own window
				ACTIVE																															   // process is active immediately
	);

	CProcess p2("C:\\Users\\Sabrina Ly\\Documents\\Year4\\CPEN 333\\CPEN333-The-Elevator\\The Elevator\\Debug\\Elevator 2.exe", // pathlist to child program executable
				NORMAL_PRIORITY_CLASS,																											   // priority
				OWN_WINDOW,																														   // process has its own window
				ACTIVE																															   // process is active immediately
	);

	CProcess p3("C:\\Users\\Sabrina Ly\\Documents\\Year4\\CPEN 333\\CPEN333-The-Elevator\\The Elevator\\Debug\\IO.exe", // pathlist to child program executable	plus some arguments
				NORMAL_PRIORITY_CLASS,																									   // priority
				OWN_WINDOW,																												   // process has its own window
				ACTIVE);

	CThread Elevator1(DispatcherStatusElevator1, ACTIVE, NULL);
	CThread Elevator2(DispatcherStatusElevator2, ACTIVE, NULL);

	r1.Wait();
	CPipe pipe1("MyPipe", 1024); // pipeline from IO with keyboard commands
	int Message = NULL;
	int current_floor = NULL;

	while (1) {
		pipe1.Read(&mystruct, sizeof(mystruct));

		//Outside Elevator
		if (mystruct.x == 'u') {
			current_floor = mystruct.y - '0';
			if (current_floor >= 0 && current_floor <= 9) {
				Message = 10 + current_floor;	// 10-19 for up

				if (E1_status.direction && E1_status.floor <= current_floor && E1_status.target_floor >= current_floor) {
					if (E2_status.direction && E2_status.floor <= current_floor && E2_status.target_floor >= current_floor) {
						if ((current_floor - E1_status.floor) < (current_floor - E2_status.floor))
							p1.Post(Message);
						else
							p2.Post(Message);
					}
					else
						p1.Post(Message);
				}
				else if (E2_status.direction && E2_status.floor <= current_floor && E2_status.target_floor >= current_floor)
					p2.Post(Message);
				else if (E1_status.direction && E1_status.floor <= current_floor) { // target_floor is < current_floor
					if (E2_status.direction && E2_status.floor <= current_floor) {
						// ?
					}
				}
			}
		}
		else if (mystruct.x == 'd') {
			current_floor = mystruct.y - '0';
			if (current_floor >= 0 && current_floor <= 9) {
				Message = 20 + current_floor; // 20-29 for down
			}
		}
		//Inside Elevator
		else if (mystruct.x == '1' || mystruct.x == '2') {
			int elevator_num = mystruct.x - '0';
			int target_floor = mystruct.y - '0';
			if (target_floor >= 0 && target_floor <= 9) {
				if (mystruct.x == '1')
					p1.Post(target_floor);
				else
					p2.Post(target_floor);
			}
		}
		else {
			// wrong command 
			// if do nothing, don't need this?
		}
	}


	r2.Wait();

	Elevator1.WaitForThread();
	Elevator2.WaitForThread();
	p1.WaitForProcess();
	p2.WaitForProcess();
	p3.WaitForProcess();

	return 0;
}
