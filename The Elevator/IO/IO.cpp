// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157
#include "C:\RTExamples\rt.h"
#include <ElevatorData.h>

struct mypipelinedata mystruct = {2, 5.5};

int temp_E1_status;
int temp_E2_status;

UINT __stdcall IOStatusElevator1(void *args) // thread function
{
	r1.Wait();
	elevator_status *IO_elevator1status; // pointer to the data struct
	IO_elevator1status = (struct elevator_status *)(Elevator1StatusDataPool->LinkDataPool());
	while (1)
	{
		ps1.Wait();
		//get status once new data is available
		temp_E1_status = IO_elevator1status->temp;
		cs1.Signal();
	}
	r2.Wait();
	return 0;
}

UINT __stdcall IOStatusElevator2(void *args) // thread function
{
	r1.Wait();
	elevator_status *IO_elevator2status; // pointer to the data struct
	IO_elevator2status = (struct elevator_status *)(Elevator1StatusDataPool->LinkDataPool());
	while (1)
	{
		ps3.Wait();
		//get status once new data is available
		temp_E2_status = IO_elevator2status->temp;
		cs3.Signal();
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

	// Read from datapool
	cout << "Dispather attempting to create/use the datapool.....\n";
	CDataPool dp("ElevatorDataPool", sizeof(struct mydatapooldata));

	struct mydatapooldata *MyDataPool = (struct mydatapooldata *)(dp.LinkDataPool());

	cout << "Dispatcher linked to datapool at address : " << MyDataPool << ".....\n";

	cout << "Child Read value for Floor = " << MyDataPool->floor << endl;
	cout << "Child Read value for Direction = " << MyDataPool->direction << endl;

	printf("Child Read values for floor array = ");
	for (i = 0; i < 10; i++)
		cout << MyDataPool->floors[i] << " ";

	// Write to pipeline
	cout << "Parent Process Creating the Pipeline.....\n";
	CPipe pipe1("MyPipe", 1024);

	cout << "Hit RETURN to Write the structure [" << mystruct.x << ", " << mystruct.y << "] to the pipeline.....\n";
	getchar();

	pipe1.Write(&mystruct, sizeof(mystruct));

	cout << "Press RETURN to end IO process\n";
	getchar();

	r2.Wait();

	Elevator1.WaitForThread();
	Elevator2.WaitForThread();

	return 0;
}