// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157
#include "C:\RTExamples\rt.h"
#include <ElevatorData.h>

int main()
{
	r1.Wait();

	// Write to datapool
	cout << "Elevator 1 attempting to create/use the datapool.....\n";
	CDataPool 		dp("ElevatorDataPool", sizeof(struct mydatapooldata));

	struct mydatapooldata* MyDataPool = (struct mydatapooldata*)(dp.LinkDataPool());

	cout << "Parent Writing value '10' to floor variable.....\n";
	MyDataPool->floor = 10;		// store 55 into the variable 'floor' in the datapool

	cout << "Parent Writing value '1' to direction variable.....\n";
	MyDataPool->direction = 1;		// store 1 into the variable 'direction' in the datapool

	cout << "Parent Writing value '0 1 2 3 4 5 6 7 8 9' to floors array.....\n";
	for (int i = 0; i < 10; i++)
		MyDataPool->floors[i] = i;

	cout << "Press RETURN to end Elevator 1 process\n";
	getchar();

	r2.Wait();

	return 0;
}