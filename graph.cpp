#include "graph.h"
#include<iostream>
#include<string>
#include"cities.h"
#include "minHeap.h" 
#include"list.h"
using namespace std;


Graph::Graph(int size) : capacity(size > 0 ? size : 100), total_cities(0) {
    
    adj_list = new List<Edge>[capacity];
}

void Graph::add_road(int u, int v, int weight) {
    
    if (u < 0 || v < 0) {
        return;
    }

    if (u > 2000 || v > 2000) {
        cout << ">> Error: City ID too high for infrastructure." << endl;
        return;
    }

    while (u >= capacity || v >= capacity) {
        int next_size = (capacity == 0) ? 10 : capacity * 2;
        resize(next_size);
    }

    if (adj_list != nullptr) {
        adj_list[u].insert(Edge(v, weight));
        adj_list[v].insert(Edge(u, weight));
    }
}
Graph::~Graph() {
    delete[] adj_list;
}

istream& operator>>(istream& is, Graph& g) {
    string line;
    while (getline(is, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string u, v, w, b;


        if (getline(ss, u, ',') &&
            getline(ss, v, ',') &&
            getline(ss, w, ',') &&
            getline(ss, b)) {

            int u1 = stoi(u);
            int v1 = stoi(v);
            int weight = stoi(w);
            bool blocked = (stoi(b) == 1);


            g.add_road(u1, v1, weight);

            if (blocked) {
                g.toggle_road_block(u1, v1, true);
            }
        }
    }
    return is;
}



void Graph::resize(int new_capacity) {

    if (new_capacity > 5000) {
        return;
    }

    List<Edge>* new_array = new List<Edge>[new_capacity];

    for (int i = 0; i < capacity; i++) {
        for (int j = 0; j < adj_list[i].size(); j++) {
            new_array[i].insert(adj_list[i].get_at(j));
        }
    }

    delete[] adj_list;
    adj_list = new_array;
    capacity = new_capacity;
}

void Graph::toggle_road_block(int u, int v, bool status) {
    if (u >= capacity || v >= capacity) return;

    for (int i = 0; i < adj_list[u].size(); i++) {
        Edge& e = adj_list[u].get_at_ref(i); 
        if (e.to_city == v) e.is_blocked = status;
    }
    for (int i = 0; i < adj_list[v].size(); i++) {
        Edge& e = adj_list[v].get_at_ref(i);
        if (e.to_city == u) e.is_blocked = status;
    }
}

void Graph::calculate_shortest_path(int start, int end, List<int>& path) {
    if (start < 0 || end < 0 || start >= capacity || end >= capacity) return;

    int infinity = 2147483647;

    int* dist = new int[capacity];
    int* parent = new int[capacity];
    bool* visited = new bool[capacity];

    for (int i = 0; i < capacity; i++) {
        dist[i] = infinity;
        parent[i] = -1;
        visited[i] = false;
    }

    MinHeap<Edge> pq(capacity * 2); 
    dist[start] = 0;
    pq.insert(Edge(start, 0));

    while (!pq.is_empty()) {
        Edge current = pq.extract_min();
        int u = current.to_city;

        if (visited[u]) continue;
        visited[u] = true;
        if (u == end) break;

        for (int i = 0; i < adj_list[u].size(); i++) {
            Edge e = adj_list[u].get_at(i);
            if (e.is_blocked) continue;

            if (dist[u] + e.weight < dist[e.to_city]) {
                dist[e.to_city] = dist[u] + e.weight;
                parent[e.to_city] = u;
                pq.insert(Edge(e.to_city, dist[e.to_city]));
            }
        }
    }

    if (dist[end] != infinity) {
        Stack<int> path_stack;
        for (int at = end; at != -1; at = parent[at]) {
            path_stack.push(at);
        }
        while (!path_stack.is_empty()) {
            path.insert(path_stack.pop());
        }
    }

    delete[] dist;
    delete[] parent;
    delete[] visited;
}


void Graph::display() {
    cout << "\n ========== ROAD MAP ==========" << endl;
    for (int i = 0; i < capacity; i++) {
        if (!adj_list[i].is_empty()) {
            cout << global_cities.get_name(i) << " connects to: " << endl;
            for (int j = 0; j < adj_list[i].size(); j++) {
                Edge e = adj_list[i].get_at(j);
                cout << "  -> " << global_cities.get_name(e.to_city)
                    << " (" << e.weight << "km)" << (e.is_blocked ? " [BLOCKED]" : "") << endl;
            }
        }
    }
    cout << "\n ==============================" << endl;
}

ostream& operator<<(ostream& os, const Graph& g) {
    for (int i = 0; i < g.capacity; i++) {
        for (int j = 0; j < g.adj_list[i].size(); j++) {
            Edge e = g.adj_list[i].get_at(j);
            if (i < e.to_city) {
                os << i << "," << e.to_city << "," << e.weight << "," << (e.is_blocked ? 1 : 0) << endl;
            }
        }
    }
    return os;
}

bool Graph::is_road_blocked(int u, int v) {
    if (u < 0 || v < 0 || u >= capacity || v >= capacity) return true;
    for (int i = 0; i < adj_list[u].size(); i++) {
        Edge e = adj_list[u].get_at(i);
        if (e.to_city == v) return e.is_blocked;
    }
    return false;
}