#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "Graph.h"
#include "Agent.h"
#include "Stack.h"
#include "Item.h"
#include "Lists.h"

#define INT_MAX 9999
#define TRUE 1
#define FALSE 0

//This struct stores information about an individual agent(detective or thief)
//You might want to add to this struct to store more information
struct agentRep{
  Vertex startLocation;
  Vertex currentLocation;
  Vertex goal;
  
  //CLV
  Vertex * visited;

  //DFS
  List DFS_path;

  //LT
  List LT_path;
  int LT_FLAG;
  
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
  agent->currentCycle = 0;
  agent->maxCycles = maxCycles;
  agent->initialStamina = stamina;
  agent->stamina = stamina;
  agent->strategy = strategy;
  agent->map = g;
  agent->name = strdup(name);
  
  //CLV
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

  //DFS
  agent->DFS_path = newList();

  //LT
  agent->LT_path = newList();
  agent->LT_FLAG = FALSE;
  
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

void setDFSPath(Graph g, Vertex start, List path) {
  assert(g != NULL);
  //empty agent's path
  emptyList(path);
  int i = 0;  int pre[numV(g)];  int st[numV(g)];
  //initialise pre and st array
  for (i = 0; i < numV(g); i++) {
    pre[i] = -1;  st[i] = -1;
  }
  Vertex currV = start;
  Edge possibleMoves[numV(g)];
  int count = 0;
  Stack s = newStack();
  StackPush(s, mkEdge(currV, currV, 0));
  while (!StackIsEmpty(s)) {//FIX
    Edge currE = StackPop(s);
    if (pre[currE.w] != -1) {
      continue;
    }    
    if (currE.v != currV) {
      //Backtrack
      while (currV != currE.v) {
	int numEdges = incidentEdges(g, currV, possibleMoves);      
        for (i = 0; i < numEdges; i++) {
          if (possibleMoves[i].w == st[currV]) {
	    insertEnd(path, newNode(possibleMoves[i]));
	    currV = st[currV];
	    break;
	  }
        }
        assert(i < numEdges);
      }	
    }
    //insert new move to end of path
    if (count != 0) { //skip the first move
      insertEnd(path, newNode(currE));
    }
    pre[currE.w] = count++;
    st[currE.w] = currE.v;
    currV = currE.w;
    int numEdges = incidentEdges(g, currV, possibleMoves);
    assert(numEdges > 0); //FIX LATER
    for (i = numEdges-1; i >= 0; i--) {
      if (pre[possibleMoves[i].w] == -1) {
        StackPush(s, possibleMoves[i]);
      }
    }
  }
  dropStack(s);
}

void setLTPath(Graph g, Agent agent, Vertex start, Vertex goal, List path) {
  assert(g != NULL);
  assert(start < numV(g));
  assert(goal != -1);
  //empty agent's path
  emptyList(path);
  //st: spanning tree, moves: num of moves, stamina: remaining stamina
  int i = 0;  Edge possibleMoves[numV(g)]; int st[numV(g)];  int moves[numV(g)]; int stamina[numV(g)];
  Vertex currV = start;
  //initialise st, moves, stamina array
  for (i = 0; i < numV(g); i++) {
    st[i] = -1; moves[i] = -1; stamina[i] = -1;
    if (i == start) {
      stamina[i] = agent->stamina;
    }
  }
  //initialise stack
  Stack s = newStack();
  StackPush(s, mkEdge(currV, currV, 0));
  while (!StackIsEmpty(s)) {
    Edge currE = StackPop(s);
    int currStamina = stamina[currE.v];
    if ( (currStamina - currE.weight) < 0) {
      if (currStamina == agent->initialStamina) {
        //not enough stamina to make move
	continue;
      }
      StackPush(s, currE);
      moves[currE.w]++;
      stamina[currE.w] = agent->initialStamina;
      continue;
    }
    //update arrays
    st[currE.w] = currE.v;
    moves[currE.w] = moves[currE.v]++;
    stamina[currE.w] = currStamina - currE.weight;
    currStamina = currStamina - currE.weight;
    currV = currE.w;
    
    int numEdges = incidentEdges(g, currV, possibleMoves);
    assert(numEdges > 0); //FIX LATER
    for (i = numEdges-1; i >= 0; i--) {
      if (moves[possibleMoves[i].w] != -1) {
	//visited
        if ( moves[possibleMoves[i].w] == (moves[currV]+1) ) {
          //equal number of moves
	  if ( stamina[possibleMoves[i].w] > (currStamina - possibleMoves[i].weight) ) {
            //more stamina leftover: don't push new move to stack
	    continue;
	  }
	} else if (moves[possibleMoves[i].w] == moves[currV]+2) {
          //more moves
	  if (!(currStamina - possibleMoves[i].weight < 0)) {
            //no extra move to replenish: don't push new move to stack
	    continue;
	  }
	}
      }
      StackPush(s, possibleMoves[i]);
    }
  }
  //backtrace Least-Turns Path
  for (i = 0; i < numV(g); i++) {
    //check that every node has been reached
    assert(st[i] != -1);
  }
  currV = goal;
  while (currV != start) {
    Edge nextMove;
    int numEdges = incidentEdges(g, st[currV], possibleMoves);
    for (i = 0; i < numEdges; i++) {
      if (possibleMoves[i].w == currV) {
        nextMove = possibleMoves[i];
	break;
      }
    }
    link new = newNode(nextMove);
    insertFront(path, new);
    currV = st[currV];
  }
}
// Get a legal move. This should be a move that the agent has enough 
// stamina to make and is a valid edge from the graph. 
// You need to implement all other strategies.
Edge getNextMove(Agent agent,Graph g) {
  Edge nextMove;
  //Least-turns Path strategy
  if (agent->LT_FLAG == TRUE) {
    nextMove = getFirstItem(agent->LT_path);
    //Check Stamina
    if (agent->stamina < nextMove.weight) {
      insertFront(agent->LT_path, newNode(nextMove));
      nextMove = mkEdge(agent->currentLocation,agent->currentLocation,0);
    }
    if (isListEmpty(agent->LT_path)) {
      //reset least turns flag
      agent->LT_FLAG = FALSE;
    }
  }
  //Stationary strategy useful for debugging
  else if(agent->strategy == STATIONARY){
    nextMove = mkEdge(agent->currentLocation,agent->currentLocation,0);  
  }
  //Random strategy
  else if(agent->strategy == RANDOM){      
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
  }
  //Cheapest Least Visited Strategy
  else if ( agent->strategy == C_L_VISITED ) {
    Edge * possibleMoves = malloc(numV(g) * sizeof(Edge));
    Edge * filteredMoves = malloc(numV(g) * sizeof(Edge));
    Edge * cheapLeastVisited = malloc(numV(g) * sizeof(Edge));

    //Get all edges to adjacent vertices
    int numEdges = incidentEdges(g,agent->currentLocation,possibleMoves);
    
    //Filter out illegal moves
    int numFilteredEdges = filterEdges(agent,numEdges,possibleMoves,filteredMoves);

    //Filter out least visited moves
    int numCLVisited = filterCLVEdges(agent, numFilteredEdges, filteredMoves, cheapLeastVisited);
    assert(numCLVisited >= 0);
    if(numCLVisited == 0 ) {
      //the agent must stay in the same location
      //and regains stamina
      nextMove = mkEdge(agent->currentLocation,agent->currentLocation,0);
    } else  {
      //get the smallest index
      nextMove = cheapLeastVisited[0];
      int i = 0;
      for (i = 1; i < numCLVisited; i++) {        
        if (cheapLeastVisited[i].w <= nextMove.w) {
          nextMove = cheapLeastVisited[i];
	}
      }
    }
    //Check stamina
    //assert(agent->stamina >= nextMove.weight);
    if (agent->stamina < nextMove.weight) {
      printf("stamina = %d\n", agent->stamina);
      printf("next move weight = %d\n", nextMove.weight);
      nextMove = mkEdge(agent->currentLocation,agent->currentLocation,0);
    }
    free(possibleMoves);
    free(cheapLeastVisited);
  }

  //Depth First Search Strategy
  else if (agent->strategy == DFS) {
    assert(agent->DFS_path != NULL);
    if (isListEmpty(agent->DFS_path)) {
      setDFSPath(g, agent->currentLocation, agent->DFS_path);
    }
    nextMove = getFirstItem(agent->DFS_path);
    //Check Stamina
    if (agent->stamina < nextMove.weight) {
      insertFront(agent->DFS_path, newNode(nextMove));
      nextMove = mkEdge(agent->currentLocation,agent->currentLocation,0);
    }
    //assert(agent->stamina >= nextMove.weight);
  } else {
    printf("Agent strategy not implemented yet\n");
    abort();
  }
  return nextMove;
}

//Actually perform the move, by changing the agent's state
//This function HAS BEEN updated to adjust the agent's stamina
void makeNextMove(Graph g, Agent agent, Edge move){
  agent->currentCycle++;
  agent->currentLocation = move.w;
  if(agent->goal != -1) {
    //location of thief stored in graph
    setThiefLocation(g, agent->currentLocation);
  }
  if (checkInformant(g, agent->currentLocation) && (agent->goal == -1) ) {
    agent->LT_FLAG = TRUE;
    setLTPath(g, agent, agent->currentLocation, getThiefLocation(g), agent->LT_path);  
  }
  //Cheapest Least Visited
  if (agent->strategy == C_L_VISITED) {
    //update visited array
    agent->visited[move.w]++;
  }
  
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
void printAgent(Agent agent) {
  Graph g = agent->map;
  //MODIFY THIS
  if (agent->goal != -1) {
    //Thief
    printf("%s %d %s (%d) %s (%d)\n", agent->name, agent->stamina, getCityName(g, agent->currentLocation), agent->currentLocation, getCityName(g, agent->goal), agent->goal);
  } else {
    //Detective
    printf("%s %d %s (%d)\n", agent->name, agent->stamina, getCityName(g, agent->currentLocation), agent->currentLocation);
  }
}

//You may need to update this to free any extra memory you use
void destroyAgent(Agent agent){
  //YOU MAY NEED TO MODIFY THIS
  free(agent->visited);
  deleteList(agent->DFS_path);
  deleteList(agent->LT_path);
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
