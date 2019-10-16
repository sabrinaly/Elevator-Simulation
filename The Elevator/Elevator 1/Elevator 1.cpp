// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157
#include "C:\RTExamples\rt.h"
#include <ElevatorData.h>
#include <ElevatorStatus.h>

int main()
{
	ElevatorStatus Elevator1Status("Elevator1");
	elevator_status status;
	r1.Wait();

	/* // Write to datapool
	cout << "Elevator 1 attempting to create/use the datapool.....\n";
	CDataPool dp("ElevatorDataPool", sizeof(struct mydatapooldata));

	struct mydatapooldata *MyDataPool = (struct mydatapooldata *)(dp.LinkDataPool());

	cout << "Parent Writing value '10' to floor variable.....\n";
	MyDataPool->floor = 10; // store 55 into the variable 'floor' in the datapool

	cout << "Parent Writing value '1' to direction variable.....\n";
	MyDataPool->direction = 1; // store 1 into the variable 'direction' in the datapool

	cout << "Parent Writing value '0 1 2 3 4 5 6 7 8 9' to floors array.....\n";
	for (int i = 0; i < 10; i++)
		MyDataPool->floors[i] = i;

	cout << "Press RETURN to end Elevator 1 process\n";
	getchar(); */

	for (int i = 0; i < floors; i++)
	{
		random_device rd;						// obtain a random number from hardware
		mt19937 eng(rd());						// seed the generator
		uniform_int_distribution<> distr(0, 1); // define the range
		status.direction = distr(eng);
		uniform_int_distribution<> distr1(1, 10); // define the range
		status.floor = distr1(eng);
		/* for (int i = 0; i < 10; i++)
	{
		status.floors[i] = i;
	} */

		cout << "elevator 1 writing status ..." << endl;
		Elevator1Status.Update_Status(status);
	}

	cout << "Press RETURN to end Elevator 1 process\n";
	getchar();

	r2.Wait();
	cout << "all threads/processes arrived at end rendevous" << endl;

	return 0;
}