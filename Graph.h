// An graph ADT interface for weighted undirected graphs

// vertices denoted by integers 0..N-1 
typedef int Vertex; 

// edges are pairs of vertices (end-points) 
typedef struct { 
    Vertex v; 
    Vertex w; 
    int weight;
} Edge; 

Edge mkEdge(Vertex, Vertex, int); 

// graph representation is hidden 
typedef struct graphRep *Graph; 

// operations on graphs 
Graph newGraph(int nV); // #vertices 

// insert an edge into the graph
// the edge must not be inserted if
//     the vertices are not valid
//     it would create a self loop
//     there already exists an edge between the vertices
void insertE(Graph g, Edge e); 

//returns 1 if there is ad adge from v to w
int isAdjacent(Graph g,Vertex v, Vertex w);

//returns the number of adjacent vertices
//and fills the adj array with the adjacent vertices
int adjacentVertices(Graph g, Vertex v, Vertex adj[]);

//returns the number of incident edges
//and fills the edges with incident edges
int incidentEdges(Graph g, Vertex v, Edge edges[]);

// returns the number of edges and fills the array of edges.
// nE is the max size of the es array
int edges(Edge es[], int nE, Graph g); 

int numV(Graph g);
int numE(Graph g);

//YOU DO NOT NEED ASSERT BASED TESTS FOR THESE FUNCTIONS
void destroyGraph(Graph g); 
void show(Graph g); 


//YOU MAY ADD MORE FUNCTIONS IN HERE FOR YOUR OWN IMPLEMENTATION
//DO NOT SUBMIT UNIT TESTS FOR THESE FUNCTIONS - OF COURSE
//YOU SHOULD TEST THEM ANYWAY BUT COMMENT OUT OR REMOVE TESTS FOR 
//SUBMISSION OTHERWISE
