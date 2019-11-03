// Names: Sabrina Ly & Raymond Chu
// Student Numbers: 16997158 & 23248157
#include "C:\RTExamples\rt.h"
#include <ElevatorData.h>
#include <ElevatorStatus.h>

int main()
{
	ElevatorStatus Elevator2Status("Elevator2");
	elevator_status status;
	r1.Wait();

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

		cout << "elevator 2 writing status ..." << endl;
		Elevator2Status.Update_Status(status);
	}

	cout << "Press RETURN to end Elevator 2 process\n";
	getchar();

	r2.Wait();
	return 0;
}