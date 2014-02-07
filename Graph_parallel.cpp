// copy from http://jqyblogger.blogspot.com/2012/06/sample-c-code-for-push-relabel-max-flow.html

#include <climits>
#include <thread>
#include "Graph.h"


Graph::Graph(int s, int t)// s and t represent the index of source node and sink node
{
	int i,j;
	// source = s;
	// sink = t;
	cout<<"Please input the number of nodes, the source and sink nodes"<<endl;
	cin>>nodeNumber;
	cin>>source>>sink;
	s = source;
	t = sink;

	height = new int[nodeNumber];
	excess = new int[nodeNumber];


	for(i = 0;i<nodeNumber;i++) {
		height[i] = 0;
		excess[i] = 0;
	}


	height[s]  = nodeNumber;//the height function of the source should be changed first


	cout<<"please input the matrix of capacity!"<<endl;
	cMatrix = new int*[nodeNumber];
	fMatrix = new int*[nodeNumber];//initialize the flow matrix to be all 0
	rMatrix = new int*[nodeNumber];
	for(i = 0;i<nodeNumber;i++)	{
		rMatrix[i] = new int[nodeNumber];
		cMatrix[i] = new int[nodeNumber];
		fMatrix[i] = new int[nodeNumber];
		for(j = 0;j<nodeNumber;j++)			{
			cin>>cMatrix[i][j];
			fMatrix[i][j] = 0;
			rMatrix[i][j] = 0;
		}
	}
	for(i = 0;i<nodeNumber;i++) {
		if(cMatrix[s][i]!=0) {
			fMatrix[s][i] = cMatrix[s][i];
			fMatrix[i][s] = -1*cMatrix[s][i];
			excess[i] = cMatrix[s][i];
			excess[s]-= cMatrix[s][i];//at first, the excess of source node should be negative. 
			if(i!=t)
				excessNode.push_back(i);//the sink cannot puch flow back!
		}
	}//all the outgoing links from source should be satuated first
  
	residualBuilder();//compose the residual network.
	// printInfo();
}


int Graph::checkNode(int i)//check whether the excess node i can push some flow out
{
	int j;
	if(excess[i]<=0)
		return -1;//first, i should have some excess flow
	for(j = 0;j<nodeNumber;j++)	{
		if(rMatrix[i][j]>0 && height[i] == height[j]+1) {//fulfill the height condition and also have "extra capacity" in residual network			
			return j;
		}
	}
	return -1;
}


void Graph::push(int a, int b)//push excess flow from node i to node j
{
	
	cout<<"now we do a push from node "<<a<<" to node "<<b<<endl;
	int delta;


	if(height[a]!=height[b]+1)
		cout<<"watch out, there is a mistake in push!"<<endl;


	if(excess[a]<=rMatrix[a][b])
		delta = excess[a];
	else
		delta = rMatrix[a][b];//identify the amount we can push


	rMatrix[b][a]+=delta;
	rMatrix[a][b]-=delta;
	excess[a]-=delta;
	excess[b]+=delta;


	if(cMatrix[a][b]>0) {
		fMatrix[a][b]+=delta;
		fMatrix[b][a] = -1*fMatrix[a][b];
	} else	{
		fMatrix[b][a]-= delta;
		fMatrix[a][b] = -1*fMatrix[b][a];
	}
	if(excess[b]>0 && b!=sink)
		excessNode.push_back(b);//sometimes j can be the source node,so it's better we check here, and sink cannot push flow back
	if(excess[a]>0)
		excessNode.push_front(a);//if i is still excess, push it back into the queue,and we wish to satuarate

	printInfo();
}




void Graph::relable(int a)//relable this node a
{
	cout<<"now we do a relable for node "<<a<<endl;
	if(excess[a]<=0)
		{cout<<"there is a mistake in relable"<<endl;cin.get();}


	int i,target,temp = INT_MAX;
	for(i = 0;i<nodeNumber;i++) {
		if(rMatrix[a][i]>0 && height[i]<temp) {
			temp = height[i];
		}
	}


	height[a] = temp+1;
	excessNode.push_back(a);
	printInfo();
}


void Graph::mineMaxFlow()
{
	int current,i,j,target;
	while(excessNode.size()!=0) {
		current = excessNode.at(0);
		excessNode.pop_front();
		target = checkNode(current);
		if(checkNode(current)<0) {
			relable(current);
		} else {
			push(current,target);
		}
	}


	cout<<"the final flow is like:"<<endl;
	for(i = 0;i<nodeNumber;i++) {
		for(j = 0;j<nodeNumber;j++) {
			cout<<fMatrix[i][j]<<" ";
		}
		cout<<endl;
	}
}

void Graph::residualBuilder()
{
	int i,j;
	for(i = 0;i<nodeNumber;i++) {
		for(j = 0;j<nodeNumber;j++) {
			if(fMatrix[i][j]>=0) {
				rMatrix[i][j] = cMatrix[i][j]-fMatrix[i][j];
			} else	{
				rMatrix[i][j] = fMatrix[j][i];
			}
		}
	}
}
void Graph::printInfo()
{
	int i,j;
	/* cout<<"the capacity situation of this graph is:"<<endl;


	   for(i = 0;i<nodeNumber;i++)
	   {
	   for(j = 0;j<nodeNumber;j++)
	   {
	   cout<<cMatrix[i][j]<<" ";
	   }
	   cout<<endl;
	   }*/


	cout<<"the flow situation of this graph is: "<<endl;
	for(i = 0;i<nodeNumber;i++) {
		for(j = 0;j<nodeNumber;j++) {
			cout<<fMatrix[i][j]<<" ";
		}
		cout<<endl;
	}


	cout<<"the residual network looks like: "<<endl;
	for(i = 0;i<nodeNumber;i++) {
		for(j = 0;j<nodeNumber;j++) {
			cout<<rMatrix[i][j]<<" ";
		}
		cout<<endl;
	}


	cout<<"the height function of each node is like: ";
	for(i = 0;i<nodeNumber;i++)
		cout<<height[i]<<" ";
	cout<<endl;
}
