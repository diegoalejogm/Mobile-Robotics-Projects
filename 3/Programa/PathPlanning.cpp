#include "Configuration.h"
#include "IntegerPoint2D.h"
#include "PathPlanning.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <queue>

#define SAFETY_DISTANCE true
#define FIRST_TRY_TO_SPIN true
#define PATH_SHORTENER false
#define CELL_SIZE 400
#define PI acos(-1.0)
using namespace std;

struct State {
	int px1, py1, px2, py2;
} startState, endState;

Configuration startposition[2], endposition[2];
vector<Configuration> solRobot1, solRobot2;

string input[20][20];
int dimX, dimY;

bool bfsPosition[20][20][20][20];
int bfsTrace[20][20][20][20][2];
queue <State> bfsQueue;


const int dirx[] = {0,0,0,1,-1,1,1,-1,-1};
const int diry[] = {0,1,-1,0,0,1,-1,1,-1};
const int angle[] = {0,0,180,270,90,315,225,45,135};
map<int, int> angleToDir;

ofstream debug;

void parseInput() {
	ifstream inputFile;
	inputFile.open("escena.txt");
	string ax1, ax2, ax3, ax4;
	inputFile >> ax1 >> dimX;
	inputFile >> ax1 >> dimY;
	inputFile >> ax1 >> ax2 >> ax3 >> ax4;
	startposition[0].position = IntegerPoint2D(std::stoi(ax2.substr(0, ax2.length() - 1)), std::stoi(ax3.substr(0, ax3.length() - 1)));
	startposition[0].angle = std::stoi(ax4);

	inputFile >> ax1 >> ax2 >> ax3 >> ax4;
	endposition[0].position = IntegerPoint2D(std::stoi(ax2.substr(0, ax2.length() - 1)), std::stoi(ax3.substr(0, ax3.length() - 1)));
	endposition[0].angle = std::stoi(ax4);

	inputFile >> ax1 >> ax2 >> ax3 >> ax4;
	startposition[1].position = IntegerPoint2D(std::stoi(ax2.substr(0, ax2.length() - 1)), std::stoi(ax3.substr(0, ax3.length() - 1)));
	startposition[1].angle = std::stoi(ax4);

	inputFile >> ax1 >> ax2 >> ax3 >> ax4;
	endposition[1].position = IntegerPoint2D(std::stoi(ax2.substr(0, ax2.length() - 1)), std::stoi(ax3.substr(0, ax3.length() - 1)));
	endposition[1].angle = std::stoi(ax4);

	for (int i = 0; i < 6; i++)
		inputFile >> ax1;
	for (int i = 0; i < dimX; i++) {
		for (int j = 0; j < dimY; j++) {
			inputFile >> input[i][j];
		}
	}
	
	inputFile.close();
}

inline bool equal(int px1, int py1, int px2, int py2) {
	return (px1 == px2) && (py1 == py2);
}

inline int simplifyAngle(int angle) {
	while (angle > 180)
		angle -= 360;
	while (angle <= -180) {
		angle += 360;
	}
	return angle;
}

bool validMove(State actual, State next, int dir1, int dir2) {
	int pnx, pny;
	if (next.px1 < 0 || next.py1 < 0 || next.px1 >= dimX || next.py1 >= dimY || 
		next.px2 < 0 || next.py2 < 0 || next.px2 >= dimX || next.py2 >= dimY) 
		return false;
	if (input[next.px1][next.py1] == "xx" || input[next.px2][next.py2] == "xx")
		return false;
	if (equal(next.px1, next.py1, next.px2, next.py2) || equal(actual.px1, actual.py1, next.px2, next.py2) ||
		equal(next.px1, next.py1, actual.px2, actual.py2) || equal(actual.px1, actual.py1, actual.px2, actual.py2))
		return false;
	if (SAFETY_DISTANCE) {
		for (int i = 0; i < 9; i++) {
			pnx = next.px1 + dirx[i];
			pny = next.py1 + diry[i];
			if (pnx < 0 || pny < 0 || pnx >= dimX || pny >= dimY || input[pnx][pny] == "xx") return false;
			if (equal(pnx, pny, next.px2, next.py2) || equal(pnx, pny, actual.px2, actual.py2)) return false;
			pnx = next.px2 + dirx[i];
			pny = next.py2 + diry[i];
			if (pnx < 0 || pny < 0 || pnx >= dimX || pny >= dimY || input[pnx][pny] == "xx") return false;
			if (equal(pnx, pny, next.px1, next.py1) || equal(pnx, pny, actual.px1, actual.py1)) return false;
		}
	} else {
		if (dir1 > 4) {
			pnx = actual.px1 + dirx[dir1];
			pny = actual.py1;
			if (pnx < 0 || pny < 0 || pnx >= dimX || pny >= dimY || input[pnx][pny] == "xx") return false;
			if (equal(pnx, pny, next.px2, next.py2) || equal(pnx, pny, actual.px2, actual.py2)) return false;
			pnx = actual.px1;
			pny = actual.py1 + diry[dir1];
			if (pnx < 0 || pny < 0 || pnx >= dimX || pny >= dimY || input[pnx][pny] == "xx") return false;
			if (equal(pnx, pny, next.px2, next.py2) || equal(pnx, pny, actual.px2, actual.py2)) return false;
		}
		if (dir2 > 4) {
			pnx = actual.px2 + dirx[dir2];
			pny = actual.py2;
			if (pnx < 0 || pny < 0 || pnx >= dimX || pny >= dimY || input[pnx][pny] == "xx") return false;
			if (equal(pnx, pny, next.px1, next.py1) || equal(pnx, pny, actual.px1, actual.py1)) return false;
			pnx = actual.px2;
			pny = actual.py2 + diry[dir2];
			if (pnx < 0 || pny < 0 || pnx >= dimX || pny >= dimY || input[pnx][pny] == "xx") return false;
			if (equal(pnx, pny, next.px1, next.py1) || equal(pnx, pny, actual.px1, actual.py1)) return false;
		}
	}
	return true;
}

void bfs() {
	
	startState.py1 = (startposition[0].position.x - CELL_SIZE / 2) / CELL_SIZE;
	startState.py2 = (startposition[1].position.x - CELL_SIZE / 2) / CELL_SIZE;
	startState.px1 = dimY - (startposition[0].position.y + CELL_SIZE / 2) / CELL_SIZE;
	startState.px2 = dimY - (startposition[1].position.y + CELL_SIZE / 2) / CELL_SIZE;
	endState.py1 = (endposition[0].position.x - CELL_SIZE / 2) / CELL_SIZE;
	endState.py2 = (endposition[1].position.x - CELL_SIZE / 2) / CELL_SIZE;
	endState.px1 = dimY - (endposition[0].position.y + CELL_SIZE / 2) / CELL_SIZE;
	endState.px2 = dimY - (endposition[1].position.y + CELL_SIZE / 2) / CELL_SIZE;
	bfsQueue.push(startState);
	memset(bfsPosition, 0, sizeof(bfsPosition));
	bfsPosition[startState.px1][startState.py1][startState.px2][startState.py2] = true;
	bfsTrace[startState.px1][startState.py1][startState.px2][startState.py2][0] =
		bfsTrace[startState.px1][startState.py1][startState.px2][startState.py2][1] = -1;
	while (bfsQueue.size() > 0 && !bfsPosition[endState.px1][endState.py1][endState.px2][endState.py2]) {
		State ac, nx;
		ac = bfsQueue.front(); bfsQueue.pop();
		debug << ac.px1 << " " << ac.py1 << " " << ac.px2 << " " << ac.py2 << endl;
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				nx.px1 = ac.px1 + dirx[i];
				nx.py1 = ac.py1 + diry[i];
				nx.px2 = ac.px2 + dirx[j];
				nx.py2 = ac.py2 + diry[j];
				if (validMove(ac, nx, i, j) && !bfsPosition[nx.px1][nx.py1][nx.px2][nx.py2]) {
					bfsPosition[nx.px1][nx.py1][nx.px2][nx.py2] = true;
					bfsTrace[nx.px1][nx.py1][nx.px2][nx.py2][0] = i;
					bfsTrace[nx.px1][nx.py1][nx.px2][nx.py2][1] = j;
					bfsQueue.push(nx);
				}
			}
		}
	}
	debug << "END DFS" << endl;
}

void generateSolution() {
	vector<State> sol;
	vector<int> soldir1;
	vector<int> soldir2;
	State current = endState;
	bool finished = false;
	while (!finished) {
		sol.push_back(current);
		debug << current.px1 << " " << current.py1 << " " << current.px2 << " " << current.py2 << endl;
		if (bfsTrace[current.px1][current.py1][current.px2][current.py2][0] == -1) finished = true;
		else {
			int dir1 = bfsTrace[current.px1][current.py1][current.px2][current.py2][0];
			int dir2 = bfsTrace[current.px1][current.py1][current.px2][current.py2][1];
			soldir1.push_back(dir1);
			soldir2.push_back(dir2);
			current.px1 = current.px1 - dirx[dir1];
			current.py1 = current.py1 - diry[dir1];
			current.px2 = current.px2 - dirx[dir2];
			current.py2 = current.py2 - diry[dir2];
		}
	}
	Configuration lastKnown1, lastKnown2;
	lastKnown1.angle = 0;
	lastKnown1.position.x = 0;
	lastKnown1.position.y = 0;
	lastKnown2.angle = 0;
	lastKnown2.position.x = 0;
	lastKnown2.position.y = 0;
	int trueAngle1, trueAngle2, dirn1, dirn2;
	for (int i = soldir1.size() - 1; i >= 0; i--) {	
		if (soldir1[i] != 0) {
			trueAngle1 = angle[soldir1[i]];
			trueAngle1 -= startposition[0].angle;
			trueAngle1 = simplifyAngle(trueAngle1);
			dirn1 = angleToDir[trueAngle1];
		} else {
			trueAngle1 = lastKnown1.angle;
			dirn1 = 0;
		}
		
		if(soldir2[i] != 0) {
			trueAngle2 = angle[soldir2[i]];
			trueAngle2 -= startposition[1].angle;
			trueAngle2 = simplifyAngle(trueAngle2);
			dirn2 = angleToDir[trueAngle2];
		}
		else {
			trueAngle2 = lastKnown2.angle;
			dirn2 = 0;
		}

		if ((trueAngle2 != lastKnown2.angle || trueAngle1 != lastKnown1.angle) && FIRST_TRY_TO_SPIN) {
			lastKnown1.angle = trueAngle1;
			lastKnown2.angle = trueAngle2;
			solRobot1.push_back(lastKnown1);
			solRobot2.push_back(lastKnown2);
		}

		lastKnown1.angle = trueAngle1;
		lastKnown1.position.y -= dirx[dirn1] * CELL_SIZE ;
		lastKnown1.position.x += diry[dirn1] * CELL_SIZE ;
	
		lastKnown2.angle = trueAngle2;
		lastKnown2.position.y -= dirx[dirn2] * CELL_SIZE ;
		lastKnown2.position.x += diry[dirn2] * CELL_SIZE ;
		solRobot1.push_back(lastKnown1);
		solRobot2.push_back(lastKnown2);
	}
	lastKnown1.angle = simplifyAngle(endposition[0].angle - startposition[0].angle);
	solRobot1.push_back(lastKnown1);
	lastKnown2.angle = simplifyAngle(endposition[1].angle - startposition[1].angle);
	solRobot2.push_back(lastKnown2);
	for (int i = 0; i < solRobot1.size(); i++) {
		debug << "A: " << solRobot1[i].position.x << " " << solRobot1[i].position.y << " " << solRobot1[i].angle << endl;
		debug << "B: " << solRobot2[i].position.x << " " << solRobot2[i].position.y << " " << solRobot2[i].angle << endl;
	}
}
void PathPlanning::Init() {
	debug.open("debug.txt");
	
	angleToDir[0] = 1;
	angleToDir[180] = 2;
	angleToDir[-90] = 3;
	angleToDir[90] = 4;
	angleToDir[-45] = 5;
	angleToDir[-135] = 6;
	angleToDir[45] = 7;
	angleToDir[135] = 8;

	parseInput();
	bfs();
	generateSolution();
}

vector<Configuration>& PathPlanning::getRobotPath(int robotNumber)
{
	if (robotNumber == 1)
		return solRobot1;
	else
		return solRobot2;
}





