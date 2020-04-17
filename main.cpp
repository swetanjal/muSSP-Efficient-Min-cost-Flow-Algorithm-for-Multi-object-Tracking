#include "bits/stdc++.h"
using namespace std;
const int MAXN = 1e5 + 5;
const long double INF = 1e18;
int N, M, SRC, SINK; // Number of nodes, Number of edges, Source, Sink
double dist[MAXN];
vector <double> cost[MAXN];
set < pair < int, int > > S;
multimap < double, int > candidates;
vector <int> edges[MAXN], descendants[MAXN];
vector <int> shortest_path, topological_ordering, nodes4Update;
int ancestor[MAXN], indegree[MAXN], shortest_path_tree_parent[MAXN];

// Read in the graph.
void initGraph(string filename){
    char pr_type[3]; //problem type;
    ifstream file(filename);
    string line_inf;
    getline(file, line_inf);
    sscanf(line_inf.c_str(), "%*c %3s %d %d", pr_type, &N, &M);
    SRC = 1;
    SINK = N;
    for(string line; getline(file, line); )
    {
        switch(line[0]){
            case 'c':
            case '\n':
            case 'n':
            case '\0':
                break;
            case 'p':
            case 'a': {
                int tail; int head;
                double weight;
                sscanf(line.c_str(), "%*c %d %d %lf", &tail, &head, &weight);

                // Clip Dummy edges that won't appear in any shortest path
                if((tail == SRC) || (head == SINK) || (tail % 2 == 0) || (weight <= cost[SRC][(head/2)-1] + cost[tail][0])){
                    edges[tail].push_back(head);
                    cost[tail].push_back(weight);
                    indegree[head]++;
                }
                break;
            }
            default: break;
        }
    }
}

// Compute Topological ordering of given graph and store the order in vector topological_ordering globally
void toposort(){
    int tmp_indegree[N + 1];
    int put[N + 1];
    for(int i = 1; i <= N; ++i){
        tmp_indegree[i] = indegree[i];
        put[i] = 1;
    }
    topological_ordering.clear();
    for(int i = 1; i <= N; ++i){
        S.insert({tmp_indegree[i], i});

    }
    while(S.empty() == false){
        int node = S.begin() -> second;
        S.erase(S.begin());
        put[node] = 0;
        for(int j = 0; j < edges[node].size(); ++j){
            if(put[edges[node][j]] == 0)
                continue;
            tmp_indegree[edges[node][j]]--;
            S.erase(S.find({tmp_indegree[edges[node][j]] + 1, edges[node][j]}));
            S.insert({tmp_indegree[edges[node][j]], edges[node][j]});
        }
        topological_ordering.push_back(node);
    }
}

// Construct the initial Shortest Path tree
void init_shortest_path_tree(){
    // Initialization
    toposort();
    int l = topological_ordering.size();
    for(int i = 1; i <= N; ++i)
        dist[i] = INF;
    dist[topological_ordering[0]] = 0;
    shortest_path_tree_parent[topological_ordering[0]] = -1; // Source

    // Iterate
    for(int i = 0; i < l; ++i){
        int node = topological_ordering[i];
        for(int j = 0; j < edges[node].size(); ++j){
            double updated_dist = dist[node] + cost[node][j];

            if(dist[edges[node][j]] - updated_dist > 0.0000001){
                dist[edges[node][j]] = updated_dist;
                shortest_path_tree_parent[edges[node][j]] = node;

                // Ancestors and Descendants for easily finding Nodes4Updates
                if(node == SRC)
                    ancestor[edges[node][j]] = edges[node][j];
                else
                    ancestor[edges[node][j]] = ancestor[node];
                descendants[ancestor[edges[node][j]]].push_back(edges[node][j]);
            }
        }
    }
}

// Extract the shortest path and store it in shortest_path vector globally
void extract_shortest_path(){
    shortest_path.clear();
    int curr = SINK;
    while(curr != 1){
        shortest_path.push_back(curr);
        curr = shortest_path_tree_parent[curr];
    }
    shortest_path.push_back(curr);
}

// Flip edges along shortest path
void flip_path(){
    int l = shortest_path.size();
    for(int i = 1; i < l; ++i){

        // Find edge index
        vector <int> :: iterator it = find(edges[shortest_path[i]].begin(), edges[shortest_path[i]].end(), shortest_path[i - 1]);
        int idx = it - edges[shortest_path[i]].begin();

        // Erase edges
        long double c = cost[shortest_path[i]][idx];
        edges[shortest_path[i]].erase(edges[shortest_path[i]].begin() + idx);
        cost[shortest_path[i]].erase(cost[shortest_path[i]].begin() + idx);
        indegree[shortest_path[i - 1]]--;

        // Add reverse edge. Permanent edge clipping: Don't add reverse edges from sink/to source
        if(shortest_path[i] != 1 && shortest_path[i - 1] != N){
            edges[shortest_path[i - 1]].push_back(shortest_path[i]);
            cost[shortest_path[i - 1]].push_back(-c);
            indegree[shortest_path[i]]++;
        }
    }
}

// Find multi paths
bool find_multi_path(){
    // Add all possible candidates if emptied in previous iteration
    if(candidates.empty()){
        for(int i = 3; i < N; i+=2){
            if((i != shortest_path[1]) && (edges[i].size() != 0) && (edges[i][0] == SINK) && (dist[i] == 0)){
                candidates.insert({dist[i] + cost[i][0], i});
            }
        }
    }

    // Check if best candidate is independent of unapplied shortest paths
    if(!candidates.empty()){
        int node = (candidates.begin())->second;
        candidates.erase(candidates.begin());

        int n = shortest_path.size();
        vector <int> :: iterator it = find(nodes4Update.begin(), nodes4Update.end(), node);
        if(it == nodes4Update.end()){

            // Update shortest path parent of sink
            shortest_path_tree_parent[SINK] = node;
            return true;
        }
        else
            return false;
    }
}

// Transforms the edge weights to positive so that Djikstra's Algorithm can be applied
void update_allgraph_weights(){
    for(int i = 1; i <= N; ++i){
        for(int j = 0; j < edges[i].size(); ++j){
            cost[i][j] = cost[i][j] + dist[i] - dist[edges[i][j]];
        }
    }
    for(int i = 1; i <= N; ++i)
        dist[i] = 0;
}

// Update shortest path tree using Dijkstra
void updateShortestPathTree(){
    // Initialization
    for(int i = 1; i <= N; ++i){
        dist[i] = INF;
        descendants[i].clear();
    }
    dist[1] = 0;
    multimap < double, int > K;
    K.insert({0, 1});

    // Go through heap
    while(K.empty() == false){
        int node = (K.begin())->second;
        double c = (K.begin())->first;
        if(node == N)
            break;
        K.erase(K.begin());

        // Iterate over node's edges
        for(int j = 0; j < edges[node].size(); ++j){
            long double upd_dist = dist[node] + cost[node][j];
            if(-0.0000001 > upd_dist - dist[edges[node][j]]){
                dist[edges[node][j]] = upd_dist;
                shortest_path_tree_parent[edges[node][j]] = node;
                K.insert({dist[edges[node][j]], edges[node][j]});

                // Ancestors and Descendants for easily finding Nodes4Updates
                if(node == SRC)
                    ancestor[edges[node][j]] = edges[node][j];
                else
                    ancestor[edges[node][j]] = ancestor[node];
            }
        }
    }

    // Placing max distance constraint
    for(int i = 2; i < N; ++i){
        if(dist[i] > dist[N])
            dist[i] = dist[N];
        descendants[ancestor[i]].push_back(i);
    }
}

// Main
int main(int argc, char * argv[]){
    // First iteration without multi-path check
    char* in_file = argv[2];
    initGraph(in_file);
    init_shortest_path_tree();
    extract_shortest_path();

    // Costs
    vector < double > iter_costs;
    iter_costs.push_back(dist[SINK]);
    double total_cost = iter_costs.back();
    update_allgraph_weights();
    flip_path();
    updateShortestPathTree();
    extract_shortest_path();
    int flag = 0;

    // Subsequent Iterations
    for(int i = 1; iter_costs.back() <= -0.0000001; ++i)
    {
        // Calc cost of current iteration
        if(flag == 0){
            iter_costs.push_back(dist[SINK] + iter_costs.back());
        }
        else{
            int n = iter_costs.size();
            iter_costs.push_back(cost[shortest_path[1]][0] + iter_costs[n - 1 - flag]);
        }
        total_cost += iter_costs.back();

        int n = shortest_path.size();
        cout << "Iteration " << i << ": "<<  iter_costs.back() << endl;
        nodes4Update.insert(nodes4Update.end(), descendants[shortest_path[n-2]].begin(), descendants[shortest_path[n-2]].end());

        if(find_multi_path()){
            flip_path();
            flag += 1;
        }
        else{
            update_allgraph_weights();
            flip_path();
            updateShortestPathTree();

            // Clear accumulated stuff
            flag = 0;
            candidates.clear();
            nodes4Update.clear();
        }
        extract_shortest_path();
    }
    cout << total_cost << endl;
    return 0;
}