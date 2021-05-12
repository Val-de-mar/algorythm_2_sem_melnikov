#include <iostream>
#include <vector>
#include <set>

typedef unsigned long long ull;

struct Edge {
    int dest;
    long long wei;
    Edge(int dest, int wei) : dest(dest), wei(wei) {}
};

std::vector<std::vector<Edge>> getEdges(int n ,int m) {
    std::vector<std::vector<Edge>> edges(n);
    int in, out, wei;
    for (int i = 0; i < m; ++i) {
        std::cin >> in >> out >> wei;
        --in;
        --out;
        edges[in].emplace_back(out, wei);
        edges[out].emplace_back(in, wei);
    }
    return edges;
}

int main() {
    int n, m;
    int s, t;
    std::cin >> n >> m >> s >> t;
    --s;
    --t;
    std::vector<bool> called(n, false);
    auto edges = getEdges(n ,m);
    std::set<std::pair<long long, int>> verts;
    std::vector<long long> dist(n, INT32_MAX);
    verts.insert(std::make_pair(0ll, s));
    dist[s] = 0;
    while (!verts.empty()) {
        auto [val, cur] = *verts.begin();
        verts.erase(verts.begin());
        if(cur == t) {
            std::cout << dist[t];
            return 0;
        }
        for (auto& i:edges[cur]) {
            if(dist[i.dest] > dist[cur] + i.wei) {
                if (dist[i.dest] != INT32_MAX) {
                    verts.erase(std::make_pair(dist[i.dest], i.dest));
                }
                dist[i.dest] = dist[cur] + i.wei;
                verts.insert(std::make_pair(dist[i.dest], i.dest));
            }
        }
    }
    std::cout << -1;
    return 0;
}
