#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <queue>
#include <list>

using namespace std;
using namespace chrono;

random_device rd;
mt19937 gen(rd());

int best_result = 1;
double deadline = 0;
vector<vector<int>> ordered_edges;
vector<pair<int, int>> best_removed_edges;
auto start_time = steady_clock::now();

void write_output(const string &output_path, int objective) {
    ofstream outfile(output_path);
    outfile << objective << endl;
    for(auto deleted_edge : best_removed_edges)
        outfile << deleted_edge.first << " " << deleted_edge.second << endl;
}

int read_input(const string& input_path, vector<vector<int>> &edges) {
    ifstream inputFile(input_path);
    int n;
    int init_cost = 0;
    inputFile >> n;
    edges.resize(n, vector<int>(5));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < 3; ++j)
            inputFile >> edges[i][j];
        init_cost += edges[i][2];
    }
    return init_cost;
}

bool compare_weight_and_degree(const vector<int> &a, const vector<int> &b) {
    return a[2] - (a[3] + a[4]) > b[2] - (b[3] + b[4]); // compare weights and sum in-degree and out-degree
}

bool compare_weight(const vector<int> &a, const vector<int> &b) {
    return a[2] > b[2]; // compare weights
}

int get_vertices_number(vector<vector<int>> &edges) {
    int vertices_num = 0;
    for (const auto &edge : edges)
        vertices_num = max({vertices_num, edge[0], edge[1]});

    cout << "|V|=" << vertices_num << endl;
    cout << "|E|=" << edges.size() << endl;
    return vertices_num + 1;
}

bool is_cyclic(vector<vector<int>> &graph, int start, int goal) {
    vector<bool> visited(graph.size(), false);

    queue<int> q;
    q.push(start);
    visited[start] = true;

    while (!q.empty()) {
        if (duration<double>(steady_clock::now() - start_time).count() > deadline)
            return true;

        int u = q.front(); q.pop(); // get vertex

        for(auto &neighbor : graph[u]){
            if (!visited[neighbor]){
                if (neighbor == goal)
                    return true;

                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }
    return false;
}

int get_feasible_solution(vector<vector<int>> &graph, vector<vector<int>> &edges, int cost, vector<bool> &deleted_edges){
    for(size_t i = 0; i < edges.size(); i++){
        auto new_edge = edges.at(i);

        // put next edge into the graph
        deleted_edges[i] = false;
        graph[new_edge.at(0)].push_back(new_edge.at(1));

        if(!is_cyclic(graph, new_edge.at(1), new_edge.at(0))){ // if not cyclic, update cost
            cost -= new_edge.at(2);
        } else{ // if graph is cyclic, remove edge and continue
            graph[new_edge.at(0)].pop_back();
            deleted_edges[i] = true;
        }
    }

    // compare local result with the best one and update if local is better
    if (cost < best_result){
        cout << "new result: " << cost << endl;
        best_removed_edges.clear();
        for(size_t i = 0; i < deleted_edges.size(); i++){
            if(deleted_edges[i])
                best_removed_edges.emplace_back(edges[i][0], edges[i][1]);
        }
        best_result = cost;
        ordered_edges = edges;
    }

    return cost;
}

void hill_climbing(vector<vector<int>> &edges, int init_cost, int vertices_num) {
    uniform_int_distribution<int> dist(0, int(edges.size() - 1) );
    vector<vector<int>> new_edges(edges.size());
    new_edges = ordered_edges;
    vector<bool> init_deleted_edges(edges.size(), true);

    while (duration<double>(steady_clock::now() - start_time).count() <= deadline){
        int a = dist(gen); // get random index of edge
        int b = dist(gen); // get second one

        // swap heuristic - swap two random edges
        iter_swap(new_edges.begin() + a, new_edges.begin() + b);

        // init new empty graph
        vector<vector<int>> graph(vertices_num);
        for(auto && edge : init_deleted_edges)
            edge = true;

        int cost = get_feasible_solution(graph, new_edges, init_cost, init_deleted_edges);
        if (cost > best_result) // if cost is worse than start again with the best edges order
            new_edges = ordered_edges;
    }
}

void count_degree(int vertices_num, vector<vector<int>> &edges){
    vector<int> out_degree(vertices_num, 0);
    vector<int> in_degree(vertices_num, 0);
    for(auto &e : edges){
        in_degree[e[1]]++;
        out_degree[e[0]]++;
    }
    for(auto &e : edges){
        e[3] = in_degree[e[0]];
        e[4] = out_degree[e[1]];
    }
}

int main([[maybe_unused]] int argc, char* argv[]) {
    vector<vector<int>> edges;
    int init_cost = read_input(argv[1], edges);
    int vertices_num = get_vertices_number(edges);

    count_degree(vertices_num, edges);

    // deadline depends on the size of the graph
    size_t E = edges.size();
    if (E < 6000){
        deadline = stof(argv[3]) - 0.5;
    } else if (E < 10000){
        deadline = stof(argv[3]) - 1.0;
    } else
        deadline = stof(argv[3]) - 2.0;

    sort(edges.begin(), edges.end(), compare_weight);

    best_result = init_cost;
    ordered_edges = edges;

    vector<vector<int>> graph(vertices_num);
    vector<bool> init_deleted_edges(edges.size(), true);

    // first run in order to get first optimal solution
    get_feasible_solution(graph, edges, init_cost, init_deleted_edges);
    // second run
    sort(edges.begin(), edges.end(), compare_weight_and_degree);
    init_deleted_edges = vector<bool>(edges.size(), true);
    graph = vector<vector<int>>(vertices_num);
    get_feasible_solution(graph, edges, init_cost, init_deleted_edges);

    // start hill climbing algorithm
    hill_climbing(edges, init_cost, vertices_num);

    cout << best_result << endl;
    write_output(argv[2], best_result);

    return 0;
}
