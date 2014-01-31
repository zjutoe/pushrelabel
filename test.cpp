// copy from http://jqyblogger.blogspot.com/2012/06/sample-c-code-for-push-relabel-max-flow.html

#include "Graph.h"
using namespace std;

int main()
{
	Graph a(0,3);
	a.printInfo();
	a.mineMaxFlow();
	return 0;
}
