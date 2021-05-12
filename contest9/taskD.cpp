#include <iostream>
#include <vector>
#include <set>

typedef unsigned long long ull;

struct Edge {
    int dest;
    long long wei;

    Edge(int dest, long long wei) : dest(dest), wei(wei) {}
};

void dfsMarker(int vert, const std::vector<std::vector<Edge>> &edges, std::vector<bool> &has_min) {
    has_min[vert] = false;
    for (const auto &edge: edges[vert]) {
        if (has_min[edge.dest]) {
            dfsMarker(edge.dest, edges, has_min);
        }
    }
}

void edgesScanning(std::vector<std::vector<Edge>> &edges, int m) {
    int in, out;
    long long wei;
    for (int i = 0; i < m; ++i) {
        std::cin >> in >> out >> wei;
        --in, --out;
        edges[in].emplace_back(out, wei);
    }
}

void algorithmRun(std::vector<std::vector<Edge>> &edges, std::vector<long long> &dist, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (dist[j] != INT64_MAX) {
                for (auto &edge: edges[j]) {
                    if (dist[edge.dest] > dist[j] + edge.wei) {
                        dist[edge.dest] = dist[j] + edge.wei;
                    }
                }
            }
        }
    }
}

std::vector<bool> markBadVerts(std::vector<std::vector<Edge>> &edges,
                               std::vector<long long> &dist, int n) {
    std::vector<bool> has_min_path(n, true);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (dist[j] != INT64_MAX) {
                for (auto &edge: edges[j]) {
                    if (dist[edge.dest] > dist[j] + edge.wei) {
                        dist[edge.dest] = dist[j] + edge.wei;
                        has_min_path[edge.dest] = false;
                    }
                }
            }
        }
    }
    return has_min_path;
}

void printResult(const std::vector<bool> &has_min_path, const std::vector<bool> &has_path,
                 const std::vector<long long> &dist, int n) {

    for (int i = 0; i < n; ++i) {
        if (!has_path[i]) {
            std::cout << "*\n";
        } else if (has_min_path[i]) {
            std::cout << dist[i] << '\n';
        } else {
            std::cout << "-\n";
        }
    }
}


int main() {
    int n, m, s;
    std::cin >> n >> m >> s;
    --s;
    std::vector<std::vector<Edge>> edges(n);
    std::vector<long long> dist(n, INT64_MAX);
    dist[s] = 0;
    edgesScanning(edges, m);
    algorithmRun(edges, dist, n);
    std::vector<bool> has_path(n, true);
    for (int i = 0; i < n; ++i) {
        if (dist[i] == INT64_MAX) {
            has_path[i] = false;
        }
    }
    auto has_min_path = markBadVerts(edges, dist, n);
    for (int i = 0; i < n; ++i) {
        if (!has_min_path[i]) {
            dfsMarker(i, edges, has_min_path);
        }
    }
    printResult(has_min_path, has_path, dist, n);
    return 0;
}
