#include <iostream>
#include <vector>
#include <list>
 
typedef unsigned int uint;
 
 
 
class StringHash{
public:
    int size;
    int module;
    StringHash(int size, int mod = 37) : size(size), module(mod) {}
    size_t operator () (const std::string& val) const {
        unsigned long long ans = 0, mod = 1;
        for (char i : val) {
            ans += i * mod;
            ans %= size;
            mod *= module;
            mod %= size;
        }
        return static_cast<size_t>(ans);
    }
};
 
template<typename Tkey, typename Tval, typename Hash>
class MapHashListed {
    std::vector<std::list<std::pair<Tkey, Tval>>> data;
    Hash hash;
public:
    template <typename ...HashArgs>
    explicit MapHashListed(int size, HashArgs... hash_args) : data(size), hash(size, hash_args...) {}
 
    explicit MapHashListed(int size) : data(size), hash(size) {}
 
 
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
 
 
};
 
int main() {
    std::ios_base::sync_with_stdio(false);
    std::cout.tie(nullptr);
    std::cin.tie(nullptr);
    MapHashListed<std::string, int, StringHash> map(1000003);
    std::string S;
    int delta;
    while (std::cin >> S) {
        std::cin >> delta;
        map.plus(S, delta);
    }
 
    return 0;
}
