//An ADT interface to represent an agent

//Constants to represent search strategies used by the agents

#define STATIONARY -1 //useful for debugging
#define RANDOM 0
#define C_L_VISITED 1 //Cheapest Least Visisted
#define DFS 2


typedef struct agentRep * Agent;

//Initialise an agent
Agent initAgent(Vertex start, int maxCycles,int stamina, 
                              int strategy,Graph g, char * name);

//Calculate the next move
Edge getNextMove(Agent agent,Graph graph);
//Execute a given move
void makeNextMove(Agent agent,Edge move);


//Get information from the agent
Vertex getCurrentLocation(Agent agent);
Vertex getStartLocation(Agent agent);
char * getName(Agent agent);

void printAgent(Agent agent);

void destroyAgent(Agent agent);
