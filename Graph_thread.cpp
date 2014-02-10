// copy from http://jqyblogger.blogspot.com/2012/06/sample-c-code-for-push-relabel-max-flow.html

#include <climits>
#include<pthread.h>
#include <deque>
#include "Graph.h"


Graph::Graph()// s and t represent the index of source node and sink node
{
	clk = 0;		// global clock

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
	cMatrixL = new int[H][W];
	cMatrixU = new int[H][W];

	fMatrixR = new int[H][W];
	fMatrixD = new int[H][W];
	fMatrixL = new int[H][W];
	fMatrixU = new int[H][W];

	rMatrixR = new int[H][W];
	rMatrixD = new int[H][W];
	rMatrixL = new int[H][W];
	rMatrixU = new int[H][W];

	hMatrix  = new int[H][W];
	eMatrix  = new int[H][W];

	// the input should interleave the capacity R and D line by
	// line
	for (i=0; i<H; i++) {
		int c;

		for (j=0; j<W; j++) {
			cin>>c;
			cMatrixR[i][j] = c;
			cMatrixL[i][j] = -c;
			
			//rMatrixR[i][j] = cMatrixR[i][j];
		}
		for (j=0; j<W; j++) {
			cin>>c;
			cMatrixD[i][j] = c;
			cMatrixU[i][j] = -c;
			
			//rMatrixD[i][j] = cMatrixD[i][j];
		}
	}
	

	memset(fMatrixR, 0, sizeof(int)*nodeNumber);
	memset(fMatrixD, 0, sizeof(int)*nodeNumber);
	memset(fMatrixL, 0, sizeof(int)*nodeNumber);
	memset(fMatrixU, 0, sizeof(int)*nodeNumber);

	fMatrixR[si][sj]  = cMatrixR[si][sj];
	fMatrixD[si][sj]  = cMatrixD[si][sj];
	fMatrixL[si][sj]  = -fMatrixR[si][sj];
	fMatrixU[si][sj]  = -fMatrixD[si][sj];

	eMatrix[si][sj+1] = fMatrixR[si][sj]; // (si, sj) -> (si, sj+1), i.e. flow to right
	eMatrix[si+1][sj] = fMatrixD[si][sj]; // (si, sj) -> (si+1, sj), i.e. flow to down	
	eMatrix[si][sj]   = -(fMatrixR[si][sj] + fMatrixD[si][sj]);

	hMatrix[si][sj]   = nodeNumber;

	// build residual 
	int f;
	for (i=0; i<H; i++) {
		for (j=0; j<W; j++) {
			f = fMatrixR[i][j];
			c = cMatrixR[i][j];
			rMatrixR[i][j] = f>=0 ? c-f : -f;

			f = fMatrixL[i][j];
			c = cMatrixL[i][j];
			rMatrixR[i][j] = f>=0 ? c-f : -f;

			f = fMatrixD[i][j];
			c = cMatrixD[i][j];
			rMatrixR[i][j] = f>=0 ? c-f : -f;

			f = fMatrixU[i][j];
			c = cMatrixU[i][j];
			rMatrixR[i][j] = f>=0 ? c-f : -f;
		}
	}

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
	
	cout<<"worker ("<<i<<","<<j<<") start"<<endl;

	while (clk >= 0) {
		if (cnt < clk) {
			// do the push relabel work
			// 1. check whether the current node is active
			// 2. check whether the current node can push some flow out
			// 3. if can push, then push
			// 4. if cannot push, then pull, and relabel
			
			if (clk % 2) { // we only pull in odd clock cycles
				int pv = 0;
				if (j>0 && g->pushMatrixR[i][j-1]>0) {
					pv += g->pushMatrixR[i][j-1];
					g->pushMatrixR[i][j-1] = 0;
				}
				if (j<W-1 && g->pushMatrixL[i][j+1]>0) {
					pv += g->pushMatrixL[i][j+1];
					g->pushMatrixL[i][j+1] = 0;
				}
				if (i>0 && g->pushMatrixD[i-1][j]>0) {
					pv += g->pushMatrixD[i-1][j];
					g->pushMatrixD[i-1][j] = 0;
				}
				if (i<H-1 && g->pushMatrixU[i+1][j]>0) {
					pv += g->pushMatrixU[i+1][j];
					g->pushMatrixU[i+1][j] = 0;
				}

				eMatrix[i][j] += pv;
				
			} else {
				int excess = g->eMatrix[i][j];
				if (excess > 0) { // active
					int** pushMatrixR = g->pushMatrixR;
					int** pushMatrixL = g->pushMatrixL;
					int** pushMatrixD = g->pushMatrixD;
					int** pushMatrixU = g->pushMatrixU;
					int** hMatrix = g->hMatrix;
					int** eMatrix = g->eMatrix;
					int** rMatrixR = g->rMatrixR;
					int** rMatrixD = g->rMatrixD;
					int** rMatrixL = g->rMatrixL;
					int** rMatrixU = g->rMatrixU;
					
					int r, d, l, u;
					r = g->push_R(i, j);
					d = g->push_D(i, j);
					l = g->push_L(i, j);
					u = g->push_U(i, j);
					if (r==-1 && d==-1 && l==-1 && u==-1) {
						// need relabel
						
						int temp = INT_MAX;
						if (rMatrixR[i][j] > 0 && hMatrix[i][j+1] < temp)
							temp = hMatrix[i][j+1];
						else if (rMatrixR[i][j-1] < 0 && hMatrix[i][j-1] < temp)
							temp = hMatrix[i][j-1];
						else if (rMatrixD[i][j] > 0 && hMatrix[i+1][j] < temp)
							temp = hMatrix[i+1][j];
						else if (rMatrixD[i-1][j] < 0 && hMatrix[i-1][j] < temp)
							temp = hMatrix[i-1][j];

						hMatrix[i][j] = temp + 1;
					}
				}
			}

			cnt++;	// catch up wit the current clock
		}		
		std::this_thread::sleep_for (std::chrono::seconds(1));
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

int Graph::do_push_R(int i, int j, int delta)
{
	pushMatrixR[i][j] += delta;
	pushMatrixL[i][j+1] -= delta;

	rMatrixR[i][j]   -= delta;
	eMatrix[i][j]    -= delta;
	rMatrixL[i][j+1] += delta;
	// eMatrix[i][j+1] += delta;  do in pull stage

	if (cMatrixR[i][j] > 0) {
		fMatrixR[i][j] += delta;
		fMatrixL[i][j+1] = -fMatrixR[i][j];
	} else {
		fMatrixL[i][j+1] -= delta;
		fMatrixR[i][j] = -fMatrixL[i][j+1];
	}

	return 0;
}

int Graph::do_push_L(int i, int j, int delta)
{
	return do_push_R(i, j-1, -delta);
}

int Graph::do_push_D(int i, int j, int delta)
{
	pushMatrixD[i][j] += delta;
	pushMatrixU[i+1][j] -= delta;

	rMatrixD[i][j]   -= delta;
	eMatrix[i][j]    -= delta;
	rMatrixU[i+1][j] += delta;
	// eMatrix[i+1][j] += delta;  do in pull stage

	if (cMatrixD[i][j] > 0) {
		fMatrixD[i][j] += delta;
		fMatrixU[i+1][j] = -fMatrixD[i][j];
	} else {
		fMatrixU[i+1][j] -= delta;
		fMatrixD[i][j] = -fMatrixU[i][j+1];
	}

	return 0;
}

int Graph::do_push_U(int i, int j, int delta)
{
	return do_push_D(i-1, j, -delta);
}

int Graph::push_R(int i, int j)
{
	if (j >= W-1) return -1;
	if (hMatrix[i][j] != hMatrix[i][j+1] + 1) return -1;

	int excess   = eMatrix[i][j];
	int residual = rMatrixR[i][j];
	int delta    = (excess <= residual) ? excess : residual;

	return do_push_R(i, j, delta);

	// pushMatrixR[i][j] += delta;
	// pushMatrixL[i][j+1] -= delta;

	// rMatrixR[i][j]   -= delta;
	// eMatrix[i][j]    -= delta;
	// rMatrixL[i][j+1] += delta;
	// // eMatrix[i][j+1] += delta;  do in pull stage

	// if (cMatrixR[i][j] > 0) {
	// 	fMatrixR[i][j] += delta;
	// 	fMatrixL[i][j+1] = -fMatrixR[i][j];
	// } else {
	// 	fMatrixL[i][j+1] -= delta;
	// 	fMatrixR[i][j] = -fMatrixL[i][j+1];
	// }
}

int Graph::push_L(int i, int j)
{
	if (j <= 0) return -1;
	if (hMatrix[i][j] != hMatrix[i][j-1] + 1) return -1;

	int excess   = eMatrix[i][j];
	int residual = rMatrixR[i][j];
	int delta    = (excess <= residual) ? excess : residual;

	return do_push_L(i, j, delta);
}

int Graph::push_D(int i, int j)
{
	if (i >= H-1) return -1;
	if (hMatrix[i][j] != hMatrix[i+1][j] + 1) return -1;

	int excess   = eMatrix[i][j];
	int residual = rMatrixR[i][j];
	int delta    = (excess <= residual) ? excess : residual;

	return do_push_D(i, j, delta);
}

int Graph::push_U(int i, int j)
{
	if (i <= 0) return -1;
	if (hMatrix[i][j] != hMatrix[i-1][j] + 1) return -1;

	int excess   = eMatrix[i][j];
	int residual = rMatrixR[i][j];
	int delta    = (excess <= residual) ? excess : residual;

	return do_push_U(i, j, delta);
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
	// int current,i,j,target;
	// while(excessNode.size()!=0) {
	// 	current = excessNode.at(0);
	// 	excessNode.pop_front();
	// 	target = checkNode(current);
	// 	if(checkNode(current)<0) {
	// 		relable(current);
	// 	} else {
	// 		push(current,target);
	// 	}
	// }

	deque<pthread_t*> threads;

	int i, j;
	for (i=0; i<H; i++) {
		for (j=0; j<W; j++) {
			pthread_t thd;
			arg_t arg;
			arg.g = this;
			arg.i = i;
			arg.j = j;
			pthread_create(&thd, NULL, worker, NULL, &arg);
			threads.push_back(&thd);
		}
	}

	for (i=0; i<5; i++) {
		clk++;
		std::this_thread::sleep_for(std::chrono::seconds(3));
	}

	// cout<<"the final flow is like:"<<endl;
	// for(i = 0;i<nodeNumber;i++) {
	// 	for(j = 0;j<nodeNumber;j++) {
	// 		cout<<fMatrix[i][j]<<" ";
	// 	}
	// 	cout<<endl;
	// }
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
