#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <functional>
std::string tf(bool val) {
    return val?"true":"false";
}

std::string str(const std::vector<int>& val) {
    std::string ans(val.size(), '\0');
    for (int i = 0; i < val.size(); ++i) {
        ans[i] = static_cast<char>(val[i]);
    }
    return ans;
}

std::string changed(std::string val, int begin, int end) {
    std::reverse(val.begin() + begin, val.begin() + end);
    return val;
}

struct Vertex {
    long long dist_begin;
    long long dist_end;
    Vertex(): dist_begin(INT64_MAX), dist_end(INT64_MAX) {}
    Vertex(long long right, long long left): dist_begin(right), dist_end(left) {}
};

bool beginStep(std::queue<std::string>& begins, std::unordered_map<std::string, Vertex>& map, int n) {
    if (begins.empty()) {
        throw std::invalid_argument("begins is empty");
    }
    std::string& current = begins.front();
    Vertex &cvert = map[begins.front()];
    if (cvert.dist_end != INT64_MAX) {
        std::cout << cvert.dist_begin + cvert.dist_end;
        return true;
    }
    for (int j = 0; j <= n; ++j) {
        for (int i = 0; i < j - 1; ++i) {
            std::string next = changed(current, i, j);
            auto &nvert = map[next];
            if (nvert.dist_begin > cvert.dist_begin + 1) {
                nvert.dist_begin = cvert.dist_begin + 1;
                begins.push(next);
            }
            if (nvert.dist_end != INT64_MAX) {
                std::cout << nvert.dist_begin + nvert.dist_end;
                return true;
            }
        }
    }
    begins.pop();
    return false;
}


bool endStep(std::queue<std::string>& ends, std::unordered_map<std::string, Vertex>& map, int n) {
    if (ends.empty()) {
        throw std::invalid_argument("ends is empty");
    }
    std::string current = ends.front();
    Vertex &cvert = map[ends.front()];
    if (cvert.dist_begin != INT64_MAX) {
        std::cout << cvert.dist_begin + cvert.dist_end;
        return true;
    }
    for (int j = 0; j <= n; ++j) {
        for (int i = 0; i < j - 1; ++i) {
            std::string next = changed(current, i, j);
            auto &nvert = map[next];
            if (nvert.dist_end > cvert.dist_end + 1) {
                nvert.dist_end = cvert.dist_end + 1;
                ends.push(next);
            }
            if (nvert.dist_begin != INT64_MAX) {
                std::cout << nvert.dist_begin + nvert.dist_end;
                return true;
            }
        }
    }
    ends.pop();
    return false;
}


int main() {
    int n;
    std::cin >> n;
    std::queue<std::string> begins, ends;
    std::vector<int> s(n), t(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> s[i];
    }
    for (int i = 0; i < n; ++i) {
        std::cin >> t[i];
    }
    std::unordered_map<std::string, Vertex> map;
    map[str(s)].dist_begin = 0;
    map[str(t)].dist_end = 0;
    begins.push(str(s));
    ends.push(str(t));
    while(true) {
        if (beginStep(begins, map, n)) {
            return 0;
        }
        if (endStep(ends, map, n)) {
            return 0;
        }
    }

    return 0;
}
