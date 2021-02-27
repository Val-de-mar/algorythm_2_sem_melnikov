#include <iostream>
#include <vector>
#include <list>
#include <cmath>
#include <algorithm>
 
 
typedef short int sint;
 
 
class HashToShortInt {
public:
    int size;
    HashToShortInt(int size) : size(size) {}
 
    size_t operator()(int val) const {
        return val % size;
    }
 
};
 
template<typename T, typename Hash>
class SetHashListed {
    std::vector<std::list<T>> data;
    Hash hash;
    bool default_val_enable = false;
    T* default_val;
public:
    template <typename ...HashArgs>
    SetHashListed(int size, const HashArgs&... hash_args) : data(size), hash(size, hash_args...) {}
 
    SetHashListed(int size) : data(size), hash(size) {}
 
    void enableDefaultValue(const T& default_value) {
        default_val = new T(default_value);
        default_val_enable = true;
    }
 
    void insert(const T &val) {
        size_t place = hash(val);
        auto iter = data[place].begin();
        while(val != *iter && iter != data[place].end()) ++iter;
        if (iter == data[place].end()) {
            data[place].push_front(val);
        } else {
            data[place].splice(data[place].begin(), data[place], iter);
            *data[place].begin() = val;
        }
    }
 
    void erase(const T &val) {
        size_t place = hash(val);
        auto iter = data[place].begin();
        while(val != iter && iter != data[place].end()) ++iter;
        if (iter != data[place].end()) {
            data[place].erase(iter);
        }
    }
 
    bool contains(const T &val) {
        size_t place = hash(val);
        auto iter = data[place].begin();
        while(val != *iter && iter != data[place].end()) ++iter;
        if (iter == data[place].end()) {
            return false;
        } else {
            data[place].splice(data[place].begin(), data[place], iter);
            return true;
        }
    }
 
    ~SetHashListed() {
        if (default_val_enable) delete default_val;
    }
};
 
 
 
int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
 
    int n, m;
 
    std::cin >> n >> m;
 
    std::vector<std::pair<sint, sint>> edges(m);
    for (auto & edge : edges) {
        std::cin >> edge.first >> edge.second;
        --edge.first;
        --edge.second;
    }
 
    std::vector<std::pair<sint, sint>> degree_vert(n);
    for (int i = 0; i < degree_vert.size(); ++i) {
        degree_vert[i].first = 0;
        degree_vert[i].second = i;
    }
    for (auto & edge : edges) {
        --degree_vert[edge.first].first;
        --degree_vert[edge.second].first;
    }
    std::sort(degree_vert.begin(), degree_vert.end());
 
    std::vector<short int> order(n);
    for (int i = 0; i < degree_vert.size(); ++i) {
        order[degree_vert[i].second] = i;
    }
    degree_vert.clear();
 
    std::vector<SetHashListed<short int, HashToShortInt>> dest_find(n, 2 * int(std::sqrt(m + 10)));
    std::vector<std::vector<short int>> dest_run(n);
 
    for (auto & edge : edges) {
        auto & fir = order[edge.first];
        auto & sec = order[edge.second];
        if (fir < sec) {
            dest_run[fir].push_back(sec);
            dest_find[fir].insert(sec);
        } else {
            dest_run[sec].push_back(fir);
            dest_find[sec].insert(fir);
        }
    }
    
 
    std::vector<std::vector<short int>> money(n);
    for (int i = 0; i < money.size(); ++i) {
        money[i].push_back(i);
    }
 
    for (int i = money.size() - 1; i >= 0; --i) {
        int size = money[i].size();
        for (auto & next : dest_run[i]) {
            int curr_size = money[next].size();
            money[next].resize(size + curr_size);
            std::copy(money[i].begin(), money[i].end(), money[next].begin() + curr_size);
        }
        money[i].clear();
    }
 
//    for (int i = 0; i < money.size(); ++i) {
//        for (int j = 0; j < money[i].size(); ++j) {
//            std::cout << money[i][j] << ' ';
//        }
//        std::cout << std::endl;
//    }
 
    for (int i = money.size() - 1; i >= 0; --i) {
        int size = money[i].size();
        for (auto & next : dest_run[i]) {
            int curr_size = money[next].size();
            money[next].resize(size + curr_size);
            std::copy(money[i].begin(), money[i].end(), money[next].begin() + curr_size);
        }
        money[i].clear();
    }
    long long result = 0;
 
    for (int i = 0; i < money.size(); ++i) {
        for (int j = 0; j < money[i].size(); ++j) {
            if(dest_find[money[i][j]].contains(i)) ++result;
        }
    }
    std::cout.setf(std::ios::fixed);
    std::cout << double(result) / 4.0;
 
    return 0;
}
