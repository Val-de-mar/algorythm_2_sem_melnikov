#include <iostream>
#include <list>
#include <vector>
#include <exception>
 
 
class HashToShortInt {
public:
    int size;
    HashToShortInt(int size) : size(size) {}
 
    size_t operator()(int val) const {
        return val % size;
    }
 
};
 
 
template<typename Tkey, typename Tval, typename Hash>
class MapHashListed {
    std::vector<std::list<std::pair<Tkey, Tval>>> data;
    Hash hash;
    bool default_val_enable = false;
    Tval* default_val;
public:
    template <typename ...HashArgs>
    explicit MapHashListed(int size, const HashArgs&... hash_args) : data(size), hash(size, hash_args...) {}
 
    explicit MapHashListed(int size) : data(size), hash(size) {}
 
    void enableDefaultValue(const Tval& default_value) {
        default_val = new Tval(default_value);
        default_val_enable = true;
    }
 
    void plus(const Tkey &key, const Tval &val) {
        size_t place = hash(key);
        auto iter = data[place].begin();
        while(key != iter -> first && iter != data[place].end()) ++iter;
        if (iter == data[place].end()) {
            data[place].push_front(std::make_pair(key, val));
        } else {
            data[place].splice(data[place].begin(), data[place], iter);
            data[place].begin()->second += val;
        }
        std::cout << data[place].begin() -> second << '\n';
    }
 
    Tval& operator [] (const Tkey& key) {
        size_t place = hash(key);
        auto iter = data[place].begin();
        while(key != iter -> first && iter != data[place].end()) ++iter;
        if (iter == data[place].end()) {
            if (default_val_enable) {
                data[place].push_front(std::pair<Tkey, Tval>(key,*default_val));
            } else {
                throw std::out_of_range{"Key hasn't been activated by insert and default activation hasn't been activated by enableDefaultValue"};
            }
        } else {
            data[place].splice(data[place].begin(), data[place], iter);
            return data[place].begin()->second;
        }
        return data[place].begin()->second;
    }
 
    ~MapHashListed() {
        if (default_val_enable) delete default_val;
    }
};
 
int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    int n;
    std::cin >> n;
    int a, b, max = 5;
    MapHashListed<int, int, HashToShortInt> map(1000003);
    map.enableDefaultValue(-1);
    for (int i = 0; i < n; ++i) {
        std::cin >> a >> b;
        max = std::max(max, std::max(a, b));
        int &ta = map[a];
        int &tb = map[b];
        if (ta == -1) {
            if (tb == -1) {
                ta = b;
                tb = a;
            } else {
                ta = tb;
                tb = a;
            }
        } else {
            if (tb == -1) {
                tb = ta;
                ta = b;
            } else {
                int t = ta;
                ta = tb;
                tb = t;
            }
        }
        std::cout << std::abs(ta - tb) << '\n';
    }
    return 0;
}
