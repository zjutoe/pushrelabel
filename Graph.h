// copy from http://jqyblogger.blogspot.com/2012/06/sample-c-code-for-push-relabel-max-flow.html

#include<iostream>
#include<deque>

using std::deque;

using std::cin;
using std::cout;
using std::endl;


class Graph
{
private:
int ** cMatrix;//capacity matrix
int ** fMatrix;//flow
int ** rMatrix;//residual flow
int * height;//height function
int * excess;//the excess of each node
deque<int> excessNode;
int nodeNumber;
int source;
int sink;


public:
Graph(int, int);//constructor
// Graph(int, int **, int **);
void printInfo();
void residualBuilder();
int checkNode(int);
void push(int, int);
void relable(int);
void mineMaxFlow();

};
