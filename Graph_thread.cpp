// copy from http://jqyblogger.blogspot.com/2012/06/sample-c-code-for-push-relabel-max-flow.html

#include <climits>
#include "Graph.h"


Graph::Graph()// s and t represent the index of source node and sink node
{
	int i,j;
	// source = s;
	// sink = t;
	cout<<"Please input the graph width, height, and the source and sink nodes"<<endl;
	cin>>W>>H;
	//cin>>source>>sink;
	nodeNumber = W * H;

	int si = 0;
	int sj = 0;
	int ti = H-1;
	int tj = W-1;

	//height = new int[X][Y];
	//excess = new int[X][Y];

	// for(i = 0;i<nodeNumber;i++) {
	// 	height[i] = 0;
	// 	excess[i] = 0;
	// }

	//height[s]  = nodeNumber;//the height function of the source should be changed first

	cout<<"please input the matrix of capacity!"<<endl;
	cMatrixR = new int[H][W];
	cMatrixD = new int[H][W];
	fMatrixR = new int[H][W];
	fMatrixD = new int[H][W];
	rMatrixR = new int[H][W];
	rMatrixD = new int[H][W];
	hMatrix  = new int[H][W];
	eMatrix  = new int[H][W];

	// the input should interleave the capacity R and D line by
	// line
	for (i=0; i<H; i++) {
		for (j=0; j<W; j++) {
			cin>>cMatrixR[i][j];
			rMatrixR[i][j] = cMatrixR[i][j];
		}
		for (j=0; j<W; j++) {
			cin>>cMatrixD[i][j];
			rMatrixD[i][j] = cMatrixD[i][j];
		}
	}

	memset(fMatrixR, 0, sizeof(int)*nodenumber);
	memset(fMatrixD, 0, sizeof(int)*nodenumber);

	fMatrixR[si][sj]  = cMatrixR[si][sj];
	fMatrixD[si][sj]  = cMatrixD[si][sj];
	eMatrix[si][sj+1] = fMatrixR[si][sj]; // (si, sj) -> (si, sj+1), i.e. flow to right
	eMatrix[si+1][sj] = fMatrixD[si][sj]; // (si, sj) -> (si+1, sj), i.e. flow to down	
	eMatrix[si][sj]   = -(fMatrixR[si][sj] + fMatrixD[si][sj]);
	rMatrixR[si][sj]  = 0;
	rMatrixD[si][sj]  = 0;
	hMatrix[si][sj]   = nodenumber;	

	// for(i = 0;i<nodeNumber;i++) {
	// 	if(cMatrix[s][i]!=0) {
	// 		fMatrix[s][i] = cMatrix[s][i];
	// 		fMatrix[i][s] = -1*cMatrix[s][i];
	// 		excess[i] = cMatrix[s][i];
	// 		excess[s]-= cMatrix[s][i];//at first, the excess of source node should be negative. 
	// 		if(i!=t)
	// 			excessNode.push_back(i);//the sink cannot puch flow back!
	// 	}
	// }//all the outgoing links from source should be satuated first
  
        // residualBuilder();//compose the residual network.
	// printInfo();
}

typedef struct _arg_t {
	Graph* g;
	int i;
	int j;
} arg_t

int clk;

void* worker(void* para)
{
	arg_t* arg = (arg_t*)para;
	Graph* g = arg->g;
	int i = arg->i;
	int j = arg->j;

	int cnt = 0;
	
	while (clk >= 0) {
		if (cnt < clk) {
			// do the push relabel work
			// 1. check whether the current node is active
			// 2. check whether the current node can push some flow out
			// 3. if can push, then push
			// 4. if cannot push, then pull, and relabel

			int excess = g->eMatrix[i][j];
			if (excess > 0) { // active
				int** pushMatrixR = g->pushMatrixR;
				int** pushMatrixD = g->pushMatrixD;
				int** hMatrix = g->hMatrix;
				int** eMatrix = g->eMatrix;
				int** rMatrixR = g->rMatrixR;
				int** rMatrixD = g->rMatrixD;
				
				if (g->rMatrixR[i][j] > 0 && hMatrix[i][j] == hMatrix[i][j+1]+1)
					g->pushMatrixR[i][j] += min(excess, g->rMatrixR[i][j]); // push Right
				else if (g->rMatrixD[i][j] > 0 && hMatrix[i][j] == hMatrix[i+1][j]+1)
					g->pushMatrixD[i][j] += min(excess, g->rMatrixD[i][j]); // push Down
				else if (g->rMatrixR[i][j-1] < 0 && hMatrix[i][j] == hMatrix[i][j-1])
					g->pushMatrixR[i][j-1] -= min(excess, g->rMatrixR[i][j-1]); // push Left
				else if (g->rMatrixD[i-1][j] < 0 && hMatrix[i][j] == hMatrix[i-1][j]+1)
					g->pushMatrixD[i-1][j] -= min(excess, g->rMatrixD[i-1][j]); // push Up
				else { // need relabel
					// pull 
					int pv = 0;
					pv += pushMatrixR[i][j-1]; pushMatrixR[i][j-1] = 0;
					pv -= pushMatrixR[i][j]; pushMatrixR[i][j] = 0;
					pv += pushMatrixD[i-1][j]; pushMatrixD[i-1][j] = 0;
					pv -= pushMatrixD[i][j]; pushMatrixD[i][j] = 0;

					// relabel
					int temp = INT_MAX;
					if (rMatrixR[i][j] > 0 && hMatrix[i][j+1] < temp)
						temp = hMatrix[i][j+1];
					else if (rMatrixR[i][j-1] < 0 && hMatrix[i][j-1] < temp)
						temp = hMatrix[i][j-1];
					else if (rMatrixD[i][j] > 0 && hMatrix[i+1][j] < temp)
						temp = hMatrix[i+1][j];
					else if (rMatrixD[i-1][j] < 0 && hMatrix[i-1][j] < temp)
						temp = hMatrix[i-1][j];

					hMatrix[i][j] = temp + 1; // toe paused here
				}
			}

			cnt++;
		}		
		sleep(100);
	}
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
