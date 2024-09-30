#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "graph.h"

int main() {
    srand((unsigned)time(NULL));    
    int vertices, edge, maxedge, *edgelist,**path, *pathlen,pathnum;
    EDGE *edges;
    vertices=rand()%5+5;
    printf("Number of vertices: %d\n",vertices);
    GRAPH* graph = createGraph(vertices);
    maxedge = vertices*(vertices-1);
    edge=rand()%maxedge;
    printf("Number of edges: %d from %d\n",edge,maxedge);
    edgelist=(int*)malloc(edge*sizeof(int));
    edges = (EDGE*)malloc(edge*sizeof(EDGE));
    printf("Edge list : \n");
    if(generateUniqueRandomNumbers(maxedge, edge, edgelist)){
        for(int i=0;i<edge;i++){
            getPositionFromNumber(edgelist[i],vertices,&edges[i].from,&edges[i].to);
            printf("Edge[%3d]: id=%3d, from %2d to %2d\n",i,edgelist[i], edges[i].from,edges[i].to);
            addEdge(graph,edges[i].from,edges[i].to);
        }
        printf("\n");    
    }else{printf(" Error...\n");}
    printGraph(graph);
    PathList* pathList=findPaths(graph, 0, 4);
    printPathList(pathList);
    freePathList(pathList); 
    free(edges);
    return 0;
}