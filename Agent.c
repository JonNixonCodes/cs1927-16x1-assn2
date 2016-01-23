#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "Graph.h"
#include "Agent.h"

//This struct stores information about an individual agent(detective or thief)
//You might want to add to this struct to store more information
struct agentRep{
    Vertex startLocation;
    Vertex currentLocation;
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
             nextMove = mkEdge(agent->currentLocation,agent->currentLocation,0);
         }
         free(filteredMoves);
         free(possibleMoves);
    }else {
         printf("Agent strategy not implemented yet\n");
         abort();
    }
    return nextMove;
}

//Actually perform the move, by changing the agent's state
//This function needs to be updated to adjust the agent's stamina
void makeNextMove(Agent agent,Edge move){
    agent->currentCycle++;
    agent->currentLocation = move.w;    
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
    free(agent->name);
    free(agent);
} 

