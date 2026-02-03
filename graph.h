#pragma once
#include "list.h"
#include "stack.h"
#include "cities.h"
using namespace std;
//0=open 1=blocked
struct Edge {
    int to_city;
    int weight;
    bool is_blocked;

    Edge() : to_city(-1), weight(0), is_blocked(false) {}
    Edge(int v, int w) : to_city(v), weight(w), is_blocked(false) {}


    bool operator<(const Edge& other) const {
        return this->weight < other.weight;
    }
    bool operator>(const Edge& other) const {
        return this->weight > other.weight;
    }
};
class Graph {
public:
    List<Edge>* adj_list;
    int capacity; 
    int total_cities;

    Graph(int initial_size = 50);
    ~Graph(); 

    void resize(int new_capacity); 

    
    void add_road(int u, int v, int weight);
    void toggle_road_block(int u, int v, bool status);
    void calculate_shortest_path(int start, int end, List<int>& path_output);
    void display();
    bool is_road_blocked(int u, int v);

    friend istream& operator>>(istream& is, Graph& g);
    friend ostream& operator<<(ostream& os, const Graph& g);
};