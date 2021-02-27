#include <iostream>
#include <list>
#include <vector>
#include <exception>
#include <fstream>
 
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
 
    void insert(const Tkey &key, const Tval &val) {
        size_t place = hash(key);
        auto iter = data[place].begin();
        while(key != iter -> first && iter != data[place].end()) ++iter;
        if (iter == data[place].end()) {
            data[place].push_front(std::make_pair(key, val));
        } else {
            data[place].splice(data[place].begin(), data[place], iter);
            data[place].begin()->second = val;
        }
    }
 
    void erase(const Tkey &key) {
        size_t place = hash(key);
        auto iter = data[place].begin();
        while(key != iter -> first && iter != data[place].end()) ++iter;
        if (iter != data[place].end()) {
            data[place].erase(iter);
        }
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
    MapHashListed<std::string, std::string, StringHash> map(1000003);
    std::string command, key, val;
    std::ifstream fin;
    fin.open("map.in");
    std::ofstream fout;
    fout.open("map.out");
    while (fin >> command) {
        switch (command[0]) {
            case 'p':
                fin >> key >> val;
                map.insert(key, val);
                break;
            case 'g':
                fin >> key;
                try {
                    fout << map[key] << '\n';
                } catch (...) {
                    fout << "none" << '\n';
                }
                break;
            case 'd':
                fin >> key;
                map.erase(key);
                break;
        }
    }
 
    return 0;
}
