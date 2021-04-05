#include <iostream>
#include <vector>

struct Vertex{
    bool visit = false;
    bool has_been_visited = false;
    std::vector<int> edges;
};

class Graph {
public:
    explicit Graph(int size) : graph(size){}
    Vertex& operator [] (int place) {
        return graph[place];
    }
    bool dfs_helper(int vertex, std::vector<int>& answer) {
        answer.push_back(vertex);
        graph[vertex].visit = true;
        graph[vertex].has_been_visited = true;
        for (int i = 0; i < graph[vertex].edges.size(); ++i) {
            if (graph[graph[vertex].edges[i]].visit) {
                answer.push_back(graph[vertex].edges[i]);
                graph[vertex].visit = false;
                return true;
            } else if (!(graph[graph[vertex].edges[i]].has_been_visited)) {
                bool success = dfs_helper(graph[vertex].edges[i], answer);
                if (success) {
                    graph[vertex].visit = false;
                    return true;
                }
            }
        }
        answer.pop_back();
        graph[vertex].visit = false;
        return false;
    }

    std::vector<int> dfs_cycle_find() {
        for (int i = 0; i < graph.size(); ++i) {
            if (!graph[i].has_been_visited) {
                std::vector<int> answer;
                bool success = dfs_helper(i, answer);
                if (success) {
                    int j;
                    for (j = 0; j < answer.size(); ++j) {
                        if(answer[j] == answer[answer.size() - 1]) {
                            for (int k = j + 1; k < answer.size(); ++k) {
                                answer[k - j - 1] = answer[k];
                            }
                            answer.resize(answer.size() - j - 1);
                            break;
                        }
                    }
                    return answer;
                }
            }
        }
        return std::vector<int>();
    }

private:
    std::vector<Vertex> graph;
};




int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    int n, m;
    int input, output;
    std::cin >> n >> m;
    Graph graph(n);
    for (int i = 0; i < m; ++i) {
        std::cin >> output >> input;
        --output;
        --input;
        graph[output].edges.push_back(input);
    }
    auto ans = graph.dfs_cycle_find();
    if (ans.empty()) {
        std::cout << "NO";
    } else {
        std::cout << "YES" << '\n';
        for (int i = 0; i < ans.size(); ++i) {
            std::cout << ans[i] + 1 << " ";
        }
    }
    return 0;
}
