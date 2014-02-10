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
int ** cMatrixR;//capacity matrix
int ** cMatrixL;//capacity matrix
int ** cMatrixD;//capacity matrix
int ** cMatrixU;//capacity matrix 
int ** fMatrixR;//flow
int ** fMatrixL;//flow
int ** fMatrixD;//flow
int ** fMatrixU;//flow
int ** rMatrixR;//residual flow
int ** rMatrixL;//residual flow
int ** rMatrixD;//residual flow
int ** rMatrixU;//residual flow
int ** hMatrix;//height function
int ** eMatrix;//the excess of each node
deque<int> excessNode;
int W;
int H;
int nodeNumber;
int source;
int sink;

int clk;


public:
Graph(int, int);//constructor
// Graph(int, int **, int **);
void printInfo();
void residualBuilder();
int checkNode(int);
void push(int, int);
void relable(int);
void mineMaxFlow();
int Graph::push_R(int i, int j);
int Graph::push_L(int i, int j);
int Graph::push_D(int i, int j);
int Graph::push_U(int i, int j);

};
