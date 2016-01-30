/* 
  Graph.c
   
  
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Graph.h"


struct graphRep { 
  //TODO
  int nV;
  int nE;
  Vertex ** edges;
}; 

// Create an edge from v to w 
Edge mkEdge(Vertex v, Vertex w, int weight) { 
  assert(v >= 0 && w >= 0); 
  assert(v != w);
  Edge e = {v,w,weight}; 
  return e; 
}

//Initialise a new graph
Graph newGraph(int nV) { 
  //COMPLETE THIS
  assert(nV >= 0);
  Graph g = malloc(sizeof(*g)); //allocate memory for graph
  g->nV = nV;
  g->nE = 0;
  g->edges = malloc(sizeof(*(g->edges)) * nV); //allocate memory for pointers to arrays
  int i = 0;
  while (i < nV) //allocate memory for each array of vertices
  {
    g->edges[i] = malloc(sizeof(Vertex) * nV);
    int j = 0;
    while (j < nV) //initialise all weights to -1
    {
      g->edges[i][j] = -1;
      j++;
    }
    i++;
  }
  return g;
}

//Insert an edge into a graph
//the edge must not be inserted if
//     the vertices are not valid
//     it would create a self loop
//     there already exists an edge between the vertices
void insertE(Graph g, Edge e) { 
  assert(g != NULL);
  //COMPLETE THIS
  g->edges[e.v][e.w] = e.weight;
  g->edges[e.w][e.v] = e.weight;
  g->nE++;
} 

//returns 1 if there is an edge from v to w
//returns 0 otherwise
int isAdjacent(Graph g, Vertex v, Vertex w){
  assert(g != NULL);
  //COMPLETE THIS
  if (g->edges[v][w] || g->edges[w][v]) {
    return 1;
  }
  return 0;
}

//returns the number of adjacent vertices
//and fills the adj array with the adjacent vertices
int adjacentVertices(Graph g, Vertex v, Vertex adj[]){
  assert(g != NULL);
  //COMPLETE THIS
  int i = 0; int counter = 0;
  while (i < g->nV) {
    if (g->edges[v][i] >= 0) {
      adj[counter] = i;
      counter++;
    }
    i++;
  }
  return counter;
}

//returns the number of incident edges
//and fills the edges with incident edges
int incidentEdges(Graph g, Vertex v, Edge edges[]){
  assert(g != NULL);
  //COMPLETE THIS
  int i = 0; int counter = 0;
  while (i < g->nV) {
    if (g->edges[v][i] >= 1) {
      Edge e = mkEdge(v, i, g->edges[v][i]);
      edges[counter] = e;
      counter++;
    }
    i++;
  }
  return counter;
}

void destroyGraph(Graph g){
  assert(g != NULL);
  //COMPLETE THIS
  int i = 0;
  while (i < g->nV) {
    free(g->edges[i]);
    i++;
  }
  free(g->edges);
  free(g);
}

//return the number of vertices in the graph
int numV(Graph g){
  assert(g != NULL);
  //COMPLETE THIS
  return g->nV;
}

//return the number of edges in the graph
int numE(Graph g){
  assert(g != NULL);
  //COMPLETE THIS
  return g->nE;
}

// returns the number of edges and fills the array of edges.
// nE is the max size of the es array
// The edges in the edges function should be in ascending order and not 
// contain duplicates. 
int edges(Edge es[], int nE, Graph g){
  assert(g != NULL);
  assert(es != NULL);
  //COMPLETE THIS
  int i = 0; int counter = 0;
  while (i < g->nV && counter < nE) {
    int j = i;
    while (j < g->nV && counter < nE) {
      if (g->edges[i][j] >= 0) {
        es[counter] = mkEdge(i, j, g->edges[i][j]);
	counter++;
      }
      j++;
    }
    i++;
  }
  return counter;
}

//Display the graph
void show(Graph g) { 
    assert(g != NULL); 
    printf("V=%d, E=%d\n", numV(g), numE(g)); 
    int i, j; 
    for (i = 0; i < numV(g); i++) { 
        int nshown = 0; 
        for (j = 0; j < numV(g); j++) { 
            if (isAdjacent(g,i,j)) {                
                printf("%d-%d : TODO ",i,j); 
                nshown++; 
            } 
        } 
        if (nshown > 0){
            printf("\n"); 
        }
    } 
}
