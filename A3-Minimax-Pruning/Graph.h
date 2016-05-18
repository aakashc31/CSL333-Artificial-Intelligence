#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;

#ifndef __GRAPH__H
#define __GRAPH__H
#define g(n) scanf("%d",&n)
#define gl(n) scanf("%lld", &n)
#define f(i,n) for(int i=0; i<n; i++)
#define pb push_back
#define mp make_pair
#define fab(i,a,b) for(int i=a; i<=b; i++)
#define test(t) while(t--)
#define getcx getchar//_unlocked
#define PLAYER_MAX true
#define PLAYER_MIN false


typedef long long int lli;
typedef pair<int,int> pii;
typedef vector<int> vi;
typedef vector< vi > vvi;

struct Node
{
	bool up,down, left, right;
};

Node makeNode(bool x1, bool x2, bool x3, bool x4){
	Node t = {x1,x2,x3,x4};
	return t;
}


class Graph
{
public:
	int N, M;
	vector< vector<Node> > board;
	int MAXSIZE = 1000;
	pii q[1000];
	Graph(){
		N = 0;
		M = 0;
		board.resize(0);
	}

	Graph(int x1, int x2){

		/********************** Initializing size of the board **********************/
		N = x1, M = x2;
		board.resize(N+1);
		for(int i=1; i<= N; i++){
			board[i].resize(M+1);
		}

		/*********************** Initializing graph *********************/ 
		for(int i=1; i<= N; i++){
			for(int j=1; j<=M; j++){
				board[i][j] = makeNode(1,1,1,1);
				if(i==1) board[i][j].up = false;
				if(i==N) board[i][j].down = false;
				if(j==1) board[i][j].left = false;
				if(j==M) board[i][j].right = false;
			}
		}
	}
	
	/*********************** Checks if node is valid **********/
	bool validNode(pii n){
		return n.first >= 1 && n.first <= N && n.second >= 1 && n.second <= M;
	}

	inline bool getNode(pii n, char ch){
		int i=n.first, j=n.second;
		assert(validNode(n));
		if(ch=='l')
			return board[i][j].left;
		else if (ch == 'r')
			return board[i][j].right;
		else if(ch=='u')
			return board[i][j].up;
		else if(ch=='d')
			return board[i][j].down;
		else{
			printf("Invalid choice in getNode()\n");
			return false;
		}

	}

	/***************** update the node's neighbour value in board, neighbr is def by choice ch ****/
	void updateNode(pii n, char ch, bool newVal){
		int i=n.first, j=n.second;
		assert(validNode(n));
		if(ch == 'l')
			board[i][j].left = newVal;
		else if(ch=='r')
			board[i][j].right = newVal;
		else if(ch=='u')
			board[i][j].up = newVal;
		else if(ch=='d')
			board[i][j].down = newVal;
		else
			printf("Invalid choice in updateNode()\n");
	}

	/*********************** Removes the edge between node n1 and n2 **********/
	void removeEdge(pii n1, pii n2){ 
		assert(validNode(n1));
		assert(validNode(n2));

		//n1 ke left me n2
		if(n1.second - 1 == n2.second){
			updateNode(n1, 'l', 0);
			updateNode(n2, 'r', 0);
		}
		else if(n1.second + 1 == n2.second){
			updateNode(n1, 'r', 0);
			updateNode(n2, 'l', 0);
		}
		else if(n1.first - 1 == n2.first){
			updateNode(n1, 'u', 0);
			updateNode(n2, 'd', 0);
		}
		else if(n1.first + 1 == n2.first){
			updateNode(n1, 'd', 0);
			updateNode(n2, 'u', 0);
		}
		else{
			printf("Non neighbour nodes in  removeEdge()\n");
		}
	}	

	/*********************** Adds the edge between node n1 and n2 **********/
	void addEdge(pii n1, pii n2){ 
		assert(validNode(n1));
		assert(validNode(n2));

		//n1 ke left me n2
		if(n1.second - 1 == n2.second){
			updateNode(n1, 'l', 1);
			updateNode(n2, 'r', 1);
		}
		else if(n1.second + 1 == n2.second){
			updateNode(n1, 'r', 1);
			updateNode(n2, 'l', 1);
		}
		else if(n1.first - 1 == n2.first){
			updateNode(n1, 'u', 1);
			updateNode(n2, 'd', 1);
		}
		else if(n1.first + 1 == n2.first){
			updateNode(n1, 'd', 1);
			updateNode(n2, 'u', 1);
		}
		else{
			printf("Non neighbour nodes in  addEdge()\n");
		}
	}

	/***********************Returns true if they are adjacent and connected, o.w. false ***/
	bool isEdge(pii n1, pii n2){
		assert(validNode(n1));
		assert(validNode(n2));

		//n1 ke left me n2
		if(n1.second - 1 == n2.second){
			return getNode(n1,'l');
		}
		else if(n1.second + 1 == n2.second){
			return getNode(n1,'r');
		}
		else if(n1.first - 1 == n2.first){
			return getNode(n1,'u');
		}
		else if(n1.first + 1 == n2.first){
			return getNode(n1,'d');
		}
		else{
			printf("Non neighbour nodes in  addEdge()\n");
		}
	}

	/*********************** Checks if there is a path b/w n1 and row and returns shortest distance, if no path : -1 **********/
	int checkPath(pii n1, int row, const char* message){ 
		assert(validNode(n1));
		if(row > 9 || row < 1){
			printf("Wrong row value in checkPath() \nMessage: %s\nn1 = %d,%d\nrow = %d\n\n", message,n1.first,n1.second,row);
			return -1;
		}
		bool isVisited[N+1][M+1];
		memset(isVisited, 0, sizeof isVisited);
		
		int qsize = 0;
		int head = 0, tail = 0;
		q[head++] = n1;
		head = head % MAXSIZE;
		qsize++;

		isVisited[n1.first][n1.second] = true;
		pii flag = mp(-1,-1);
		q[head++] = flag;
		head = head % MAXSIZE;
		qsize++;

		int dist = 0;
		int j=0;
		while(qsize > 1){
			for(j=1; j<= 9; j++){
				if(isVisited[row][j])
					goto Outer;
			}
			while(q[tail] != flag){
				pii s = q[tail];
				tail = (tail + 1)%MAXSIZE;
				qsize--;

				if(getNode(s,'u')){
		        	if(!isVisited[s.first-1][s.second]){
		        		isVisited[s.first-1][s.second] = true;
		        		q[head++] = mp(s.first-1,s.second);
						head = head % MAXSIZE;
						qsize++;
		        	}
		        }
		        if(getNode(s,'d')){
		        	if(!isVisited[s.first+1][s.second]){
		        		isVisited[s.first+1][s.second] = true;
		        		q[head++] = mp(s.first+1,s.second);
						head = head % MAXSIZE;
						qsize++;
		        	}
		        }
		        if(getNode(s,'l')){
		        	if(!isVisited[s.first][s.second-1]){
		        		isVisited[s.first][s.second-1] = true;
		        		q[head++] = mp(s.first,s.second-1);
						head = head % MAXSIZE;
						qsize++;
		        	}
		        }
		        if(getNode(s,'r')){
		        	if(!isVisited[s.first][s.second+1]){
		        		isVisited[s.first][s.second+1] = true;
		        		q[head++] = mp(s.first,s.second+1);
						head = head % MAXSIZE;
						qsize++;
		        	}
		        }
			}

			dist++;
			tail = (tail + 1)%MAXSIZE;
			qsize--;
			q[head++] = flag;
			head = head % MAXSIZE;
			qsize++;
	        
		}
		Outer:
		if(j==10)
			return -1;
		return isVisited[row][j] ? dist : -1;
	}

	void placeHorizontalWall(pii w){
		assert(2<=w.first && w.first <= N && 2<=w.second && w.second <= M);
		int x = w.first, y = w.second;
		removeEdge(mp(x,y), mp(x-1,y));
		removeEdge(mp(x-1,y-1), mp(x,y-1));
	}

	void removeHorizontalWall(pii w){
		assert(2<=w.first && w.first <= N && 2<=w.second && w.second <= M);
		int x = w.first, y = w.second;
		addEdge(mp(x,y), mp(x-1,y));
		addEdge(mp(x-1,y-1), mp(x,y-1));
	}

	bool isHorizontalWall(pii w){
		if(2<=w.first && w.first <= N && 2<=w.second && w.second <= M){
			int x = w.first, y = w.second;
			return (!isEdge(mp(x,y), mp(x-1,y)) && !isEdge(mp(x-1,y-1), mp(x,y-1)));
		}
		return false;
	}

	void placeVerticalWall(pii w){
		assert(2<=w.first && w.first <= N && 2<=w.second && w.second <= M);
		int x = w.first, y = w.second;
		removeEdge(mp(x,y), mp(x,y-1));
		removeEdge(mp(x-1,y-1), mp(x-1,y));
	}

	void removeVerticalWall(pii w){
		assert(2<=w.first && w.first <= N && 2<=w.second && w.second <= M);
		int x = w.first, y = w.second;
		addEdge(mp(x,y), mp(x,y-1));
		addEdge(mp(x-1,y-1), mp(x-1,y));
	}

	bool isVerticalWall(pii w){
		if(2<=w.first && w.first <= N && 2<=w.second && w.second <= M){
			int x = w.first, y = w.second;
			return (!isEdge(mp(x,y), mp(x,y-1)) && !isEdge(mp(x-1,y-1), mp(x-1,y)));
		}
		return false;
	}


};

#endif

