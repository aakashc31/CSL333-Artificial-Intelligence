#include <bits/stdc++.h>
#include <unistd.h>
#include "Graph.h"
#include <random>
using namespace std;

struct Move
{
	int choice;
	pii loc; //location
};

bool operator<(const Move& m1, const Move& m2){
	if(m1.choice == m2.choice){
		return m1.loc < m2.loc;
	}
	return m1.choice < m2.choice;
}

bool operator==(const Move& m1, const Move& m2){
	return m1.choice == m2.choice && m1.loc == m2.loc;
}

void print(Move &m){
	printf("choice: %d\nLoc: %d,%d\n",m.choice, m.loc.first, m.loc.second);
}

bool min_comparator(const pair<Move, float>& p1, const pair<Move, float>& p2){
	return p1.second < p2.second;
}

bool max_comparator(const pair<Move, float>& p1, const pair<Move, float>& p2){
	return p1.second > p2.second;
}

class State
{
public:
	pii opponent; //location of opponent
	pii myloc; //my location on board
	vector<pair<int,pii> > walls; //list of all walls
	Graph *graph;
	float timeRem;
	int oppWalls, myWalls;
	int goal; //destination row
	bool won;
	int gameStatus; //31 if MAX has won, 32 if MIN has won
	int player; //from running game's perspective
	default_random_engine generator;
  	uniform_real_distribution<double> distribution;

	// this is for checking oscillations
	queue<Move> buffer;
	bool osc;
	Move osc_block; 

	int numMoves;
	Move initMoves[3];

	time_t time_init;	
	State(int n, int m, pii initOpp, pii initMe, int K, float _time){
		graph = new Graph(n,m);
		opponent = initOpp;
		goal = opponent.first;
		myloc = initMe;
		walls.resize(0);
		oppWalls = myWalls = K;
		timeRem = _time;
		srand(time(NULL));
		won = false;
		gameStatus = 3;
		player = (goal == 9)?1:2;
		osc = false;
		distribution = uniform_real_distribution<double>(0.0,2.0);
		numMoves = 0;
		if(player == 1){
			initMoves[0] = (Move){0,mp(2,5)};
			initMoves[1] = (Move){0,mp(3,5)};
			initMoves[2] = (Move){0,mp(4,5)};
		}
		else{
			initMoves[0] = (Move){0,mp(8,5)};
			initMoves[1] = (Move){0,mp(7,5)};
			initMoves[2] = (Move){0,mp(6,5)};
		}
	}

	void printState(){
		printf("\n\n\nCurrent state:\n");
		printf("Player: %d\n", player);
		printf("Goal: %d\n",goal);
		printf("Opponent :%d, %d\n", opponent.first, opponent.second);
		printf("My location: %d, %d\n", myloc.first, myloc.second);
		printf("Walls: \n");
		for(int i=0; i<walls.size(); i++){
			if(walls[i].first == 1)
				printf("Horizontal: ");
			else
				printf("Vertical: ");
			printf("%d, %d\n",walls[i].second.first, walls[i].second.second);
		}
		printf("Game status: %d\n",gameStatus );
		printf("\n\n\n");
	}
	
	// Returns the possible set of moves from this state for the player
	vector<Move> possibleMovesHelper(pii meraLoc, pii oppKaLoc, bool hasWon, bool hasOppWon, int wallsRem, int meraGoal){
		vector<Move> moves;
		if(wallsRem == 0)
			goto later;
		for (int i=2;i<=9;i++){
			for (int j=2;j<=9;j++){
				if (!graph->isHorizontalWall(mp(i,j)) && !graph->isHorizontalWall(mp(i,j-1)) && !graph->isHorizontalWall(mp(i,j+1)) && !graph->isVerticalWall(mp(i,j))){
					graph->placeHorizontalWall(mp(i,j));

					bool myflag = false, oppflag = false;
					
					if (hasWon || (graph->checkPath(meraLoc,meraGoal, "possibleMovesHelper 1") > 0)){
						myflag = true;
					}

					if(hasOppWon || (graph->checkPath(oppKaLoc, meraGoal==9?1:9, "possibleMovesHelper 2") > 0)){
						oppflag = true;
					}
					
					
					if(myflag && oppflag)
						moves.pb((Move){1,mp(i,j)});
					graph->removeHorizontalWall(mp(i,j));
				}

				if (!graph->isVerticalWall(mp(i,j)) && !graph->isVerticalWall(mp(i-1,j)) && !graph->isVerticalWall(mp(i+1,j)) && !graph->isHorizontalWall(mp(i,j))){
					graph->placeVerticalWall(mp(i,j));
					bool myflag = false, oppflag = false;
					if (hasWon || (graph->checkPath(meraLoc,meraGoal, "possibleMovesHelper 3") > 0)){
						myflag = true;
					}
					if(hasOppWon || (graph->checkPath(oppKaLoc, meraGoal==9?1:9, "possibleMovesHelper 4") > 0)){
						oppflag = true;
					}
					if(myflag && oppflag)
						moves.pb((Move){2,mp(i,j)});
					graph->removeVerticalWall(mp(i,j));
				}
			}
		}

		later:
		if(hasWon) //means I have won
		{
			moves.pb((Move){0,meraLoc});
			return moves;
		}
		if (graph->getNode(meraLoc,'u')){
			if (oppKaLoc==mp(meraLoc.first-1,meraLoc.second)){
				if (graph->getNode(oppKaLoc,'u')){
					moves.pb((Move){0,mp(meraLoc.first-2,meraLoc.second)});
				}
				else{
					if (graph->getNode(oppKaLoc,'l')){
						moves.pb((Move){0,mp(oppKaLoc.first,oppKaLoc.second-1)});
					}
					if (graph->getNode(oppKaLoc,'r')){
						moves.pb((Move){0,mp(oppKaLoc.first,oppKaLoc.second+1)});
					}
				}
			}
			else{
				moves.pb((Move){0,mp(meraLoc.first-1,meraLoc.second)});
			}
		}
		if (graph->getNode(meraLoc,'d')){
			if (oppKaLoc==mp(meraLoc.first+1,meraLoc.second)){
				if (graph->getNode(oppKaLoc,'d')){
					moves.pb((Move){0,mp(meraLoc.first+2,meraLoc.second)});
				}
				else{
					if (graph->getNode(oppKaLoc,'l')){
						moves.pb((Move){0,mp(oppKaLoc.first,oppKaLoc.second-1)});
					}
					if (graph->getNode(oppKaLoc,'r')){
						moves.pb((Move){0,mp(oppKaLoc.first,oppKaLoc.second+1)});
					}
				}
			}
			else{
				moves.pb((Move){0,mp(meraLoc.first+1,meraLoc.second)});
			}
		}
		if (graph->getNode(meraLoc,'l')){
			if (oppKaLoc==mp(meraLoc.first,meraLoc.second-1)){
				if (graph->getNode(oppKaLoc,'l')){
					moves.pb((Move){0,mp(meraLoc.first,meraLoc.second-2)});
				}
				else{
					if (graph->getNode(oppKaLoc,'u')){
						moves.pb((Move){0,mp(oppKaLoc.first-1,oppKaLoc.second)});
					}
					if (graph->getNode(oppKaLoc,'d')){
						moves.pb((Move){0,mp(oppKaLoc.first+1,oppKaLoc.second)});
					}
				}
			}
			else{
				moves.pb((Move){0,mp(meraLoc.first,meraLoc.second-1)});
			}
		}
		if (graph->getNode(meraLoc,'r')){
			if (oppKaLoc==mp(meraLoc.first,meraLoc.second+1)){
				if (graph->getNode(oppKaLoc,'r')){
					moves.pb((Move){0,mp(meraLoc.first,meraLoc.second+2)});
				}
				else{
					if (graph->getNode(oppKaLoc,'u')){
						moves.pb((Move){0,mp(oppKaLoc.first-1,oppKaLoc.second)});
					}
					if (graph->getNode(oppKaLoc,'d')){
						moves.pb((Move){0,mp(oppKaLoc.first+1,oppKaLoc.second)});
					}
				}
			}
			else{
				moves.pb((Move){0,mp(meraLoc.first,meraLoc.second+1)});
			}
		}
		// up, down, left, right, all walls s.t. path exists
		return moves;
	}

	vector<Move> possibleMoves(bool player){
		if(player == PLAYER_MAX)
			return possibleMovesHelper(myloc, opponent, gameStatus==31, gameStatus==32, myWalls, goal);
		else
			return possibleMovesHelper(opponent, myloc, gameStatus==32, gameStatus==31, oppWalls, goal==9?1:9);
	}

	// Out of all the possible moves this function will return the best one
	Move getNextMove(){
		time_init = time(NULL);
		numMoves++;
		if(buffer.size() == 8){
			Move ar[8];
			for(int i=0; i<8; i++){
				ar[i] = buffer.front();
				buffer.pop();
			}
			if(ar[0] == ar[4] && ar[2] == ar[6] && ar[1] == ar[5] && ar[3] == ar[7]){
				osc = true;
				osc_block = ar[0];
			}
			for(int i=0; i<8; i++)
				buffer.push(ar[i]);
		}

		if(timeRem < 5){//upto 2 depth
			Move r = miniMax(0,(1<<31), INT_MAX, 2).first;
			applyMove(r,PLAYER_MAX,true);
			osc = false;
			return r;
		}
		else{
			Move r = miniMax(0,(1<<31), INT_MAX, 2).first;
			applyMove(r,PLAYER_MAX,true);
			osc = false;
			return r;
		}
	}

	// update state based on opposition's move
	void applyMove(Move m, bool player, bool commit){
		if(commit){
			buffer.push(m);
			while(buffer.size() > 8)
				buffer.pop();
		}
		if(m.choice == 0){
			if(player == PLAYER_MAX) //I will not update myloc after winning
			{
				myloc = m.loc;
			}
			else if(gameStatus != 32) //if I have lost the game then it does not make sense to update opponents loc
				opponent = m.loc;
		}
		else if(m.choice == 1){ //place horizontal wall
			if(commit)
				walls.pb(mp(1,m.loc));
			graph->placeHorizontalWall(m.loc);
			if(player == PLAYER_MAX){ 
				myWalls--;
			}
			else
				oppWalls--;
		}
		else if(m.choice == 2){
			if(commit)
				walls.pb(mp(2,m.loc));
			graph->placeVerticalWall(m.loc);
			if(player == PLAYER_MAX){
				myWalls--;
			}
			else
				oppWalls--;
		}
		else
			printf("Wrong choice in applyMove()\n");
	}

	// undo the changes made in current state by move m of player
	void revertMove(Move m, bool player){
		if(m.choice == 0){ //if pawn has been moved, store the old location in Move.loc
			if(player == PLAYER_MAX)
			{
				myloc = m.loc;
			}
			else
				opponent = m.loc;
		}
		else if(m.choice == 1){ //place horizontal wall
			graph->removeHorizontalWall(m.loc);
			if(player == PLAYER_MAX){ 
				myWalls++;
			}
			else
				oppWalls++;
		}
		else if(m.choice == 2){
			graph->removeVerticalWall(m.loc);
			if(player == PLAYER_MAX){
				myWalls++;
			}
			else
				oppWalls++;
		}
		else
			printf("Wrong choice in applyMove()\n");
	}

	// this function will return the evaluation value of the current state
	float eval(){
		float mydist = gameStatus == 31 ? 0 : graph->checkPath(myloc, goal, "eval() mydist"); //my distance to 
		float oppdist = gameStatus == 32 ? 0 : graph->checkPath(opponent, goal==9?1:9, "eval() oppdist");
		// float ret = (oppdist - mydist) + ((myloc.first == goal) ? 20:0) + ((opponent.first == (goal==9?1:9)) ? -20:0);
		float maxWin = (myloc.first == goal) ? 1:0;
		float minWin = ((opponent.first == (goal==9?1:9)) ? -1:0); 
		
		float f1 = (player==1) ? myloc.first - 1 : 9 - myloc.first;
		
		float f2 = f1 - ((player == 1)? 9 - opponent.first : opponent.first - 1);
		
		int maxNextCol = (player == 1)? myloc.first+1 : myloc.first - 1;
		float f3 = (gameStatus == 31 || myloc.first == goal) ? 0 : graph->checkPath(myloc, maxNextCol, "eval() f3");
		
		
		int minNextCol = (player == 2)? opponent.first + 1 : opponent.first - 1;
		float f4 = (gameStatus == 32 || opponent.first == (goal==9?1:9)) ? 0 : graph->checkPath(opponent, minNextCol, "eval() f4");
		
		float w1 = 0.6, w2 = 0.6001, w3 = -0.1, w4 = 6.52;
		// float w1 = 0.6, w2 = 0.6001, w3 = 14.45, w4 = 6.52;
		
		float ret = w3*f3 + w4*f4 + (1.5*oppdist - 4*mydist) + 20*maxWin + 20*minWin + distribution(generator);
		// float ret = w1*f1 + w2*f2 + w3/f3 + w4*f4 + distribution(generator);
		return ret;
	}

	float eval_helper(Move m, bool player){

		pii oldloc = mp(0,0);
		if(m.choice == 0)
			oldloc = (player == PLAYER_MIN) ? opponent : myloc;
		// apply move
		applyMove(m, player, false);

		// float mydist = gameStatus == 31 ? 0 : graph->checkPath(myloc, goal, "eval_helper() mydist"); //my distance to 
		// float oppdist = gameStatus == 32 ? 0 : graph->checkPath(opponent, goal==9?1:9, "eval_helper() oppdist");
		
		float ret = eval();
		// oppdist - mydist + distribution(generator);
		

		if(m.choice==0)
			revertMove((Move){0,oldloc}, player);
		else
			revertMove(m, player);

		return ret;

	}

	#define NUM_CHILD 100
	pair<Move,int> miniMax(int d, int alpha, int beta, int max_depth){
		if(d == max_depth){
			return mp((Move){3, mp(0, 0)}, eval());
		}
		if(d&1){//MIN
			vector<Move> moves = possibleMoves(PLAYER_MIN);
			int best = INT_MAX;
			Move bestMove;

			vector<pair<Move,float> > new_moves1;
			for(int i=0; i<moves.size(); i++){
				new_moves1.pb(mp(moves[i], eval_helper(moves[i], PLAYER_MIN)));
			}
			sort(new_moves1.begin(), new_moves1.end(), min_comparator);
			if(new_moves1.size() > NUM_CHILD)
				new_moves1.resize(NUM_CHILD);

			vector<Move> new_moves;
			for(int i=0; i<new_moves1.size(); i++)
				new_moves.pb(new_moves1[i].first);
			// iterating on all children
			for(int i=0; i<new_moves.size(); i++){

				// store old location if the move involves moving the spawn
				pii oldloc = mp(0,0);
				if(new_moves[i].choice == 0)
					oldloc = opponent;

				// apply move
				applyMove(new_moves[i], PLAYER_MIN, false);

				// call next
				pair<Move, int> ret = miniMax(d+1, alpha, beta, max_depth);

				// maintain best move and best eval value
				if(ret.second < best){
					best = ret.second;
					bestMove = new_moves[i];
				}

				// if spawn moved, revert using oldloc
				if(new_moves[i].choice==0)
					revertMove((Move){0,oldloc}, PLAYER_MIN);
				else
					revertMove(new_moves[i], PLAYER_MIN);

				// alpha beta pruning chutiyapa
				beta = min(beta, ret.second);
				if(timeRem - (time(NULL) - time_init) <= 5){
					// printf("YOLO************8\n");
					break;			
				if(alpha >= beta)
					break;	
				}
			}


			return mp(bestMove, best);
		}
		else{//MAX

			vector<Move> moves = possibleMoves(PLAYER_MAX);
			if(d == 0 && osc){
				vector<Move>::iterator it = find(moves.begin(), moves.end(), osc_block);
				if(it != moves.end())
				{
					if(moves.size() > 1)
						moves.erase(it);
				}
			}

			vector<pair<Move,float> > new_moves1;
			for(int i=0; i<moves.size(); i++){
				new_moves1.pb(mp(moves[i], eval_helper(moves[i], PLAYER_MAX)));
			}
			sort(new_moves1.begin(), new_moves1.end(), max_comparator);
			if(new_moves1.size() > NUM_CHILD)
				new_moves1.resize(NUM_CHILD);

			vector<Move> new_moves;
			for(int i=0; i<new_moves1.size(); i++)
				new_moves.pb(new_moves1[i].first);
			// preveinting oscillation

			// osc code 
			int best = 1<<31;
			Move bestMove;
			for(int i=0; i<new_moves.size(); i++){
				pii oldloc = mp(0,0);
				if(new_moves[i].choice == 0)
					oldloc = myloc;
				applyMove(new_moves[i],PLAYER_MAX, false);
				pair<Move, int> ret = miniMax(d+1, alpha, beta, max_depth);
				if(ret.second > best){
					best = ret.second;
					bestMove = new_moves[i];
				}
				if(new_moves[i].choice==0)
					revertMove((Move){0,oldloc}, PLAYER_MAX);
				else
					revertMove(new_moves[i], PLAYER_MAX);
				alpha = max(alpha, ret.second);
				if(alpha >= beta)
					break;
				if(timeRem - (time(NULL) - time_init) <= 5){
					// printf("YOYOYOYO**************\n");
					break;
				}
			}
			return mp(bestMove, best);
		}
	}
};