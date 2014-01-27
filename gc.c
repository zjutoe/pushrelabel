#include <stdio.h>

#define MAX 100

#define min((a), (b)) ( (a)<(b) ? (a) : (b) )

int mt_c[MAX][MAX];
int mt_h[MAX];
int mt_f[MAX][MAX];
int mt_e[MAX];
int mt_cf[MAX][MAX];

void init_preflow(int S, int T, int size)
{
	memset(mt_h, 0, size);
	mt_h[S] = size;

	int i;	

	for (i=0; i<size; i++) {
		int cap = mt_c[S][i];
		if (cap > 0) {
			mt_f[S][i] = cap;
			mt_f[i][S] = -cap;
			mt_e[i] = cap;
			mt_e[S] -= cap;
			mt_cf[S][i] = 0;
			mt_cf[i][S] = 0;
		}
	}

}

void push(u, v)
{
	int temp = min(mt_e[u], mt_cf[u][v]);

	mt_f[u][v] = mt_f[u][v] + temp;
	mt_f[v][u] = -mt_f[u][v];

	mt_e[u] = mt_e[u] - temp;
	mt_e[v] = mt_e[v] + temp;

	mt_cf[u][v] = mt_c[u][v] - f[u][v];
	mt_cf[v][u] = -mt_cf[u][v];
}

void relabel(u, size)
{
	int temp = -1;
	
	int i;
	for (i=0; i<size; i++) {
		if (mt_c[u][i] > 0 && mt_cf[u][i] > 0) {
			if (temp == -1 || temp > mt_h[i])
				temp = mt_h[i]			
		}
	}
	mt_h[u] = 1 + temp
}

