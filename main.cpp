#include "bits/stdc++.h"
using namespace std;
const int MAXN = 1e5 + 5;
int N, M; // Number of nodes, 
vector <int> edges[MAXN];
vector <long double> cost[MAXN];
long double dist[MAXN];
int shortest_path_tree_parent[MAXN];

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
            case 'c':                  /* skip lines with comments */
            case '\n':                 /* skip empty lines   */
            case 'n':
            case '\0':                 /* skip empty lines at the end of file */
                break;
            case 'p':
            case 'a': {
                int tail; int head;
                double weight;
                sscanf(line.c_str(), "%*c %d %d %lf", &tail, &head, &weight);
                edges[tail].push_back(head);
                cost[tail].push_back(weight);
                break;
            }
            default:
                break;
        }
    }
}

void init_shortest_path_tree()
{
    
}

int main(int argc, char * argv[])
{
    char* in_file = argv[2];
    initGraph(in_file);
    init_shortest_path_tree();
    return 0;
}