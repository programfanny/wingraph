//graph.h
#ifndef _GRAPH_H_
#define _GRAPH_H_
#include <windows.h>
#include <stdbool.h>
typedef enum { UDUW, UDW, DUW, DW } GRAPHTYPE;
typedef struct graphnode{
	int vertex;
	int x,y,z;	
	struct graphnode *next;
}GraphNode;
typedef struct edge{
	int from,to;
	int length;
}EDGE;
typedef struct Graph{
	GRAPHTYPE gtype;
	int numVertices;
	int *vertex;
	POINT *pos;
	int numEdges;
	GraphNode** adjList;
	int **adjMatrix;
}GRAPH;
// 用于存储路径的数组
typedef struct path {
    int *vertices;
    int length;
} Path;

// 用于存储所有路径的数组
typedef struct pathList {
    Path *paths;
    int size;
    int capacity;
} PathList;

GraphNode* createGraphNode(int v);
GRAPH* createGraph(int vertices);
void addEdge(GRAPH *graph, int src, int dest);
void printGraph(GRAPH *graph);
bool generateUniqueRandomNumbers(int n, int k, int *array);
void getPositionFromNumber(int m, int n, int *from, int *to);
bool InitGraph(GRAPH **graph);
void ShortPathA(GRAPH *graph, int A, int B, int **path, int *len);
PathList* findPaths(GRAPH* graph, int start, int end);
void printPathList(PathList* pathList);
void freePathList(PathList* pathList);

#endif