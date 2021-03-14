#include <iostream>
#include <vector>
#include <list>
#include <cmath>
#include <algorithm>
 
 
typedef short int sint;


class HashToShortInt {
public:
    explicit HashToShortInt(int size) : size_(size) {}

    size_t operator()(int val) const {
        return val % size_;
    }

private:
    int size_;
};
 
template<typename T, typename Hash>
class SetHashListed {
public:
    template <typename ...HashArgs>
    SetHashListed(int size, const HashArgs&... hash_args) : data_(size), hash_(size, hash_args...) {}
 
    SetHashListed(int size) : data_(size), hash_(size) {}
 
    void enableDefaultValue(const T& default_value) {
        default_val_ = new T(default_value);
        default_val_enable_ = true;
    }
 
    void insert(const T &val) {
        size_t place = hash_(val);
        auto iter = data_[place].begin();
        while(val != *iter && iter != data_[place].end()) {
            ++iter;
        }
        if (iter == data_[place].end()) {
            data_[place].push_front(val);
        } else {
            data_[place].splice(data_[place].begin(), data_[place], iter);
            *data_[place].begin() = val;
        }
    }
 
    void erase(const T &val) {
        size_t place = hash_(val);
        auto iter = data_[place].begin();
        while(val != iter && iter != data_[place].end()) {
            ++iter;
        }
        if (iter != data_[place].end()) {
            data_[place].erase(iter);
        }
    }
 
    bool contains(const T &val) {
        size_t place = hash_(val);
        auto iter = data_[place].begin();
        while(val != *iter && iter != data_[place].end()) ++iter;
        if (iter == data_[place].end()) {
            return false;
        } else {
            data_[place].splice(data_[place].begin(), data_[place], iter);
            return true;
        }
    }
 
    ~SetHashListed() {
        if (default_val_enable_) delete default_val_;
    }

private:
    std::vector<std::list<T>> data_;
    Hash hash_;
    bool default_val_enable_ = false;
    T* default_val_;
};

void optimise() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
}

void edgeScan(std::vector<std::pair<sint, sint>>& edges) {
    for (auto & edge : edges) {
        std::cin >> edge.first >> edge.second;
        --edge.first;
        --edge.second;
    }
}

void degreeVertCreate (int n, const std::vector<std::pair<sint, sint>>& edges, std::vector<short int>& order) {
    std::vector<std::pair<sint, sint>> degree_vert(n);
    for (int i = 0; i < degree_vert.size(); ++i) {
        degree_vert[i].first = 0;
        degree_vert[i].second = i;
    }
    for (const auto & edge : edges) {
        --degree_vert[edge.first].first;
        --degree_vert[edge.second].first;
    }
    std::sort(degree_vert.begin(), degree_vert.end());
    for (int i = 0; i < degree_vert.size(); ++i) {
        order[degree_vert[i].second] = i;
    }
    degree_vert.clear();
}

void destCreate(const std::vector<short int>& order, const std::vector<std::pair<sint, sint>>& edges, std::vector<std::vector<short int>>& dest_run, std::vector<SetHashListed<short int, HashToShortInt>>& dest_find) {
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
}

void moneyEvolve (std::vector<std::vector<short int>>& money, std::vector<std::vector<short int>>& dest_run) {
    for (int i = money.size() - 1; i >= 0; --i) {
        int size = money[i].size();
        for (auto & next : dest_run[i]) {
            int curr_size = money[next].size();
            money[next].resize(size + curr_size);
            std::copy(money[i].begin(), money[i].end(), money[next].begin() + curr_size);
        }
        money[i].clear();
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
}

 
int main() {
    optimise();

    int n, m;
    std::cin >> n >> m;
 
    std::vector<std::pair<sint, sint>> edges(m);
    edgeScan(edges);

    std::vector<short int> order(n);
    degreeVertCreate(n, edges, order);
 
    std::vector<SetHashListed<short int, HashToShortInt>> dest_find(n, 2 * int(std::sqrt(m + 10)));
    std::vector<std::vector<short int>> dest_run(n);

    destCreate(order, edges, dest_run, dest_find);
 
    std::vector<std::vector<short int>> money(n);
    for (int i = 0; i < money.size(); ++i) {
        money[i].push_back(i);
    }
 
    moneyEvolve(money, dest_run);
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
