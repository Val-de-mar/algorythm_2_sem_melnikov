#include <iostream>
#include <vector>

struct Vertex{
    int time_in;
    int time_out;
    std::vector<int> edges;
};

class Graph {
public:
    int root;
    explicit Graph(int size) : graph(size){}
    Vertex& operator [] (int place) {
        return graph[place];
    }
    void dfs_ancestor_helper(int vertex, int& time) {
        graph[vertex].time_in = time;
        ++ time;
        for (int i = 0; i < graph[vertex].edges.size(); ++i) {
            dfs_ancestor_helper(graph[vertex].edges[i], time);
        }
        graph[vertex].time_out = time;
        ++time;
    }

    void dfs_ancestor_find() {
        int time = 0;
        dfs_ancestor_helper(root, time);
    }

    size_t size() {
        return graph.size();
    }

private:
    std::vector<Vertex> graph;
};




int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    int n, m;
    int output;
    std::cin >> n;
    Graph graph(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> output;
        if (output) {
            --output;
            graph[output].edges.push_back(i);
        } else {
            graph.root = i;
        }
    }
    graph.dfs_ancestor_find();
    std::cin >> m;
    int first_vert, second_vert;
    for (int i = 0; i < m; ++i) {
        std::cin >> first_vert >> second_vert;
        --first_vert;
        --second_vert;
        bool ans = graph[first_vert].time_in < graph[second_vert].time_in &&
                graph[first_vert].time_out > graph[second_vert].time_out;
        std::cout << ans << '\n';
    }
    return 0;
}
