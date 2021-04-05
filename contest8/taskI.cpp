#include <iostream>
#include <vector>

struct Vertex{
    int time_in;    //ремя входа
    int characteristic; //ret
    bool visited = false;   //были ли в вершине
    bool joint = false; //сочленение ли
    std::vector<int> edges;     //куда велут ребра
};

class Graph {
public:
    explicit Graph(int size) : graph(size){}
    Vertex& operator [] (int place) {
        return graph[place];
    }
    int dfs_helper(int vertex, int previous) {
        if (graph[vertex].visited) {
            return graph[vertex].characteristic;
        }
        ++ time;
        graph[vertex].time_in = time;
        graph[vertex].visited = true;
        graph[vertex].characteristic = graph[vertex].time_in;
        int pre_char = time;
        for (int i = 0; i < graph[vertex].edges.size(); ++i) {
            if (graph[vertex].edges[i] != previous) {
                bool not_descendant = !graph[graph[vertex].edges[i]].visited;
                int possible = dfs_helper(graph[vertex].edges[i], vertex);

                if (possible >= graph[vertex].time_in && not_descendant) {
                    graph[vertex].joint = true;
                }
                pre_char = std::min(pre_char, possible);
            }
        }
        ++time;
        graph[vertex].characteristic = pre_char;
        return graph[vertex].characteristic;
    }

    void dfs_root_helper(int vertex){
        ++time;
        graph[vertex].time_in = time;
        graph[vertex].visited = true;
        graph[vertex].characteristic = graph[vertex].time_in;
        int counter = 0;
        for (int i = 0; i < graph[vertex].edges.size(); ++i) {
            if (!graph[graph[vertex].edges[i]].visited) {
                dfs_helper(graph[vertex].edges[i], vertex);
                ++counter;
            }
        }
        if (counter > 1) {
            graph[vertex].joint = true;
        }
    }

    void dfs() {
        for (int i = 0; i < graph.size(); ++i) {
            if (!graph[i].visited) {
                dfs_root_helper(i);
            }
        }
    }

    size_t size() {
        return graph.size();
    }

private:
    std::vector<Vertex> graph;
    int time = 0;
};




int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    int n, m;
    int output, input;
    std::vector<std::pair<int, int>> edges;
    std::cin >> n >> m;
    Graph graph(n);
    for (int i = 0; i < m; ++i) {
        std::cin >> input >> output;
        --input;
        --output;
        graph[output].edges.push_back(input);
        graph[input].edges.push_back(output);
        edges.emplace_back(input, output);
    }               //ввод графа закончен
    graph.dfs();
    std::vector<int> ans;
    for (int i = 0; i < n; ++i) {
        if (graph[i].joint) {
            ans.push_back(i + 1);
        }
    }
    std::cout << ans.size() << '\n';
    for (int i = 0; i < ans.size(); ++i) {
        std::cout << ans[i] << ' ';
    }
    return 0;
}
