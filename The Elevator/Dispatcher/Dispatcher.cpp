// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157
#include "C:\RTExamples\rt.h"
#include <ElevatorData.h>
#include <ElevatorStatus.h>

struct mypipelinedata mystruct;

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
			 << "floor: " << E1_status.floor << endl;
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
			 << "floor: " << E2_status.floor << endl;
		print.Signal();
	}

	r2.Wait();
	return 0;
}

int main()
{
	int i;

	CProcess p1("C:\\Users\\sfron\\OneDrive\\School\\UBC 4th Year\\CPEN333\\Labs\\CPEN333-The-Elevator\\The Elevator\\x64\\Debug\\Elevator 1.exe", // pathlist to child program executable
				NORMAL_PRIORITY_CLASS,																											   // priority
				OWN_WINDOW,																														   // process has its own window
				ACTIVE																															   // process is active immediately
	);

	CProcess p2("C:\\Users\\sfron\\OneDrive\\School\\UBC 4th Year\\CPEN333\\Labs\\CPEN333-The-Elevator\\The Elevator\\x64\\Debug\\Elevator 2.exe", // pathlist to child program executable
				NORMAL_PRIORITY_CLASS,																											   // priority
				OWN_WINDOW,																														   // process has its own window
				ACTIVE																															   // process is active immediately
	);

	CProcess p3("C:\\Users\\sfron\\OneDrive\\School\\UBC 4th Year\\CPEN333\\Labs\\CPEN333-The-Elevator\\The Elevator\\x64\\Debug\\IO.exe", // pathlist to child program executable	plus some arguments
				NORMAL_PRIORITY_CLASS,																									   // priority
				OWN_WINDOW,																												   // process has its own window
				ACTIVE);

	CThread Elevator1(DispatcherStatusElevator1, ACTIVE, NULL);
	CThread Elevator2(DispatcherStatusElevator2, ACTIVE, NULL);

	r1.Wait();
	print.Wait();
	cout << "all threads/processes arrived at end rendevous" << endl;
	print.Signal();

	/* // Read from datapool
	cout << "Dispather attempting to create/use the datapool.....\n";
	CDataPool dp("ElevatorDataPool", sizeof(struct mydatapooldata));

	struct mydatapooldata *MyDataPool = (struct mydatapooldata *)(dp.LinkDataPool());

	cout << "Dispatcher linked to datapool at address : " << MyDataPool << ".....\n";

	cout << "Child Read value for Floor = " << MyDataPool->floor << endl;
	cout << "Child Read value for Direction = " << MyDataPool->direction << endl;

	cout << "Child Read values for floor array = ";
	for (i = 0; i < 10; i++)
		cout << MyDataPool->floors[i] << " "; */

	// Read from pipeline
	CPipe pipe1("MyPipe", 1024);

	pipe1.Read(&mystruct, sizeof(mystruct));
	print.Wait();
	cout << "Child Process read [" << mystruct.x << "," << mystruct.y << "] from Pipeline.....\n";
	print.Signal();

	Sleep(10000);
	stop = 1;
	r2.Wait();
	cout << "all threads/processes arrived at end rendevous" << endl;

	Elevator1.WaitForThread();
	Elevator2.WaitForThread();
	p1.WaitForProcess();
	p2.WaitForProcess();
	p3.WaitForProcess();

	return 0;
}
