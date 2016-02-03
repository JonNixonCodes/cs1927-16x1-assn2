#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "Graph.h"
#include "Agent.h"

#define INT_MAX 9999

//This struct stores information about an individual agent(detective or thief)
//You might want to add to this struct to store more information
struct agentRep{
  Vertex startLocation;
  Vertex currentLocation;
  Vertex goal;
  Vertex * visited;
  int currentCycle;
  int maxCycles;
  int initialStamina; //max stamina
  int stamina;  //current stamina
  int strategy;
  Graph map;
  char * name;
};


//This creates one individual thief or detective
//You may need to add more to this
Agent initAgent(Vertex start, int maxCycles,int stamina, 
                              int strategy, Graph g, char * name){
  if(start >= numV(g)){
    printf("Error starting vertex %d not valid\n",start);
    abort();
  }
  Agent agent = malloc(sizeof(struct agentRep));

  agent->startLocation = start;
  agent->currentLocation = start;
  agent->goal = -1;
  agent->visited = malloc(numV(g) * sizeof(Vertex)); //allocate memory
  int i = 0;
  for (i = 0; i < numV(g); i++) {
    //initialise visited[start] to 1
    if(i == start) {
      agent->visited[i] = 1;
    } else {
      agent->visited[i] = 0;
    }
  }
  agent->currentCycle = 0;
  agent->maxCycles = maxCycles;
  agent->initialStamina = stamina;
  agent->stamina = stamina;
  agent->strategy = strategy;
  agent->map = g;
  agent->name = strdup(name);
  
  return agent;
}


// Takes an array with all the possible edges and puts the ones the agent
// has enough stamina for into the filteredMoves array
// returns the number of filteredMoves
int filterEdges(Agent a,int numEdges,Edge *possibleMoves,Edge * filteredMoves){
    int numFiltered = 0;
    int i;
    for(i=0;i<numEdges;i++){
        if(possibleMoves[i].weight <= a->stamina){
            filteredMoves[numFiltered++] = possibleMoves[i];
        }
    }
    return numFiltered;
}

//function returns number of edges leading to cheapest least visited vertices
int filterCLVEdges(Agent a, int numFiltered, Edge * filteredMoves, Edge * cheapLeastVisited)
{
  int numCLVisited = 0;
  int i = 0;
  int leastVisited = INT_MAX;
  int cheapest = INT_MAX;
  //Cycle through each valid edge
  for (i = 0; i < numFiltered; i++) {
    //Edge is least visited
    if (a->visited[filteredMoves[i].w] < leastVisited) {
      leastVisited = a->visited[filteredMoves[i].w];
      numCLVisited = 0;
      cheapest = filteredMoves[i].weight;
      cheapLeastVisited[numCLVisited++] = filteredMoves[i];
    //Edge is equally least visited
    } else if (a->visited[filteredMoves[i].w] == leastVisited) {
      //Edge is cheapest
      if (filteredMoves[i].weight < cheapest) {
        numCLVisited = 0;
	cheapest = filteredMoves[i].weight;
	cheapLeastVisited[numCLVisited++] = filteredMoves[i];
      //Edge is equally cheapest
      } else if (filteredMoves[i].weight == cheapest) {
        cheapLeastVisited[numCLVisited++] = filteredMoves[i];
      }
    }
  }
  return numCLVisited;
}

// Get a legal move. This should be a move that the agent has enough 
// stamina to make and is a valid edge from the graph. 
// You need to implement all other strategies.
Edge getNextMove(Agent agent,Graph g){
  Edge nextMove;          
  //Stationary strategy useful for debugging
  if(agent->strategy == STATIONARY){
    nextMove = mkEdge(agent->currentLocation,agent->currentLocation,0);  
  }else if(agent->strategy == RANDOM){      
    Edge * possibleMoves = malloc(numV(g) * sizeof(Edge));
    Edge * filteredMoves = malloc(numV(g) * sizeof(Edge));

    //Get all edges to adjacent vertices
    int numEdges = incidentEdges(g,agent->currentLocation,possibleMoves);
         
    //Filter out edges that the agent does not have enough stamina for
    int numFilteredEdges = filterEdges(agent,numEdges,possibleMoves,filteredMoves);
    if(numFilteredEdges!= 0){
      //nextMove is randomly chosen from the filteredEdges
      nextMove = filteredMoves[rand()%numFilteredEdges];
    } else {
      //the agent must stay in the same location
      //and regains stamina
      nextMove = mkEdge(agent->currentLocation,agent->currentLocation,0);
    }
    free(filteredMoves);
    free(possibleMoves);
  } else if ( agent->strategy == C_L_VISITED ) {
    Edge * possibleMoves = malloc(numV(g) * sizeof(Edge));
    Edge * filteredMoves = malloc(numV(g) * sizeof(Edge));
    Edge * cheapLeastVisited = malloc(numV(g) * sizeof(Edge));

    //Get all edges to adjacent vertices
    int numEdges = incidentEdges(g,agent->currentLocation,possibleMoves);
         
    //Filter out edges that the agent does not have enough stamina for
    int numFilteredEdges = filterEdges(agent,numEdges,possibleMoves,filteredMoves);

    //Filter out least visited moves
    int numCLVisited = filterCLVEdges(agent, numFilteredEdges, filteredMoves, cheapLeastVisited);
    assert(numCLVisited >= 0);
    if(numCLVisited == 0 ) {
      //the agent must stay in the same location
      //and regains stamina
      nextMove = mkEdge(agent->currentLocation,agent->currentLocation,0);
    } else if (numCLVisited == 1) {
      nextMove = cheapLeastVisited[0];
    } else {
      //nextMove is chosen by CLV from the filteredEdges
      nextMove = cheapLeastVisited[rand()%numCLVisited];      
    }
    free(filteredMoves);
    free(possibleMoves);
    free(cheapLeastVisited);
  } else {
    printf("Agent strategy not implemented yet\n");
    abort();
  }
  return nextMove;
}

//Actually perform the move, by changing the agent's state
//This function HAS BEEN updated to adjust the agent's stamina
void makeNextMove(Agent agent,Edge move){
  agent->currentCycle++;
  agent->currentLocation = move.w;
  //update visited array
  agent->visited[move.w]++;
  
  if (move.v == move.w) {
    agent->stamina = agent->initialStamina;
  } else {
    agent->stamina -= move.weight;
  }
  assert(agent->stamina >= 0);
}


Vertex getCurrentLocation(Agent agent){
    return agent->currentLocation;
}


char * getName(Agent agent){
    return agent->name;
}

//Needs to be updated to print out vertex name information
//and * for cities with informants
void printAgent(Agent agent){
    printf("%s %.d (%d)",agent->name,agent->stamina,
                                      agent->currentLocation);
    //MODIFY THIS
}

//You may need to update this to free any extra memory you use
void destroyAgent(Agent agent){
  //YOU MAY NEED TO MODIFY THIS
  free(agent->visited);
  free(agent->name);
  free(agent);
} 

//Additional Functions
void setGoal(Agent agent, Vertex goal) {
  agent->goal = goal;
}

Vertex getGoal(Agent agent) {
  assert(agent->goal >= 0);
  return agent->goal;
}
