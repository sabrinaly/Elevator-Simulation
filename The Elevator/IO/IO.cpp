// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157
#include "C:\RTExamples\rt.h"
#include <ElevatorData.h>

struct	 mypipelinedata mystruct = { 2, 5.5 };

int main()
{
	r1.Wait();
	int i;

	// Read from datapool
	cout << "Dispather attempting to create/use the datapool.....\n";
	CDataPool 		dp("ElevatorDataPool", sizeof(struct mydatapooldata));

	struct mydatapooldata* MyDataPool = (struct mydatapooldata*)(dp.LinkDataPool());

	cout << "Dispatcher linked to datapool at address : " << MyDataPool << ".....\n";

	cout << "Child Read value for Floor = " << MyDataPool->floor << endl;
	cout << "Child Read value for Direction = " << MyDataPool->direction << endl;

	printf("Child Read values for floor array = ");
	for (i = 0; i < 10; i++)
		cout << MyDataPool->floors[i] << " ";

	// Write to pipeline
	cout << "Parent Process Creating the Pipeline.....\n";
	CPipe	pipe1("MyPipe", 1024);

	cout << "Hit RETURN to Write the structure [" << mystruct.x << ", " << mystruct.y << "] to the pipeline.....\n";
	getchar();

	pipe1.Write(&mystruct, sizeof(mystruct));

	cout << "Press RETURN to end IO process\n";
	getchar();

	r2.Wait();

	return 0;
}