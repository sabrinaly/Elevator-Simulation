// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157
#include "C:\RTExamples\rt.h"
#include <ElevatorData.h>

struct mypipelinedata mystruct;

int temp_E1_status;
int temp_E2_status;

UINT __stdcall DispatcherStatusElevator1(void *args) // thread function
{
	r1.Wait();
	elevator_status *dispatcher_elevator1status; // pointer to the data struct
	dispatcher_elevator1status = (struct elevator_status *)(Elevator1StatusDataPool->LinkDataPool());
	while (1)
	{
		ps2.Wait();
		//get status once new data is available
		temp_E1_status = dispatcher_elevator1status->temp;
		cs2.Signal();
	}
	r2.Wait();
	return 0;
}

UINT __stdcall DispatcherStatusElevator2(void *args) // thread function
{
	r1.Wait();
	elevator_status *dispatcher_elevator2status; // pointer to the data struct
	dispatcher_elevator2status = (struct elevator_status *)(Elevator2StatusDataPool->LinkDataPool());
	while (1)
	{
		ps4.Wait();
		//get status once new data is available
		temp_E2_status = dispatcher_elevator2status->temp;
		cs4.Signal();
	}
	r2.Wait();
	return 0;
}

int main()
{
	int i;

	CProcess p1("C:\\Users\\Sabrina Ly\\Documents\\Year4\\CPEN 333\\CPEN333-The-Elevator\\The Elevator\\Debug\\Elevator 1.exe", // pathlist to child program executable
				NORMAL_PRIORITY_CLASS,																							// priority
				OWN_WINDOW,																										// process has its own window
				ACTIVE																											// process is active immediately
	);

	CProcess p2("C:\\Users\\Sabrina Ly\\Documents\\Year4\\CPEN 333\\CPEN333-The-Elevator\\The Elevator\\Debug\\Elevator 2.exe", // pathlist to child program executable
				NORMAL_PRIORITY_CLASS,																							// priority
				OWN_WINDOW,																										// process has its own window
				ACTIVE																											// process is active immediately
	);

	CProcess p3("C:\\Users\\Sabrina Ly\\Documents\\Year4\\CPEN 333\\CPEN333-The-Elevator\\The Elevator\\Debug\\IO.exe", // pathlist to child program executable	plus some arguments
				NORMAL_PRIORITY_CLASS,																					// priority
				OWN_WINDOW,																								// process has its own window
				ACTIVE);

	CThread Elevator1(DispatcherStatusElevator1, ACTIVE, NULL);
	CThread Elevator2(DispatcherStatusElevator2, ACTIVE, NULL);

	r1.Wait();

	// Read from datapool
	cout << "Dispather attempting to create/use the datapool.....\n";
	CDataPool dp("ElevatorDataPool", sizeof(struct mydatapooldata));

	struct mydatapooldata *MyDataPool = (struct mydatapooldata *)(dp.LinkDataPool());

	cout << "Dispatcher linked to datapool at address : " << MyDataPool << ".....\n";

	cout << "Child Read value for Floor = " << MyDataPool->floor << endl;
	cout << "Child Read value for Direction = " << MyDataPool->direction << endl;

	cout << "Child Read values for floor array = ";
	for (i = 0; i < 10; i++)
		cout << MyDataPool->floors[i] << " ";

	// Read from pipeline
	cout << endl
		 << "Dispatcher Opening Pipeline.....\n";
	CPipe pipe1("MyPipe", 1024);

	pipe1.Read(&mystruct, sizeof(mystruct));
	cout << "Child Process read [" << mystruct.x << "," << mystruct.y << "] from Pipeline.....\n";
	//Sleep(2000);

	r2.Wait();

	Elevator1.WaitForThread();
	Elevator2.WaitForThread();
	p1.WaitForProcess();
	p2.WaitForProcess();
	p3.WaitForProcess();

	return 0;
}
