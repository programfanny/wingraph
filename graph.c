//graph.c
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "graph.h"

GraphNode* createGraphNode(int v){
	GraphNode *newNode =(GraphNode*)malloc(sizeof(GraphNode));
	newNode->vertex=v;
	newNode->next=NULL;
	return newNode;
}
GRAPH* createGraph(int vertices){
	GRAPH *graph=(GRAPH*)malloc(sizeof(GRAPH));
	graph->numVertices = vertices;
    graph->pos=(POINT*)malloc(graph->numVertices*sizeof(POINT));
    for(int v=0;v<vertices;v++){
        graph->pos[v].x=500+200*cos(v*2*3.1415926/graph->numVertices);
        graph->pos[v].y=300+200*sin(v*2*3.1415926/graph->numVertices);
    }
    graph->vertex=(int*)malloc(graph->numVertices*sizeof(int));
    for(int i=0;i<graph->numVertices;i++)graph->vertex[i]=i;
	graph->adjList = (GraphNode**)malloc(vertices * sizeof(GraphNode*));
	for(int i=0;i<vertices;i++){
		graph->adjList[i]=NULL;
	}
	return graph;
}
void addEdge(GRAPH *graph, int src, int dest){
	GraphNode *newNode = createGraphNode(dest);
	newNode->next=graph->adjList[src];
	graph->adjList[src] = newNode;
}
void printGraph(GRAPH *graph){
	for(int v=0;v<graph->numVertices; v++){
		GraphNode *temp=graph->adjList[v];
		printf("Adjacency list of vertex : %d ",v);
		while(temp){
			printf("->%d",temp->vertex);
			temp=temp->next;
		}
        printf("\n");
	}    
}
bool generateUniqueRandomNumbers(int n, int k, int *array) {
    int *tempArray = (int *)malloc(n * sizeof(int));
    if (tempArray == NULL) {
    	printf("Error malloc space.\n");
        return false;
    }
    for (int i = 0; i < n; i++) {
        tempArray[i] = i;
    }
    for (int i = 0; i < k; i++) {
        int j = rand() % (n - i) + i;
        int temp = tempArray[i];
        tempArray[i] = tempArray[j];
        tempArray[j] = temp;
    }
    for (int i = 0; i < k; i++) {
        array[i] = tempArray[i];
    }
    free(tempArray);
    return true;
}
void getPositionFromNumber(int m, int n, int *from, int *to){
	int row = m/(n-1);
	int col = m % (n-1);
	if(col>=row)col=col+1;
	*from = row;
	*to = col;
}
bool InitGraph(GRAPH **graph){
    int vertices, edge, maxedge, *edgelist;
    EDGE *edges;
	vertices=rand()%6+5;
	if((*graph)!=NULL){
		free((*graph)->adjList); 
        free((*graph)->vertex);
		(*graph)->adjList=NULL;
		(*graph)->numVertices=0;
        (*graph)->vertex=NULL;
        free(*graph);
        *graph=NULL;        
	}
	*graph = createGraph(vertices);
    maxedge = vertices*(vertices-1);
    edge=rand() % maxedge;
    (*graph)->numEdges = edge;
    edgelist=(int*)malloc(edge*sizeof(int));
    edges = (EDGE*)malloc(edge*sizeof(EDGE));
    if(generateUniqueRandomNumbers(maxedge, edge, edgelist)){
        for(int i=0;i<edge;i++){
        	getPositionFromNumber(edgelist[i],vertices,&edges[i].from,&edges[i].to);
            addEdge(*graph,edges[i].from,edges[i].to);
        }
    }else{
        return FALSE;
    }  
    free(edges);
    free(edgelist); 
    return TRUE;
}
void printPathList(PathList* pathList) {
    printf("size: %d\n", pathList->size);
    for (int i = 0; i < pathList->size; i++) {
        for (int j = 0; j < pathList->paths[i].length; j++) {
            printf("%d ", pathList->paths[i].vertices[j]);
        }
        printf("\n");
    }
}
void freePathList(PathList* pathList) {
    for (int i = 0; i < pathList->size; i++) {
        free(pathList->paths[i].vertices);
        pathList->paths[i].vertices=NULL;
        pathList->paths[i].length=0;
    }
    free(pathList->paths);
    pathList->paths=NULL;
    pathList->size=0;
    pathList->capacity=0;
    free(pathList);
}
Path* createPath(int length) {
    Path *newPath = (Path*)malloc(sizeof(Path));
    newPath->vertices = (int*)malloc(length * sizeof(int));
    newPath->length = length;
    return newPath;
}
void addPath(PathList *pathList, Path *path) {
    if (pathList->size >= pathList->capacity) {
        pathList->capacity = (pathList->capacity == 0) ? 1 : pathList->capacity * 2;
        pathList->paths = (Path*)realloc(pathList->paths, pathList->capacity * sizeof(Path));
    }
    pathList->paths[pathList->size] = *path;
    pathList->size++;
}
void findAllPaths(GRAPH* graph, int start, int end, int pathIndex, int* path, int pathSize, PathList* pathList, int *visited) {
    if (start == end) {
        path[pathIndex]=end;
        Path newPath;
        newPath.vertices = (int*)malloc((pathIndex + 1) * sizeof(int));
        for (int i = 0; i <= pathIndex; i++) {
            newPath.vertices[i] = path[i];
        }
        newPath.length = pathIndex + 1;
        addPath(pathList, &newPath);
        //free(newPath.vertices);
        return;
    }
    path[pathIndex] = start;
    visited[start] = 1;
    for (GraphNode* temp = graph->adjList[start]; temp != NULL; temp = temp->next) {
        if (!visited[temp->vertex]) {  // 确保有足够的空间存储路径
            findAllPaths(graph, temp->vertex, end, pathIndex + 1, path, pathSize, pathList,visited);
        }
    }
    visited[start] = 0;
}
PathList* findPaths(GRAPH* graph, int start, int end) {
    printf("Enter findPath(%d, %d)\n",start, end);
    PathList* pathList = (PathList*)malloc(sizeof(PathList));
    pathList->paths = NULL;
    pathList->size = 0;
    pathList->capacity = 0;
    int pathSize = graph->numVertices;
    int* path = (int*)calloc(pathSize, sizeof(int));
    int* visited =(int*)calloc(pathSize, sizeof(int));
    findAllPaths(graph, start, end, 0, path, pathSize, pathList, visited);
    free(visited);
    free(path);
    printf("Exit findPath()\n");
    return pathList;
}

