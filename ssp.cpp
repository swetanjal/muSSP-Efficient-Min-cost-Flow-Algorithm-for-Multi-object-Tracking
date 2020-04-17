#include "bits/stdc++.h"
using namespace std;
const long double INF = 1e18;
const int MAXN = 1e5 + 5;
int N, M; // Number of nodes, Number of edges
vector <int> edges[MAXN];
vector <double> cost[MAXN];
int indegree[MAXN];
double dist[MAXN];
int shortest_path_tree_parent[MAXN];
set < pair < int, int > > S;

// This function is responsible for reading in the graph.
void initGraph(string filename)
{
    char pr_type[3]; //problem type;
    ifstream file(filename);
    string line_inf;
    getline(file, line_inf);
    sscanf(line_inf.c_str(), "%*c %3s %d %d", pr_type, &N, &M);
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
                edges[tail].push_back(head);
                cost[tail].push_back(weight);
                indegree[head]++;
                break;
            }
            default: break;
        }
    }
}

// Computes Topological ordering of given graph and stores the order in vector topological_ordering(defined globally).
vector <int> topological_ordering;
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
void init_shortest_path_tree()
{
    toposort();
    int l = topological_ordering.size();
    for(int i = 1; i <= N; ++i)
        dist[i] = INF;
    dist[topological_ordering[0]] = 0;
    shortest_path_tree_parent[topological_ordering[0]] = -1; // Source
    for(int i = 0; i < l; ++i){
        int node = topological_ordering[i];
        for(int j = 0; j < edges[node].size(); ++j){
            double updated_dist = dist[node] + cost[node][j];
            if(dist[edges[node][j]] - updated_dist > 0.0000001){
                dist[edges[node][j]] = updated_dist;
                shortest_path_tree_parent[edges[node][j]] = node;
            }
        }
    }
}

// This function transforms the edge weights to positive so that Djikstra's Algorithm can be applied
void update_allgraph_weights()
{
    for(int i = 1; i <= N; ++i){
        for(int j = 0; j < edges[i].size(); ++j){
            cost[i][j] = cost[i][j] + dist[i] - dist[edges[i][j]];
        }
    }
    for(int i = 1; i <= N; ++i)
        dist[i] = 0;
}

// This function extracts the shortest path and stores it in shortest_path vector globally
vector <int> shortest_path;
void extract_shortest_path(){
    shortest_path.clear();
    int curr = N;
    while(curr != 1){
        shortest_path.push_back(curr);
        curr = shortest_path_tree_parent[curr];
    }
    shortest_path.push_back(curr);
}

// This function flips edges along shortest path extracted.
void flip_path()
{
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

        // Add reverse edge. Do not add reverse edge from top sinker to source (otherwise results in formation of cycles)
        if(shortest_path[i] != 1){
            edges[shortest_path[i - 1]].push_back(shortest_path[i]);
            cost[shortest_path[i - 1]].push_back(-c);
            indegree[shortest_path[i]]++;
        }
    }
}

void updateShortestPathTree()
{
    for(int i = 1; i <= N; ++i)
        dist[i] = INF;
    dist[1] = 0;
    multimap < double, int > K;
    K.insert({0, 1});
    while(K.empty() == false){
        int node = (K.begin())->second;
        double c = (K.begin())->first;
        if(node == N)
            break;
        K.erase(K.begin());
        for(int j = 0; j < edges[node].size(); ++j){
            long double upd_dist = dist[node] + cost[node][j];
            if(-0.0000001 > upd_dist - dist[edges[node][j]]){
                dist[edges[node][j]] = upd_dist;
                shortest_path_tree_parent[edges[node][j]] = node;
                K.insert({dist[edges[node][j]], edges[node][j]});
            }
        }
    }
    for(int i = 1; i <= N; ++i){
        if(dist[i] > dist[N])
            dist[i] = dist[N];
    }
}

int main(int argc, char * argv[])
{
    ofstream out("Shortest_Paths.txt");
    char* in_file = argv[2];
    initGraph(in_file);
    init_shortest_path_tree();
    double total_cost = dist[N];
    update_allgraph_weights();
    extract_shortest_path();
    int l = shortest_path.size();
    for(int i = l - 1; i >= 0; --i)
        out << shortest_path[i] << " ";
    out << endl;
    flip_path();
    double curr_cost = total_cost;
    int c = 1;
    while(true)
    {
        updateShortestPathTree();
        curr_cost += dist[N];
        total_cost += curr_cost;
        cout << "Iteration " << (c++) << ": "<<  curr_cost << endl;
        if (curr_cost > -0.0000001) {
            break;
        }
        update_allgraph_weights();
        extract_shortest_path();
        int l = shortest_path.size();
        for(int i = l - 1; i >= 0; --i)
            out << shortest_path[i] << " ";
        out << endl;
        flip_path();
    }
    cout << total_cost << endl;
    return 0;
}