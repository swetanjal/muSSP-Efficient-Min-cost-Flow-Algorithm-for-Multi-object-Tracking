#include "bits/stdc++.h"
using namespace std;
const int MAXN = 1e5 + 5;
const long double INF = 1e17;

int N, M, SRC, SINK;
double dist[MAXN];
int ancestor[MAXN], indegree[MAXN], shortest_path_tree_parent[MAXN], visited[MAXN];
vector <int> shortest_path, topological_ordering, nodes4Update, root0;
vector <int> edges[MAXN], descendants[MAXN];
vector <double> cost[MAXN];

set < pair < int, int > > S;
map < int, int > anc0;
multimap < double, int > candidates;
ofstream shortest_path_file;


// For adding distances. Want to prevent overflow when distance is INF.
double add_dist(double dist1, double dist2){
    if(dist1 == INF || dist2 == INF)
        return INF;
    else
        return dist1 + dist2;
}

// For subtracting distances. Want to prevent overflow when distance is INF.
double sub_dist(double dist1, double dist2){
    if(dist1 == INF && dist2 == INF)
        return 0;
    else if(dist1 == INF)
        return INF;
    else if(dist2 == INF)
        return -INF;
    else
        return dist1 - dist2;
}

// Read in the graph.
void initGraph(string filename){
    char pr_type[3];
    ifstream file(filename);
    string line_inf;
    getline(file, line_inf);
    sscanf(line_inf.c_str(), "%*c %3s %d %d", pr_type, &N, &M);
    SRC = 1; SINK = N;
    for(string line; getline(file, line);){
        switch(line[0]){
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
    for(int i = 1; i <= N; ++i)
        dist[i] = INF;
    dist[topological_ordering[0]] = 0;
    shortest_path_tree_parent[topological_ordering[0]] = -1; // Source

    // Iterate
    for(int i = 0; i < topological_ordering.size(); ++i){
        int node = topological_ordering[i];
        for(int j = 0; j < edges[node].size(); ++j){
            double updated_dist = add_dist(dist[node], cost[node][j]);

            if(updated_dist < dist[edges[node][j]]){
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
    while(curr != SRC){
        shortest_path.push_back(curr);
        curr = shortest_path_tree_parent[curr];
    }
    shortest_path.push_back(curr);
}

// Flip edges along shortest path
void flip_path(){
    int l = shortest_path.size();
    shortest_path_file << shortest_path[l - 1] << " ";
    for(int i = 1; i < l; ++i){

        // Erase edge
        vector <int> :: iterator it = find(edges[shortest_path[i]].begin(), edges[shortest_path[i]].end(), shortest_path[i - 1]);
        int idx = it - edges[shortest_path[i]].begin();
        long double c = cost[shortest_path[i]][idx];
        edges[shortest_path[i]].erase(edges[shortest_path[i]].begin() + idx);
        cost[shortest_path[i]].erase(cost[shortest_path[i]].begin() + idx);

        // Add reverse edge. Permanent edge clipping: Don't add reverse edges from sink/to source
        if(shortest_path[i] != SRC && shortest_path[i - 1] != SINK){
            edges[shortest_path[i - 1]].push_back(shortest_path[i]);
            cost[shortest_path[i - 1]].push_back(-c);
        }
        shortest_path_file << shortest_path[l - 1 - i] << " ";
    }
    shortest_path_file << '\n';
}

// Find multi paths
bool find_multi_path(){
    // Check if best candidate is independent of unapplied shortest paths
    if(!candidates.empty()){
        int node = (candidates.begin())->second;
        candidates.erase(candidates.begin());
        if(dist[node] != 0)
            return false;

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
    else
        return false;
}

// Transforms the edge weights to positive so that Djikstra's Algorithm can be applied
void update_allgraph_weights(){
    for(int i = 1; i <= N; ++i){
        for(int j = 0; j < edges[i].size(); ++j){
            cost[i][j] = sub_dist(add_dist(cost[i][j], dist[i]), dist[edges[i][j]]);
        }
    }
}

// Update shortest path tree using Dijkstra without optimizations
void updateShortestPathTree_stand(){
    // Initialization
    for(int i = 1; i <= N; ++i){
        descendants[i].clear();
        dist[i] = INF;
    }
    candidates.clear();
    multimap < double, int > K;
    K.insert({0, 1});
    dist[1] = 0;

    // Go through heap
    while(K.empty() == false){
        int node = (K.begin())->second;
        double c = (K.begin())->first;
        K.erase(K.begin());

        // Iterate over node's edges
        for(int j = 0; j < edges[node].size(); ++j){
            long double upd_dist = add_dist(dist[node], cost[node][j]);
            if(upd_dist < dist[edges[node][j]]){
                dist[edges[node][j]] = upd_dist;
                shortest_path_tree_parent[edges[node][j]] = node;
                K.insert({dist[edges[node][j]], edges[node][j]});
                if(node == SRC)
                    ancestor[edges[node][j]] = edges[node][j];
                else
                    ancestor[edges[node][j]] = ancestor[node];
            }
        }
    }

    // For updating candidates and descendants
    for(int i = 2; i < N; ++i){
        descendants[ancestor[i]].push_back(i);
        if((i % 2 == 1) && (edges[i].size() != 0) && (edges[i][0] == SINK) && (dist[i] != INF))
            candidates.insert({add_dist(dist[i], cost[i][0]), i});
    }
    int node = (candidates.begin())->second;
    candidates.erase(candidates.begin());
}

// Update shortest path tree using Dijkstra
void updateShortestPathTree(){
    // Initialization
    for(int i = 1; i <= N; ++i){
        descendants[i].clear();
        visited[i] = 0;
    }
    for(int i = 0; i < nodes4Update.size(); ++i){
        dist[nodes4Update[i]] = INF;
        shortest_path_tree_parent[nodes4Update[i]] = 0;
        ancestor[nodes4Update[i]] = 0;
    }
    candidates.clear();
    multimap < double, int > K;

    // Relax edges between nodes outside 0-tree to inside through bfs
    queue < int > bfs;
    bfs.push(SRC);
    visited[SRC] = 1;
    while(!bfs.empty()){
        int node = bfs.front();
        bfs.pop();

        // Iterate through all edges of node
        for(int i = 0; i < edges[node].size(); ++i){
            int anc = ancestor[edges[node][i]];

            // If child in 0-tree
            if(anc0.find(anc) != anc0.end()){
                double upt_dist = add_dist(dist[node], cost[node][i]);
                if(upt_dist < dist[edges[node][i]]){
                    dist[edges[node][i]] = upt_dist;
                    shortest_path_tree_parent[edges[node][i]] = node;
                    if(node == SRC)
                        ancestor[edges[node][i]] = edges[node][i];
                    else
                        ancestor[edges[node][i]] = ancestor[node];
                }
            }
            else if(visited[edges[node][i]] == 0){
                visited[edges[node][i]] = 1;
                bfs.push(edges[node][i]);
            }
        }
    }

    // For each 0-tree
    for(int i = 0; i < root0.size(); ++i){
        int root = root0[i];
        bfs.push(root);
        K.insert({dist[root], root});

        // Update within 0-tree through bfs
        while(!bfs.empty()){
            int node = bfs.front();
            bfs.pop();

            // Iterate through all edges of node
            for(int j = 0; j < edges[node].size(); ++j){
                if(cost[node][j] == 0){
                    if(dist[edges[node][j]] > dist[node]){
                        dist[edges[node][j]] = dist[node];
                        shortest_path_tree_parent[edges[node][j]] = node;
                        if(node == SRC)
                            ancestor[edges[node][j]] = edges[node][j];
                        else
                            ancestor[edges[node][j]] = ancestor[node];
                    }
                    else{
                        K.insert({dist[edges[node][j]], edges[node][j]});
                    }
                    bfs.push(edges[node][j]);
                }
            }
        }
    }

    // Go through heap
    while(K.empty() == false){
        int node = (K.begin())->second;
        double c = (K.begin())->first;
        if(node == N)
            break;
        K.erase(K.begin());

        // Iterate over node's edges
        for(int j = 0; j < edges[node].size(); ++j){
            long double upd_dist = add_dist(dist[node], cost[node][j]);
            if(upd_dist <= dist[edges[node][j]]){
                dist[edges[node][j]] = upd_dist;
                shortest_path_tree_parent[edges[node][j]] = node;
                K.insert({dist[edges[node][j]], edges[node][j]});
                if(node == SRC)
                    ancestor[edges[node][j]] = edges[node][j];
                else
                    ancestor[edges[node][j]] = ancestor[node];
            }
        }
    }

    // For updating candidates and descendants
    for(int i = 2; i < N; ++i){
        descendants[ancestor[i]].push_back(i);
        if((i % 2 == 1) && (edges[i].size() != 0) && (edges[i][0] == SINK) && (dist[i] != INF)){
            candidates.insert({add_dist(dist[i], cost[i][0]), i});
        }
    }

    // Update Sink with best candidate
    int node = (candidates.begin())->second;
    candidates.erase(candidates.begin());
    dist[SINK] = add_dist(dist[node], cost[node][0]);
    shortest_path_tree_parent[SINK] = node;
    ancestor[SINK] = ancestor[node];
}

// Main
int main(int argc, char * argv[]){
    // First iteration without multi-path check
    shortest_path_file.open("Shortest_Paths.txt");
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
    updateShortestPathTree_stand();
    extract_shortest_path();
    int flag = 0; int remem = -1;

    // Subsequent Iterations
    for(int i = 1; iter_costs.back() <= -0.0000001; ++i)
    {
        // Calc cost of current iteration
        if(flag == 0){
            if(remem != -1){
                iter_costs.back() = iter_costs[remem];
                remem = -1;
            }
            iter_costs.push_back(dist[SINK] + iter_costs.back());
        }
        else{
            int n = iter_costs.size();
            if(flag == 1){
                remem = n - 1;
            }
            iter_costs.push_back(cost[shortest_path[1]][0] + iter_costs[n - 1 - flag]);
        }
        total_cost += iter_costs.back();

        int n = shortest_path.size();
        cout << "Iteration " << i << ": "<<  iter_costs.back() << endl;
        anc0.insert({shortest_path[n - 2], 1});
        root0.push_back(shortest_path[1]);
        nodes4Update.insert(nodes4Update.end(), descendants[shortest_path[n-2]].begin(), descendants[shortest_path[n-2]].end());

        // Multi paths or not?
        if(find_multi_path()){
            flip_path();
            flag += 1;
        }
        else{
            update_allgraph_weights();
            flip_path();
            updateShortestPathTree_stand();

            // Clear accumulated stuff
            flag = 0;
            nodes4Update.clear();
            anc0.clear();
            root0.clear();
        }
        extract_shortest_path();
    }
    cout << total_cost << endl;
    return 0;
}