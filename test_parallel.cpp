// copy from http://jqyblogger.blogspot.com/2012/06/sample-c-code-for-push-relabel-max-flow.html

#include <iostream>
#include <string>
#include <thread>

//#include "Graph.h"
using namespace std;

//The function we want to make the thread run.
void task1(string msg)
{
	cout << "task1 says: " << msg;
}

int main()
{
	// Graph a(0,3);
	// a.printInfo();
	// a.mineMaxFlow();

	thread t1(task1, "Hello");
	t1.join();

	return 0;
}
