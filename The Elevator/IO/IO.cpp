// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157
#include "C:\RTExamples\rt.h"
#include <ElevatorData.h>
#include <ElevatorStatus.h>

struct mypipelinedata mystruct = {2, 5.5};

elevator_status E1_status;
elevator_status E2_status;

UINT __stdcall IOStatusElevator1(void *args) // thread function
{
	ElevatorStatus Elevator1Status("Elevator1");
	r1.Wait();
	for (int i = 0; i < floors; i++)
	{
		E1_status = Elevator1Status.IO_Get_Elevator_Status();
		print.Wait();
		cout << "IO >>> elevator 1 status" << endl
			 << "direction: " << E1_status.direction << endl
			 << "floor: " << E1_status.floor << endl;
		print.Signal();
	}
	r2.Wait();
	return 0;
}

UINT __stdcall IOStatusElevator2(void *args) // thread function
{
	ElevatorStatus Elevator2Status("Elevator2");
	r1.Wait();
	for (int i = 0; i < floors; i++)
	{
		elevator_status *dispatcher_elevator2status; // pointer to the data struct
		dispatcher_elevator2status = (struct elevator_status *)(Elevator2StatusDataPool->LinkDataPool());
		E2_status = Elevator2Status.IO_Get_Elevator_Status();
		print.Wait();
		cout << "IO >>> elevator 2 status" << endl
			 << "direction: " << E2_status.direction << endl
			 << "floor: " << E2_status.floor << endl;
		print.Signal();
	}
	r2.Wait();
	return 0;
}

int main()
{
	CThread Elevator1(IOStatusElevator1, ACTIVE, NULL);
	CThread Elevator2(IOStatusElevator2, ACTIVE, NULL);

	r1.Wait();
	int i;

	/* // Read from datapool
	cout << "Dispather attempting to create/use the datapool.....\n";
	CDataPool dp("ElevatorDataPool", sizeof(struct mydatapooldata));

	struct mydatapooldata *MyDataPool = (struct mydatapooldata *)(dp.LinkDataPool());

	cout << "Dispatcher linked to datapool at address : " << MyDataPool << ".....\n";

	cout << "Child Read value for Floor = " << MyDataPool->floor << endl;
	cout << "Child Read value for Direction = " << MyDataPool->direction << endl;

	printf("Child Read values for floor array = ");
	for (i = 0; i < 10; i++)
		cout << MyDataPool->floors[i] << " "; */

	// Write to pipeline
	print.Wait();
	cout << "Parent Process Creating the Pipeline.....\n";
	print.Signal();

	CPipe pipe1("MyPipe", 1024);

	/* print.Wait();
	cout << "Hit RETURN to Write the structure [" << mystruct.x << ", " << mystruct.y << "] to the pipeline.....\n";
	print.Signal();
	getchar(); */

	pipe1.Write(&mystruct, sizeof(mystruct));

	print.Wait();
	cout << "Press RETURN to end IO process\n";
	print.Signal();

	getchar();

	r2.Wait();

	Elevator1.WaitForThread();
	Elevator2.WaitForThread();

	return 0;
}