#include <iostream>
#include <vector>
#include <list>



struct Vertex{
    int number;
    std::vector<int> edges;
};



class Graph {
public:

    Vertex& operator [] (size_t place) {
        return graph[place];
    }
    const Vertex& operator[] (size_t place) const {
        return graph[place];
    }

    std::list<int> subcycle(int vertex) {
        std::list<int> ans;
        while (true) {
            ans.push_back(vertex);
            if (graph[vertex].edges.empty()){
                return ans;
            } else {
                int next_v = graph[vertex].edges.back();
                graph[vertex].edges.pop_back();
                vertex = next_v;
            }
        }
    }

    std::list<int> cycle(int vertex) {
        std::list<int> ans;
        ans.push_back(vertex);
        auto iter = ans.begin();
        while (true){
            while(!graph[*iter].edges.empty()) {
                std::list<int> add = subcycle(*iter);
                add.erase(add.begin());
                auto next = iter;
                ++next;
                ans.splice(next, add);
            }
            ++iter;
            if (iter == ans.end()){
                break;
            }
        }
        return ans;
    }

    explicit Graph(size_t size) : graph(size) {
        for (int i = 0; i < size; ++i) {
            graph[i].number = i;
        }
    }
private:
    std::vector<Vertex> graph;
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    int n, position;
    std::cin >> n >> position;
    --position;
    int scan;
    Graph graph(n);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            std::cin >> scan;
            if (!scan && i != j) {
                graph[i].edges.emplace_back(j);
            }
        }
    }
    auto ans = graph.cycle(position);
    auto it1 = ans.begin();
    auto it2 = ans.begin();
    ++it2;
    while (it2 != ans.end()){
        std::cout << *it1 + 1 << " " << *it2 + 1<< "\n";
        ++it1;
        ++it2;
    }

    return 0;
}
