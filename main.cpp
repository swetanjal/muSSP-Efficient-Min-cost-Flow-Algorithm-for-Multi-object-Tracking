#include "bits/stdc++.h"
using namespace std;
const int MAXN = 1e5 + 5;
const long double INF = 1e17;

int N, M, SRC, SINK;
double dist[MAXN];
int indegree[MAXN], shortest_path_tree_parent[MAXN], status[MAXN], old_parent[MAXN];
vector <int> shortest_path, topological_ordering, nodes4Update, root0;
vector <int> edges[MAXN];
vector <double> cost[MAXN];

set < pair < int, int > > S;
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
            shortest_path_tree_parent[shortest_path[i]] = shortest_path[i-1];
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
            }
        }
    }

    // For updating candidates and descendants
    for(int i = 2; i < N; ++i){
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
        status[i] = 0;
        old_parent[i] = shortest_path_tree_parent[i];
    }
    for(int i = 0; i < nodes4Update.size(); ++i){
        dist[nodes4Update[i]] = INF;
        status[nodes4Update[i]] = 1;
    }
    candidates.clear();
    multimap < double, int > K;

    // Update distances now that edge weights are updated
    queue < int > bfs;
    bfs.push(SRC);
    while(!bfs.empty()){
        int node = bfs.front();
        bfs.pop();
        status[node] = 2;
        for(int i = 0; i < edges[node].size(); ++i){
            if(shortest_path_tree_parent[edges[node][i]] == node){
                dist[edges[node][i]] = add_dist(dist[node], cost[node][i]);
                bfs.push(edges[node][i]);
            }
        }
    }

    // Relax edges between nodes outside of 0-tree and nodes within
    for(int i = 1; i < N; ++i){
        if(status[i] == 2){
            for(int j = 0; j < edges[i].size(); ++j){
                double upt_dist = add_dist(dist[i], cost[i][j]);
                if((status[edges[i][j]] == 1) && (upt_dist < dist[edges[i][j]])){
                    dist[edges[i][j]] = upt_dist;
                    shortest_path_tree_parent[edges[i][j]] = i;
                }
            }
        }
    }

    // BFS through 0-trees to see which should be pushed to heap
    for(int i = 0; i < root0.size(); ++i){
        int root = root0[i];
        bfs.push(root);
        K.insert({dist[root], root});
        while(!bfs.empty()){
            int node = bfs.front();
            bfs.pop();

            // Iterate through all edges of node
            for(int j = 0; j < edges[node].size(); ++j){
                if(old_parent[edges[node][j]] == node){
                    if(dist[edges[node][j]] >= dist[node]){
                        dist[edges[node][j]] = dist[node];
                        shortest_path_tree_parent[edges[node][j]] = node;
                    }
                    else
                        K.insert({dist[edges[node][j]], edges[node][j]});
                    bfs.push(edges[node][j]);
                }
            }
        }
    }

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
            }
            else if(upd_dist == dist[edges[node][j]] && old_parent[edges[node][j]] == node){
                old_parent[edges[node][j]] = 0;
                K.insert({dist[edges[node][j]], edges[node][j]});
            }
        }
    }

    // For updating candidates and descendants
    for(int i = 2; i < N; ++i){
        if((i % 2 == 1) && (edges[i].size() != 0) && (edges[i][0] == SINK) && (dist[i] != INF)){
            candidates.insert({add_dist(dist[i], cost[i][0]), i});
        }
    }

    // Update Sink with best candidate
    int node = (candidates.begin())->second;
    candidates.erase(candidates.begin());
    dist[SINK] = add_dist(dist[node], cost[node][0]);
    shortest_path_tree_parent[SINK] = node;
}

// Add descendants to node4Update
void add_descendants(int begin){
    queue < int > bfs;
    bfs.push(begin);
    while(!bfs.empty()){
        int node = bfs.front();
        bfs.pop();
        if(node != SINK){
            nodes4Update.push_back(node);
            for(int i = 0; i < edges[node].size(); ++i)
                if(shortest_path_tree_parent[edges[node][i]] == node)
                    bfs.push(edges[node][i]);
        }
    }
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
        root0.push_back(shortest_path[1]);
        add_descendants(shortest_path[n-2]);

        // Multi paths or not?
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
            nodes4Update.clear();
            root0.clear();
        }
        extract_shortest_path();
    }
    cout << total_cost << endl;
    return 0;
}